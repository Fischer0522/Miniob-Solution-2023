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
// Created by Wangyunlai on 2022/07/05.
//

#include <complex>
#include "pch.h"
#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/executor/sql_result.h"
#include "sql/operator/project_physical_operator.h"
#include <common/lang/bitmap.h>
#include <regex>

using namespace std;

RC FieldExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC      rc   = RC::SUCCESS;
  let_mut spec = TupleCellSpec(table_name(), field_name());
  spec.table_index = table_index;
  rc = tuple.find_cell(spec, value);
  if (rc != RC::SUCCESS) {
    assert(query_context_ != nullptr);
    int i = (int)query_context_->operator_stack().size() - 1;
    for (; i >= 0; i--) {
      let_ref item = query_context_->operator_stack()[i];

      let_mut tbl_name = table_name();
      if (let name = query_context_->find_table_name(item, tbl_name); name != nullptr) {
        tbl_name = name;
      }

      auto *current_tuple = item->current_tuple();
      rc                  = current_tuple->find_cell(TupleCellSpec(tbl_name, field_name()), value);
      if (rc == RC::SUCCESS) {
        break;
      }
    }
  }
  if (rc != RC::SUCCESS) {
    return rc;
  }

  switch (func_) {
    case F_NONE: break;
    case F_LENGTH: return value.apply_length_func();
    case F_ROUND: return value.apply_round_func(decimals);
    case F_DATE_FORMAT: return value.apply_date_format_func(date_format);
  }
  return rc;
}

AttrType FieldExpr::value_type() const
{
  AttrType type = field_.attr_type();
  switch (func_) {
    case F_NONE: break;
    case F_LENGTH:
    case F_ROUND: type = INTS; break;
    case F_DATE_FORMAT: type = CHARS; break;
  }
  return type;
}

