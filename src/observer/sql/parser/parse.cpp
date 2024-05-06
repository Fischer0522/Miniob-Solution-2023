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
// Created by Meiyi
//

#include <mutex>
#include "sql/parser/parse.h"
#include "common/log/log.h"
#include "sql/expr/expression.h"
#include "sstream"

RC parse(char *st, ParsedSqlNode *sqln);

CalcSqlNode::~CalcSqlNode()
{
  for (Expression *expr : expressions) {
    delete expr;
  }
  expressions.clear();
}

ParsedSqlNode::ParsedSqlNode() : flag(SCF_ERROR)
{}

ParsedSqlNode::ParsedSqlNode(SqlCommandFlag _flag) : flag(_flag)
{}

void ParsedSqlResult::add_sql_node(std::unique_ptr<ParsedSqlNode> sql_node)
{
  sql_nodes_.emplace_back(std::move(sql_node));
}

////////////////////////////////////////////////////////////////////////////////

int sql_parse(const char *st, ParsedSqlResult *sql_result);

RC parse(const char *st, ParsedSqlResult *sql_result)
{
  int res = sql_parse(st, sql_result);
  if (res == -1) {
    return RC::INTERNAL;
  }
  return RC::SUCCESS;
}

bool check_date(int y,int m,int d) {
  static int month[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
  bool leap = (y%400==0 || y%100 && y%4==0);
  return y > 0
         && (m > 0) && (m <= 12)
         && (d > 0) && (d <= ((m == 2 && leap)?1:0) + month[m]);
}
Value* value_init_date(const char* v) {

  auto *value = new Value();
  value->set_type(DATES);
  Date date(v);
  value->set_date(date);
  return value;

}

