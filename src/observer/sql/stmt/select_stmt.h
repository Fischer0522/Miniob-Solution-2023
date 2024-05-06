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
// Created by Wangyunlai on 2022/6/5.
//

#pragma once

#include <vector>
#include <memory>

#include "common/rc.h"
#include "sql/stmt/stmt.h"
#include "storage/field/field.h"
#include "sql/expr/expression.h"
#include "sql/expr/tuple_cell.h"
#include "join_stmt.h"

class FieldMeta;
class FilterStmt;
class Db;
class Table;
class FilterTree;

/**
 * @brief 表示select语句
 * @ingroup Statement
 */
class SelectStmt : public Stmt
{
public:
  using Aggregate = AggregateExpr;
  SelectStmt()    = default;
  ~SelectStmt() override;

  StmtType type() const override { return StmtType::SELECT; }

public:
  static RC create(Db *aggr_expr, SelectSqlNode &select_sql, Stmt *&stmt);

public:
  const std::vector<Table *>                     &tables() const { return tables_; }
  std::vector<std::unique_ptr<Expression>>        take_exprs() { return std::move(query_exprs_); }
  const std::vector<std::unique_ptr<Expression>> &query_exprs() { return query_exprs_; }
  const TableMap &table_map() const { return table_map_; }
  const std::vector<Aggregate>                   &aggregates() const { return aggregates_; }
  const std::vector<Expression *>                &select_exprs() const { return select_exprs_; }
  const std::vector<Field>                       &group_bys() const { return group_bys_; }
  FilterStmt                                     *filter_stmt() const { return filter_stmt_; }
  FilterStmt                                     *having_stmt() const { return having_stmt_; }
  const std::vector<JoinStmt *>                  &join_stmts() const { return join_stmts_; }
  const std::vector<Field>                       &order_by_elems() const { return order_by_elems_; }

private:
  std::vector<Expression *>                select_exprs_;
  std::vector<std::unique_ptr<Expression>> query_exprs_;
  std::vector<Table *>                     tables_;
  FilterStmt                              *filter_stmt_ = nullptr;
  FilterStmt                              *having_stmt_ = nullptr;
  std::vector<Aggregate>                   aggregates_;
  std::vector<Field>                       group_bys_;
  TableMap                                 table_map_;
  std::vector<JoinStmt *>                  join_stmts_;
  std::vector<Field>                       order_by_elems_;
};