RC ValueExpr::get_value(const Tuple &tuple, Value &value) const
{
  value = value_;
  return RC::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
CastExpr::CastExpr(unique_ptr<Expression> child, AttrType cast_type) : child_(std::move(child)), cast_type_(cast_type)
{}

CastExpr::~CastExpr() {}

RC CastExpr::cast(const Value &value, Value &cast_value) const
{
  RC rc = RC::SUCCESS;
  if (this->value_type() == value.attr_type()) {
    cast_value = value;
    return rc;
  }

  switch (cast_type_) {
    case BOOLEANS: {
      bool val = value.get_boolean();
      cast_value.set_boolean(val);
    } break;
    default: {
      rc = RC::INTERNAL;
      LOG_WARN("unsupported convert from type %d to %d", child_->value_type(), cast_type_);
    }
  }
  return rc;
}

RC CastExpr::get_value(const Tuple &tuple, Value &cell) const
{
  RC rc = child_->get_value(tuple, cell);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return cast(cell, cell);
}

RC CastExpr::try_get_value(Value &value) const
{
  RC rc = child_->try_get_value(value);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return cast(value, value);
}

////////////////////////////////////////////////////////////////////////////////

ComparisonExpr::ComparisonExpr(CompOp comp, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : comp_(comp), left_(std::move(left)), right_(std::move(right))
{}

ComparisonExpr::~ComparisonExpr() {}

bool pattern_match(const std::string &str, const std::string &pat)
{
  // Convert the SQL LIKE pattern to a regular expression
  std::string regexPattern = "^";
  for (char c : pat) {
    switch (c) {
      case '%': regexPattern += ".*"; break;
      case '_': regexPattern += "."; break;
      case '.':
      case '\\':
      case '+':
      case '*':
      case '?':
      case '[':
      case '^':
      case ']':
      case '$':
      case '(':
      case ')':
      case '{':
      case '}':
      case '=':
      case '!':
      case '<':
      case '>':
      case '|':
      case ':':
      case '-': regexPattern += "\\" + std::string(1, c); break;
      default: regexPattern += c; break;
    }
  }
  regexPattern += "$";

  std::regex re(regexPattern, std::regex::icase);  // icase for case-insensitive matching

  // Check if the string matches the pattern
  return std::regex_match(str, re);
}

RC ComparisonExpr::compare_value(const Value &left, const Value &right, bool &result) const
{
  // handle is null and is not null first
  if (comp_ == IS_OP) {
    if (left.is_null() && right.is_null()) {
      result = true;
    } else {
      result = false;
    }
    return RC::SUCCESS;
  }
  if (comp_ == IS_NOT_OP) {
    if (left.is_null() && right.is_null()) {
      result = false;
    } else {
      result = true;
    }
    return RC::SUCCESS;
  }
  // false for null
  if (left.is_null()) {
    result = false;
    return RC::SUCCESS;
  }

  if (comp_ == LIKE_OP || comp_ == NOT_LIKE_OP) {
    if (right.attr_type() != CHARS || left.attr_type() != CHARS) {
      return RC::INVALID_ARGUMENT;
    }

    std::string str = left.get_string();
    std::string pat = right.get_string();
    result          = pattern_match(str, pat);
    if (comp_ == NOT_LIKE_OP) {
      result = !result;
    }
    return RC::SUCCESS;
  }

  if (comp_ == EXISTS_OP) {
    result = !left.is_null();
    return RC::SUCCESS;
  }

  if (comp_ == NOT_EXISTS_OP) {
    result = left.is_null();
    return RC::SUCCESS;
  }

  if (right.is_null()) {
    if (comp_ == NOT_IN_OP) {
      result = true;
    } else {
      result = false;
    }

    return RC::SUCCESS;
  }

  if (comp_ == IN_OP || comp_ == NOT_IN_OP) {
    result = right.contains(left);
    if (comp_ == NOT_IN_OP) {
      result = !result;
    }
    return RC::SUCCESS;
  }

  // can't compare between tuple set!
  if (left.attr_type() == TUPLE_SET || right.attr_type() == TUPLE_SET) {
    return RC::COMPARE_ERROR;
  }

  RC  rc         = RC::SUCCESS;
  int cmp_result = left.compare(right);
  result         = false;
  switch (comp_) {
    case EQUAL_TO: {
      result = (0 == cmp_result);
    } break;
    case LESS_EQUAL: {
      result = (cmp_result <= 0);
    } break;
    case NOT_EQUAL: {
      result = (cmp_result != 0);
    } break;
    case LESS_THAN: {
      result = (cmp_result < 0);
    } break;
    case GREAT_EQUAL: {
      result = (cmp_result >= 0);
    } break;
    case GREAT_THAN: {
      result = (cmp_result > 0);
    } break;
    default: {
      LOG_WARN("unsupported comparison. %d", comp_);
      rc = RC::INTERNAL;
    } break;
  }

  return rc;
}

RC ComparisonExpr::try_get_value(Value &cell) const
{
  if (left_->type() == ExprType::VALUE && right_->type() == ExprType::VALUE) {
    ValueExpr   *left_value_expr  = static_cast<ValueExpr *>(left_.get());
    ValueExpr   *right_value_expr = static_cast<ValueExpr *>(right_.get());
    const Value &left_cell        = left_value_expr->get_value();
    const Value &right_cell       = right_value_expr->get_value();

    bool value = false;
    RC   rc    = compare_value(left_cell, right_cell, value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to compare tuple cells. rc=%s", strrc(rc));
    } else {
      cell.set_boolean(value);
    }
    return rc;
  }

  return RC::INVALID_ARGUMENT;
}

RC ComparisonExpr::get_value(const Tuple &tuple, Value &value) const
{
  Value left_value;
  Value right_value;

  RC rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }

  // right can be null
  if (right_) {
    rc = right_->get_value(tuple, right_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
      return rc;
    }
  }

  bool bool_value = false;
  rc              = compare_value(left_value, right_value, bool_value);
  if (rc == RC::SUCCESS) {
    value.set_boolean(bool_value);
  }
  return rc;
}

////////////////////////////////////////////////////////////////////////////////
RC ConjunctionExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;
  //  if (children_.empty()) {
  //    value.set_boolean(true);
  //    return rc;
  //  }

  //  for (const unique_ptr<Expression> &expr : children_) {
  //    rc = expr->get_value(tuple, tmp_value);
  //    if (rc != RC::SUCCESS) {
  //      LOG_WARN("failed to get value by child expression. rc=%s", strrc(rc));
  //      return rc;
  //    }
  //    bool bool_value = tmp_value.get_boolean();
  //    if ((conjunction_type_ == Type::AND && !bool_value) || (conjunction_type_ == Type::OR && bool_value)) {
  //      value.set_boolean(bool_value);
  //      return rc;
  //    }
  //  }
  if (expr_) {
    rc = expr_->get_value(tuple, value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value from expression. rc=%s", strrc(rc));
      return rc;
    }
  } else {
    rc = left_->get_value(tuple, value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value from left expression. rc=%s", strrc(rc));
      return rc;
    }
    let left_val = value.get_boolean();
    rc           = right_->get_value(tuple, value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value from right expression. rc=%s", strrc(rc));
      return rc;
    }
    let right_val = value.get_boolean();
    if (conjunction_type_ == Type::AND) {
      value.set_boolean(left_val && right_val);
    } else {
      value.set_boolean(left_val || right_val);
    }
  }
  return rc;
}

