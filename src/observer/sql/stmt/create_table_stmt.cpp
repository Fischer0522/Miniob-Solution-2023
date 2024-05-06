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
// Created by Wangyunlai on 2023/6/13.
//

#include "sql/stmt/create_table_stmt.h"
#include "event/sql_debug.h"
#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/operator/physical_operator.h"
#include "sql/operator/project_physical_operator.h"

RC CreateTableStmt::create(Db *db, const CreateTableSqlNode &create_table, Stmt *&stmt)
{
  let create_table_stmt   = new CreateTableStmt(create_table.relation_name, {});
  create_table_stmt->node = std::move(const_cast<CreateTableSqlNode &>(create_table));
  //  create_table_stmt->values_to_insert = std::move(values_to_insert);
  stmt = create_table_stmt;

  sql_debug("create table statement: table name %s", create_table.relation_name.c_str());
  return RC::SUCCESS;
}
RC CreateTableStmt::materialize()
{
  RC rc = RC::SUCCESS;
  if (node.type == TABLE_T) {
    attr_infos_ = node.attr_infos;
  } else {
    attr_infos_.resize(0);
  }

  if (node.is_from_select) {
    // create table from select
    if (let select = dynamic_cast<SqlExpr *>(node.select.get())) {
      rc = select->materialize();
      if (rc != RC::SUCCESS) {
        return_rc(rc);
      }

      values_to_insert = select->result();

      rc = traverse_physical_operators(select->get_operator(), Order::PreOrder, [&](PhysicalOperator *oper) {
        if (let project = dynamic_cast<ProjectPhysicalOperator *>(oper)) {
          std::vector<AttrInfoSqlNode> infos;
          for (let_ref expr : *project->get_project_exprs()) {
            infos.emplace_back(expr->value_type(), expr->name(), 36, true);
          }
          if (attr_infos_.empty()) {
            attr_infos_.swap(infos);
          }
        }
        return RC::SUCCESS;
      });
      if (rc != RC::SUCCESS) {
        return_rc(rc);
      }
    } else {
      return_rc(RC::EMPTY);
    }
  }

  return rc;
}
