/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/6/6.
//

#include "sql/stmt/select_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "join_stmt.h"

SelectStmt::~SelectStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
  if (nullptr != having_stmt_) {
    delete having_stmt_;
    having_stmt_ = nullptr;
  }
}

static void wildcard_fields(Table *table, std::vector<std::unique_ptr<Expression>> &field_metas)
{
  const TableMeta &table_meta = table->table_meta();
  const int        field_num  = table_meta.field_num();
  for (int i = table_meta.sys_field_num() + table_meta.null_field_num(); i < field_num; i++) {
    field_metas.push_back(std::make_unique<FieldExpr>(table, table_meta.field(i)));
  }
}
RC SelectStmt::create(Db *db, SelectSqlNode &select_sql, Stmt *&stmt)
{
  RC rc = RC::SUCCESS;
  if (nullptr == db) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }

  // collect tables in `from` statement
  std::vector<Table *> tables;
  TableMap table_map;
  for (size_t i = 0; i < select_sql.relations.size(); i++) {
    const char *table_name = select_sql.relations[i].first.c_str();
    let_ref     alias      = select_sql.relations[i].second;
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. relation name is null. index=%d", i);
      return RC::INVALID_ARGUMENT;
    }

    Table *table = db->find_table(table_name);
    if (nullptr == table) {
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    tables.push_back(table);
    table_map.insert(std::make_pair(table_name, table_entry{table, i}));
    if (!alias.empty()) {
      table_map.insert(std::make_pair(alias, table_entry{table, i}));
    }
  }
  std::vector<JoinStmt *> join_stmts;
  for (size_t i = 0; i < select_sql.join_list.conditions.size(); i++) {
    FilterTree *filter_tree = nullptr;
    RC rc = JoinStmt::create(db, tables[i], tables[i + 1], &table_map, select_sql.join_list.conditions[i], filter_tree);
    JoinStmt *join_stmt = new JoinStmt(std::unique_ptr<FilterTree>(filter_tree));
    if (rc != RC::SUCCESS) {
      return rc;
    }
    join_stmts.push_back(join_stmt);
  }

  // collect query fields in `select` statement
  //  std::vector<Field>                       query_fields;
  std::vector<AggregateExpr>               aggregates;
  std::vector<Field>                       group_bys;
  std::vector<std::unique_ptr<Expression>> query_exprs;

  auto push_aggregate = [&](AggregateExpr *aggr_expr, Table *table) {
    aggr_expr->set_table(table);
    aggregates.push_back(*aggr_expr);
    aggregates.back().set_name(aggr_expr->name());

    //    auto expr = std::make_unique<AggregateExpr>(aggr_expr->node(), table);
    //    expr->set_name(aggr_expr->name());
    //    query_exprs.push_back(std::move(expr));
  };
  for (const auto &expr : select_sql.group_by.exprs) {
    // TODO: currently only group by fields!
    auto *field_expr = dynamic_cast<FieldExpr *>(expr.get());

    Table *table       = tables[0];
    auto   found_table = table_map.find(field_expr->node().relation_name);
    if (found_table != table_map.end()) {
      table = found_table->second.table;
    }
    auto meta = table->table_meta().field(field_expr->node().attribute_name.c_str());
    group_bys.emplace_back(table, meta);
  }

  auto process_aggregate = [&](AggregateExpr *aggr_expr) {
    // e.g. MIN(*)
    if (strcmp(aggr_expr->field_name(), "*") == 0 &&
        (aggr_expr->aggr_func() != AggregateFunc::Count || aggr_expr->aggr_func() != AggregateFunc::CountStar)) {
      return RC::SQL_SYNTAX;
    }

    // find table
    bool   field_exist = false;
    Table *table       = tables[0];
    {
      auto found_table = table_map.find(aggr_expr->table_name());
      if (found_table != table_map.end()) {
        table = found_table->second.table;
      }
    }

    // find field
    for (const auto &field_meta : *table->table_meta().field_metas()) {
      if (strcmp(field_meta.name(), aggr_expr->field_name()) == 0) {
        field_exist = true;
        break;
      }
    }
    if (!field_exist && aggr_expr->aggr_func() != AggregateFunc::CountStar) {
      return RC::SCHEMA_FIELD_NOT_EXIST;
    }

    // TODO: single table has no need for table constraint
    if (tables.size() > 1) {
      const_cast<std::string &>(aggr_expr->node().attr.relation_name) = table->name();
    }

    push_aggregate(aggr_expr, table);

    return RC::SUCCESS;
  };

  let is_aggregate = std::ranges::any_of(
      select_sql.select_exprs, [&](auto &e) { return dynamic_cast<AggregateExpr *>(e.get()) != nullptr; });

  for (int i = static_cast<int>(select_sql.select_exprs.size()) - 1; i >= 0; i--) {
    auto &selected_expr = select_sql.select_exprs[i];

    if (auto *field_expr = dynamic_cast<FieldExpr *>(selected_expr.get())) {
      const auto &relation_attr = field_expr->node();

      if (common::is_blank(relation_attr.relation_name.c_str()) &&
          0 == strcmp(relation_attr.attribute_name.c_str(), "*")) {

        let_mut field_index = query_exprs.size();
        let_mut table_index = 0;
        for (Table *table : tables) {
          wildcard_fields(table, query_exprs);
          for (; field_index < query_exprs.size(); field_index++) {
            dynamic_cast<FieldExpr *>(query_exprs[field_index].get())->table_index = table_index;
          }
          ++table_index;
        }
      } else if (!common::is_blank(relation_attr.relation_name.c_str())) {
        const char *table_name = relation_attr.relation_name.c_str();
        const char *field_name = relation_attr.attribute_name.c_str();

        if (0 == strcmp(table_name, "*")) {
          if (0 != strcmp(field_name, "*")) {
            LOG_WARN("invalid field name while table is *. attr=%s", field_name);
            return RC::SCHEMA_FIELD_MISSING;
          }
          let_mut field_index = query_exprs.size();
          let_mut table_index = 0;
          for (Table *table : tables) {
            wildcard_fields(table, query_exprs);
            for (; field_index < query_exprs.size(); field_index++) {
              dynamic_cast<FieldExpr *>(query_exprs[field_index].get())->table_index = table_index;
            }
            ++table_index;
          }
        } else {
          int    table_index = 0;
          Table *table       = nullptr;
          let    iter        = table_map.find(table_name);
          if (iter == table_map.end()) {
            LOG_WARN("no such table in from list: %s", table_name);
            return RC::SCHEMA_FIELD_MISSING;
          }
          table       = iter->second.table;
          table_index = (int)iter->second.table_index;

          if (0 == strcmp(field_name, "*")) {
            let_mut field_index = query_exprs.size();
            wildcard_fields(table, query_exprs);
            for (; field_index < query_exprs.size(); field_index++) {
              dynamic_cast<FieldExpr *>(query_exprs[field_index].get())->table_index = table_index;
            }
          } else {
            const FieldMeta *field_meta = table->table_meta().field(field_name);
            if (nullptr == field_meta) {
              LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), field_name);
              return RC::SCHEMA_FIELD_MISSING;
            }

            //            query_exprs.push_back(std::make_unique<FieldExpr>(table, field_meta));
            //            query_exprs.back()->set_name(field_expr->name());
            field_expr->set_table(table);
            field_expr->table_index = table_index;
            query_exprs.push_back(std::move(selected_expr));
          }
        }
      } else {
        if (tables.size() != 1) {
          LOG_WARN("invalid. I do not know the attr's table. attr=%s", relation_attr.attribute_name.c_str());
          return RC::SCHEMA_FIELD_MISSING;
        }

        Table           *table      = tables[0];
        const FieldMeta *field_meta = table->table_meta().field(relation_attr.attribute_name.c_str());
        if (nullptr == field_meta) {
          LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), relation_attr.attribute_name.c_str());
          return RC::SCHEMA_FIELD_MISSING;
        }

        field_expr->set_table(table);
        query_exprs.push_back(std::move(selected_expr));
        //        query_exprs.push_back(std::make_unique<FieldExpr>(table, field_meta));
        //        query_exprs.back()->set_name(field_expr->name());
        //      query_fields.push_back(Field(table, field_meta));
      }
    } else {
      //      if (auto* arithmetic_expr = dynamic_cast<ArithmeticExpr*>(select_expr.get())) {
      //
      //      }

      {
        rc = traverse_expressions<AggregateExpr>(selected_expr.get(), Order::PreOrder, [&](AggregateExpr *aggr_expr) {
          return process_aggregate(aggr_expr);
        });
        if (rc != RC::SUCCESS) {
          return rc;
        }
      }

      {
        rc = traverse_expressions<FieldExpr>(selected_expr.get(), Order::PreOrder, [&](FieldExpr *field_expr) {
          RC               rc         = RC::SUCCESS;
          Table           *table      = nullptr;
          const FieldMeta *field_meta = nullptr;
          rc = get_table_and_field(db, tables[0], &table_map, field_expr->node(), table, field_meta);
          if (rc != RC::SUCCESS) {
            LOG_WARN("cannot find attr");
            return rc;
          }
          field_expr->field() = Field(table, field_meta);
          return rc;
        });
        if (rc != RC::SUCCESS) {
          return rc;
        }
      }
      query_exprs.push_back(std::move(selected_expr));
    }
  }

  LOG_INFO("got %d tables in from stmt and %d fields in query stmt", tables.size(), query_exprs.size());

  Table *default_table = nullptr;
  if (tables.size() == 1) {
    default_table = tables[0];
  }

  // create filter statement in `where` statement
  FilterTree *root     = nullptr;
  let         sql_node = container_of(&select_sql, ParsedSqlNode, selection);
  {
    rc = FilterStmt::create(db, default_table, &table_map, select_sql.conditions.get(), sql_node, root);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot construct filter stmt");
      return rc;
    }
  }
  let filter_stmt = new FilterStmt(std::unique_ptr<FilterTree>(root));

  // create filter statement in `having` statement;
  root = nullptr;
  {
    rc = FilterStmt::create(db, default_table, &table_map, select_sql.having.get(), sql_node, root);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot construct filter stmt");
      return rc;
    }
  }
  let having_stmt = new FilterStmt(std::unique_ptr<FilterTree>(root));
  {
    rc = traverse_filter_tree(having_stmt->filter_tree(), [&](FilterUnit *unit) {
      RC  rc           = RC::SUCCESS;
      let traverse_agg = [&](Expression *expr) {
        return traverse_expressions<AggregateExpr>(
            expr, Order::PreOrder, [&](AggregateExpr *agg) { return process_aggregate(agg); });
      };
      rc = traverse_agg(unit->left().expr.get());
      if (rc != RC::SUCCESS) {
        return rc;
      }
      rc = traverse_agg(unit->right().expr.get());
      if (rc != RC::SUCCESS) {
        return rc;
      }

      return rc;
    });
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }

  let exist_in_group_bys = [&](FieldExpr *field) {
    return std::ranges::any_of(group_bys, [&field](const auto &group_by) {
      return group_by.table() == field->table() && group_by.meta() == field->field_meta();
    });
  };

  if (is_aggregate && std::ranges::any_of(query_exprs, [&](let_ref query_expr) {
        bool exist = true;
        traverse_expressions<FieldExpr>(query_expr.get(), Order::PreOrder, [&](FieldExpr *field_expr) {
          if (!exist_in_group_bys(field_expr)) {
            exist = false;
          }
          return RC::SUCCESS;
        });
        return !exist;
      })) {
    // used a field not in group by
    rc = RC::NOTFOUND;
    return rc;
  }

  std::vector<Field> order_by_fields;
  Table             *tmp_table;
  const FieldMeta   *tmp_field;
  bool               tmp_sort_way;
  for (const RelAttrSqlNode &node : select_sql.order_lists) {

    for (const auto &expr : query_exprs) {

      if (let field = dynamic_cast<FieldExpr *>(expr.get())) {
        if (node.relation_name.empty()) {
          if (!expr->name().empty() && expr->name() == node.attribute_name ||
              node.attribute_name == field->field_name()) {
            order_by_fields.push_back(field->field());
            order_by_fields.back().sort_way() = node.sort_way == "ASC";
          }
        } else {
          if ((field->table_name() == node.relation_name || field->raw_table_name() == node.relation_name) &&
              field->field_name() == node.attribute_name) {
            order_by_fields.push_back(field->field());
            order_by_fields.back().sort_way() = node.sort_way == "ASC";
          }
        }
      }
    }
    //    if (i.relation_name != "") {
    //      tmp_table = (table_map.find(i.relation_name))->second;
    //    } else {
    //      tmp_table = default_table;
    //    }
    //    if (i.attribute_name != "") {
    //      tmp_field = tmp_table->table_meta().field(i.attribute_name.c_str());
    //    }
    //    if (i.sort_way == "ASC") {
    //      tmp_sort_way = true;
    //    } else {
    //      tmp_sort_way = false;
    //    }
    //    order_by_fields.emplace_back(tmp_table, tmp_field, tmp_sort_way);
  }

  // everything alright
  SelectStmt *select_stmt = new SelectStmt();
  // TODO add expression copy
  select_stmt->table_map_ = std::move(table_map);
  select_stmt->tables_.swap(tables);
  select_stmt->query_exprs_.swap(query_exprs);
  select_stmt->filter_stmt_ = filter_stmt;
  select_stmt->having_stmt_ = having_stmt;
  select_stmt->aggregates_.swap(aggregates);
  select_stmt->group_bys_.swap(group_bys);
  select_stmt->order_by_elems_.swap(order_by_fields);
  for (const auto &item : select_sql.select_exprs) {
    select_stmt->select_exprs_.push_back(item.get());
  }
  select_stmt->join_stmts_.swap(join_stmts);
  stmt = select_stmt;
  return RC::SUCCESS;
}