////////////////////////////////////////////////////////////////////////////////

ArithmeticExpr::ArithmeticExpr(ArithmeticExpr::Type type, Expression *left, Expression *right)
    : arithmetic_type_(type), left_(left), right_(right)
{}
ArithmeticExpr::ArithmeticExpr(ArithmeticExpr::Type type, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : arithmetic_type_(type), left_(std::move(left)), right_(std::move(right))
{}

AttrType ArithmeticExpr::value_type() const
{
  if (!right_) {
    return left_->value_type();
  }

  if (left_->value_type() == AttrType::INTS && right_->value_type() == AttrType::INTS &&
      arithmetic_type_ != Type::DIV) {
    return AttrType::INTS;
  }

  return AttrType::FLOATS;
}

RC ArithmeticExpr::calc_value(const Value &left_value, const Value &right_value, Value &value) const
{
  RC rc = RC::SUCCESS;

  if (left_value.is_null() || right_value.is_null()) {
    value.set_null();
    return rc;
  }

  const AttrType target_type = value_type();

  switch (arithmetic_type_) {
    case Type::ADD: {
      if (target_type == AttrType::INTS) {
        value.set_int(left_value.get_int() + right_value.get_int());
      } else {
        value.set_float(left_value.get_float() + right_value.get_float());
      }
    } break;

    case Type::SUB: {
      if (target_type == AttrType::INTS) {
        value.set_int(left_value.get_int() - right_value.get_int());
      } else {
        value.set_float(left_value.get_float() - right_value.get_float());
      }
    } break;

    case Type::MUL: {
      if (target_type == AttrType::INTS) {
        value.set_int(left_value.get_int() * right_value.get_int());
      } else {
        value.set_float(left_value.get_float() * right_value.get_float());
      }
    } break;

    case Type::DIV: {
      if (target_type == AttrType::INTS) {
        if (right_value.get_int() == 0) {
          // NOTE:
          // 设置为整数最大值是不正确的。通常的做法是设置为NULL，但是当前的miniob没有NULL概念，所以这里设置为整数最大值。
          value.set_int(numeric_limits<int>::max());
          //          value.set_null();
        } else {
          value.set_int(left_value.get_int() / right_value.get_int());
        }
      } else {
        if (right_value.get_float() > -EPSILON && right_value.get_float() < EPSILON) {
          // NOTE:
          // 设置为浮点数最大值是不正确的。通常的做法是设置为NULL，但是当前的miniob没有NULL概念，所以这里设置为浮点数最大值。
          //          value.set_null();
          value.set_float(numeric_limits<float>::max());
        } else {
          value.set_float(left_value.get_float() / right_value.get_float());
        }
      }
    } break;

    case Type::NEGATIVE: {
      if (target_type == AttrType::INTS) {
        value.set_int(-left_value.get_int());
      } else {
        value.set_float(-left_value.get_float());
      }
    } break;

    default: {
      rc = RC::INTERNAL;
      LOG_WARN("unsupported arithmetic type. %d", arithmetic_type_);
    } break;
  }
  return rc;
}

RC ArithmeticExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;

  Value left_value;
  Value right_value;

  rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }

  if (right_) {
    rc = right_->get_value(tuple, right_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
      return rc;
    }
  }

  return calc_value(left_value, right_value, value);
}

