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

#pragma once

#include <vector>
#include <unordered_map>
#include "sql/parser/parse_defs.h"
#include "sql/stmt/stmt.h"
#include "sql/expr/expression.h"

class Db;
class Table;
class FieldMeta;

struct FilterObj
{
  //  bool is_attr;
  //  Field field;
  //  Value value;
  //
  //  void init_attr(const Field &field)
  //  {
  //    is_attr = true;
  //    this->field = field;
  //  }
  //
  //  void init_value(const Value &value)
  //  {
  //    is_attr = false;
  //    this->value = value;
  //  }
  std::unique_ptr<Expression> expr;
};

class FilterUnit
{
public:
  FilterUnit() = default;
  ~FilterUnit() {}

  void set_comp(CompOp comp) { comp_ = comp; }

  CompOp comp() const { return comp_; }

  void set_left(FilterObj obj) { left_ = std::move(obj); }
  void set_right(FilterObj obj) { right_ = std::move(obj); }

  FilterObj &left() { return left_; }
  FilterObj &right() { return right_; }

  bool is_comparable();

private:
  CompOp    comp_ = NO_OP;
  FilterObj left_;
  FilterObj right_;
};

struct FilterTree
{
  std::unique_ptr<FilterTree> left;
  ConjunctionExpr::Type       type;
  std::unique_ptr<FilterTree> right;

  std::unique_ptr<FilterUnit> filter_unit = nullptr;
};

RC traverse_filter_tree(FilterTree *root, const std::function<RC(FilterUnit *)> &func);

/**
 * @brief Filter/谓词/过滤语句
 * @ingroup Statement
 */
class FilterStmt
{
public:
  FilterStmt(std::unique_ptr<FilterTree> tree) : filter_tree_(std::move(tree)) {}
  FilterStmt() = default;
  virtual ~FilterStmt();

public:
  //  std::vector<FilterUnit *> &filter_units() { return filter_units_; }
  FilterTree *filter_tree() { return filter_tree_.get(); }

public:
  static RC create(Db *db, Table *default_table, TableMap *tables, const ConditionSqlNode *conditions,
      int condition_num, FilterStmt *&stmt);

  static RC create(Db *db, Table *default_table, TableMap *tables, const ConditionTree *root, ParsedSqlNode *sql_node,
      FilterTree *&tree);

  static RC create_filter_unit(Db *db, Table *default_table, TableMap *tables, ConditionSqlNode &condition,
      ParsedSqlNode *sql_node, FilterUnit *&filter_unit);

private:
  std::vector<FilterUnit *>   filter_units_;  // 默认当前都是AND关系
  std::unique_ptr<FilterTree> filter_tree_;
};
