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
// Created by Wangyunlai on 2022/5/22.
//

#include "common/rc.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "sql/operator/physical_operator.h"
#include "sql/operator/project_physical_operator.h"

FilterStmt::~FilterStmt()
{
  for (FilterUnit *unit : filter_units_) {
    delete unit;
  }
  filter_units_.clear();
}

RC FilterStmt::create(Db *db, Table *default_table, TableMap *tables, const ConditionSqlNode *conditions,
    int condition_num, FilterStmt *&stmt)
{
  RC rc = RC::SUCCESS;
  stmt  = nullptr;

  FilterStmt *tmp_stmt = new FilterStmt();
  for (int i = 0; i < condition_num; i++) {
    FilterUnit *filter_unit = nullptr;
    rc                      = create_filter_unit(
        db, default_table, tables, const_cast<ConditionSqlNode &>(conditions[i]), nullptr, filter_unit);
    if (rc != RC::SUCCESS) {
      delete tmp_stmt;
      LOG_WARN("failed to create filter unit. condition index=%d", i);
      return rc;
    }
    tmp_stmt->filter_units_.push_back(filter_unit);
  }

  stmt = tmp_stmt;
  return rc;
}

RC FilterStmt::create_filter_unit(Db *db, Table *default_table, TableMap *tables, ConditionSqlNode &condition,
    ParsedSqlNode *sql_node, FilterUnit *&filter_unit)
{
  RC rc = RC::SUCCESS;

  CompOp comp = condition.comp;
  if (comp < EQUAL_TO || comp >= NO_OP) {
    LOG_WARN("invalid compare operator : %d", comp);
    return RC::INVALID_ARGUMENT;
  }

  filter_unit = new FilterUnit;

  // traverse expression tree to fill table in field expr
  auto fill_table = [&](Expression *expr) {
    if (let _ = dynamic_cast<SqlExpr *>(expr)) {
      return RC::SUCCESS;  // stop!
    }
    return traverse_expressions<FieldExpr>(expr, Order::PreOrder, [&](FieldExpr *field_expr) {
      RC rc = RC::SUCCESS;

      if (field_expr->table() != nullptr) {
        LOG_WARN("cannot find attr");
        return rc;
      }

      Table           *table      = nullptr;
      const FieldMeta *field_meta = nullptr;

      int table_index = -1;
      rc = get_table_and_field(db, default_table, tables, field_expr->node(), table, field_meta, table_index);
      if (rc != RC::SUCCESS) {
        if (sql_node) {

          while ((sql_node = sql_node->query_context->get_parent_sql_node(sql_node)) != nullptr) {
            let_ref p_tables = &sql_node->query_context->get_table_map(sql_node);
            rc               = get_table_and_field(db, default_table, p_tables, field_expr->node(), table, field_meta, table_index);
            if (rc == RC::SUCCESS) {
              break;
            }
          }
        }
        if (rc != RC::SUCCESS) {
          LOG_WARN("cannot find attr");
          return rc;
        }
      }
      if (table) {
        field_expr->field() = Field(table, field_meta);
        field_expr->table_index = table_index;
      }
      return RC::SUCCESS;
    });
  };

  {
    rc = fill_table(condition.left.get());
    if (rc != RC::SUCCESS) {
      return rc;
    }
    FilterObj filter_obj{std::move(condition.left)};
    //    filter_obj.init_attr(Field(table, field));
    filter_unit->set_left(std::move(filter_obj));
  }

  {
    fill_table(condition.right.get());
    if (rc != RC::SUCCESS) {
      return rc;
    }
    FilterObj filter_obj{std::move(condition.right)};
    //    filter_obj.init_attr(Field(table, field));
    filter_unit->set_right(std::move(filter_obj));
  }

  filter_unit->set_comp(comp);

  //  // 检查两个类型是否能够比较
  //  if (!filter_unit->is_comparable()) {
  //    rc = RC::NOT_COMPARABLE;
  //  }

  return rc;
}
RC FilterStmt::create(Db *db, Table *default_table, TableMap *tables, const ConditionTree *root,
    ParsedSqlNode *sql_node, FilterTree *&tree)
{
  RC rc = RC::SUCCESS;
  if (root == nullptr) {
    tree = nullptr;
    return rc;
  }

  tree = new FilterTree;
  if (root->node != nullptr) {
    FilterUnit *filter_unit = nullptr;
    rc                      = create_filter_unit(
        db, default_table, tables, const_cast<ConditionSqlNode &>(*root->node), sql_node, filter_unit);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    tree->filter_unit.reset(filter_unit);
  } else {
    // recursive create
    FilterTree *left = nullptr;
    rc               = create(db, default_table, tables, root->left.get(), sql_node, left);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    tree->left.reset(left);

    tree->type = root->op == ConditionTree::AND_OP ? ConjunctionExpr::Type::AND : ConjunctionExpr::Type::OR;

    FilterTree *right = nullptr;
    rc                = create(db, default_table, tables, root->right.get(), sql_node, right);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    tree->right.reset(right);
  }
  return rc;
}

bool FilterUnit::is_comparable()
{
  //  if (!is_range_comp(comp_)) {
  ////    if (let select_expr = dynamic_cast<SelectExpr *>(left_.expr.get())) {
  ////      if (!select_expr->is_aggregate_func()) {
  ////        return false;
  ////      }
  ////    }
  ////    if (let select_expr = dynamic_cast<SelectExpr *>(right_.expr.get())) {
  ////      if (!select_expr->is_aggregate_func()) {
  ////        return false;
  ////      }
  ////    }
  //  } else {
  if (let expressions_expr = dynamic_cast<ExpressionsExpr *>(right_.expr.get())) {
    if (let select_expr = expressions_expr->as_select_expr()) {
      // can only select single col
      if (select_expr->select_count() != 1) {
        return false;
      }
    }
  }
  //  }
  return true;
}
RC traverse_filter_tree(FilterTree *root, const std::function<RC(FilterUnit *)> &func)
{
  RC rc = RC::SUCCESS;
  if (root == nullptr) {
    return rc;
  }

  if (root->filter_unit != nullptr) {
    rc = func(root->filter_unit.get());
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }

  if (root->left != nullptr) {
    rc = traverse_filter_tree(root->left.get(), func);
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }

  if (root->right != nullptr) {
    rc = traverse_filter_tree(root->right.get(), func);
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }
  return rc;
}