RC ArithmeticExpr::try_get_value(Value &value) const
{
  RC rc = RC::SUCCESS;

  Value left_value;
  Value right_value;

  rc = left_->try_get_value(left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }

  if (right_) {
    rc = right_->try_get_value(right_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
      return rc;
    }
  }

  return calc_value(left_value, right_value, value);
}
RC AggregateExpr::get_value(const Tuple &tuple, Value &value) const
{
  return tuple.find_cell(TupleCellSpec(name().c_str()), value);
}
AttrType AggregateExpr::value_type() const
{
  AttrType type = AttrType::UNDEFINED;
  switch (aggr_.func) {
    case AggregateFunc::None: break;
    case AggregateFunc::Count:
    case AggregateFunc::CountStar: type = AttrType::INTS; break;
    case AggregateFunc::Min:
    case AggregateFunc::Max:
    case AggregateFunc::Sum:
    case AggregateFunc::Avg: type = field().attr_type(); break;
  }
  return type;
}
RC SqlExpr::materialize() const
{
  RC rc = RC::SUCCESS;
  rc    = operator_->open(nullptr);
  DEFER(closer, operator_->close();)

  if (OB_FAIL(rc)) {
    return rc;
  }

  clear_result();

  while (RC::SUCCESS == (rc = operator_->next())) {
    std::vector<Value> tuple_values;

    let tuple = operator_->current_tuple();
    assert(tuple != nullptr);

    Value val;
    for (int i = 0; i < tuple->cell_num(); ++i) {
      tuple->cell_at(i, val);
      tuple_values.emplace_back(std::move(val));
    }

    result_.push_back(new ValueListTuple(std::move(tuple_values)));
  }

  if (rc == RC::RECORD_EOF) {
    return RC::SUCCESS;
  }

  return rc;
}
SqlExpr::~SqlExpr()
{
  clear_result();
  delete operator_;
}
RC SqlExpr::try_get_value(Value &value) const
{
  RC rc = RC::SUCCESS;
  rc    = materialize();
  if (rc != RC::SUCCESS) {
    LOG_INFO("failed to materialize. rc=%s", strrc(rc));
    return rc;
  }

  values_.resize(0);
  Value val;
  for (const auto &item : result_) {
    if (item->cell_num() > 1) {
      return_rc(RC::INVALID_ARGUMENT);
    }
    item->cell_at(0, val);
    values_.push_back(std::move(val));
  }
  value.set_tuple_set(const_cast<Value *>(values_.data()), values_.size());
  return RC::SUCCESS;
}
bool SqlExpr::is_aggregate_func()
{
  if (let oper = get_operator()) {
    let_ref children = oper->children();
    if (!children.empty() && children[0]->type() == PhysicalOperatorType::AGGREGATE) {
      return true;
    }
  }

  return false;
}
auto SqlExpr::select_count() -> std::size_t
{
  if (let oper = dynamic_cast<ProjectPhysicalOperator *>(get_operator())) {
    return oper->expressions().size();
  }
  return 0;
}
void SqlExpr::clear_result() const
{
  for (const auto &item : result_) {
    delete item;
  }
  result_.resize(0);
}
RC ExpressionsExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;
  // single select
  if (let select = as_select_expr()) {
    rc = select->get_value(tuple, value);
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }

  // multi numbers
  Value              val;
  std::vector<Value> cells;
  for (const auto &item : expressions_) {
    rc = item->get_value(tuple, val);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    cells.push_back(std::move(val));
  }

  delete result_;
  result_ = new ValueListTuple(std::move(cells));

  value.set_tuple_set(const_cast<Value *>(result_->cells().data()), result_->cells().size());

  return rc;
}
ExpressionsExpr::~ExpressionsExpr()
{
  for (const auto &item : expressions_) {
    delete item;
  }
  delete result_;
}
