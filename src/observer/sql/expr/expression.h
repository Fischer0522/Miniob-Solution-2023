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

#pragma once

#include "pch.h"
#include <string.h>
#include <memory>
#include <string>
#include <list>

#include "storage/field/field.h"
#include "sql/parser/value.h"
#include "common/log/log.h"

class Tuple;
class ValueListTuple;

/**
 * @defgroup Expression
 * @brief 表达式
 */

/**
 * @brief 表达式类型
 * @ingroup Expression
 */
enum class ExprType
{
  NONE,
  STAR,         ///< 星号，表示所有字段
  FIELD,        ///< 字段。在实际执行时，根据行数据内容提取对应字段的值
  VALUE,        ///< 常量值
  CAST,         ///< 需要做类型转换的表达式
  COMPARISON,   ///< 需要做比较的表达式
  CONJUNCTION,  ///< 多个表达式使用同一种关系(AND或OR)来联结
  ARITHMETIC,   ///< 算术运算
  AGGREGATE,    ///<
  SQL,          ///< SQL语句
  EXPRESSIONS,  ///< expr1, expr2, expr3, ...
};

/**
 * @brief 表达式的抽象描述
 * @ingroup Expression
 * @details 在SQL的元素中，任何需要得出值的元素都可以使用表达式来描述
 * 比如获取某个字段的值、比较运算、类型转换
 * 当然还有一些当前没有实现的表达式，比如算术运算。
 *
 * 通常表达式的值，是在真实的算子运算过程中，拿到具体的tuple后
 * 才能计算出来真实的值。但是有些表达式可能就表示某一个固定的
 * 值，比如ValueExpr。
 */
class Expression
{
public:
  Expression()          = default;
  virtual ~Expression() = default;

  /**
   * @brief 根据具体的tuple，来计算当前表达式的值。tuple有可能是一个具体某个表的行数据
   */
  virtual RC get_value(const Tuple &tuple, Value &value) const = 0;

  /**
   * @brief 在没有实际运行的情况下，也就是无法获取tuple的情况下，尝试获取表达式的值
   * @details 有些表达式的值是固定的，比如ValueExpr，这种情况下可以直接获取值
   */
  virtual RC try_get_value(Value &value) const { return RC::UNIMPLENMENT; }

  /**
   * @brief 表达式的类型
   * 可以根据表达式类型来转换为具体的子类
   */
  virtual ExprType type() const = 0;

  /**
   * @brief 表达式值的类型
   * @details 一个表达式运算出结果后，只有一个值
   */
  virtual AttrType value_type() const = 0;

  /**
   * @brief 表达式的名字，比如是字段名称，或者用户在执行SQL语句时输入的内容
   */
  virtual std::string name() const { return name_; }
  virtual void        set_name(std::string name) { name_ = name; }

  virtual void set_query_context(class QueryContext *context) { query_context_ = context; }

protected:
  class QueryContext *query_context_{};

  std::string name_;
};

/**
 * @brief 字段表达式
 * @ingroup Expression
 */
class FieldExpr : public Expression
{
public:
  FieldExpr() = default;
  FieldExpr(const Table *table, const FieldMeta *field_meta) : field_(table, field_meta) {}
  FieldExpr(const Field &field) : field_(field) {}
  FieldExpr(const RelAttrSqlNode &node) : node_(node) {}

  void set_table(Table *table) { field_ = Field(table, table->table_meta().field(node_.attribute_name.c_str())); }

  virtual ~FieldExpr() = default;

  ExprType type() const override { return ExprType::FIELD; }
  AttrType value_type() const override;

  Field &field() { return field_; }

  const Field &field() const { return field_; }

  const Table *table() const { return field_.table(); }

  const FieldMeta *field_meta() const { return field_.meta(); }

  const char *table_name() const
  {
    if (field_.table() != nullptr) {
      return field_.table()->name();
    }
    if (!node_.relation_name.empty()) {
      return node_.relation_name.c_str();
    }
    return "";
  }

  const char *raw_table_name() const
  {
    if (!node_.relation_name.empty()) {
      return node_.relation_name.c_str();
    }
    return "";
  }

  std::string name() const override
  {
    if (name_.empty()) {
      return field_name();
    }
    return name_;
  }

  void set_func(FieldFunction func) { func_ = func; }
  auto get_func() { return func_; }

  const char *field_name() const
  {
    if (field_.table() == nullptr) {
      return node_.attribute_name.c_str();
    }
    return field_.field_name();
  }
  RC get_value(const Tuple &tuple, Value &value) const override;

  const RelAttrSqlNode &node() const { return node_; }

  int         decimals = 0;  // used for length func
  std::string date_format;   // used for date

  int table_index = -1;

private:
  Field          field_;
  RelAttrSqlNode node_;
  FieldFunction  func_ = F_NONE;
};

/**
 * @brief 常量值表达式
 * @ingroup Expression
 */
class ValueExpr : public Expression
{
public:
  ValueExpr() = default;
  explicit ValueExpr(const Value &value) : value_(value) {}

  virtual ~ValueExpr() = default;

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC try_get_value(Value &value) const override
  {
    value = value_;
    return RC::SUCCESS;
  }

  ExprType type() const override { return ExprType::VALUE; }

  AttrType value_type() const override { return value_.attr_type(); }

  void get_value(Value &value) const { value = value_; }

  const Value &get_value() const { return value_; }

private:
  Value value_;
};

/**
 * @brief 类型转换表达式
 * @ingroup Expression
 */
class CastExpr : public Expression
{
public:
  CastExpr(std::unique_ptr<Expression> child, AttrType cast_type);
  virtual ~CastExpr();

  ExprType type() const override { return ExprType::CAST; }
  RC       get_value(const Tuple &tuple, Value &value) const override;

  RC try_get_value(Value &value) const override;

  AttrType value_type() const override { return cast_type_; }

  std::unique_ptr<Expression> &child() { return child_; }

private:
  RC cast(const Value &value, Value &cast_value) const;

private:
  std::unique_ptr<Expression> child_;      ///< 从这个表达式转换
  AttrType                    cast_type_;  ///< 想要转换成这个类型
};

bool pattern_match(const std::string &str, const std::string &pat);

/**
 * @brief 比较表达式
 * @ingroup Expression
 */
class ComparisonExpr : public Expression
{
public:
  ComparisonExpr(CompOp comp, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
  virtual ~ComparisonExpr();

  ExprType type() const override { return ExprType::COMPARISON; }

  RC get_value(const Tuple &tuple, Value &value) const override;

  AttrType value_type() const override { return BOOLEANS; }

  CompOp comp() const { return comp_; }

  std::unique_ptr<Expression> &left() { return left_; }
  std::unique_ptr<Expression> &right() { return right_; }

  /**
   * 尝试在没有tuple的情况下获取当前表达式的值
   * 在优化的时候，可能会使用到
   */
  RC try_get_value(Value &value) const override;

  /**
   * compare the two tuple cells
   * @param value the result of comparison
   */
  RC compare_value(const Value &left, const Value &right, bool &value) const;

  void set_query_context(class QueryContext *context) override
  {
    Expression::set_query_context(context);
    if (left_) {
      left_->set_query_context(context);
    }
    if (right_) {
      right_->set_query_context(context);
    }
  }

private:
  CompOp                      comp_;
  std::unique_ptr<Expression> left_;
  std::unique_ptr<Expression> right_;
};

/**
 * @brief 联结表达式
 * @ingroup Expression
 * 多个表达式使用同一种关系(AND或OR)来联结
 * TODO: support OR
 */
class ConjunctionExpr : public Expression
{
public:
  enum class Type
  {
    AND,
    OR,
  };

public:
  //  ConjunctionExpr(Type type, std::vector<std::unique_ptr<Expression>> &children);
  ConjunctionExpr(Type type, std::unique_ptr<ConjunctionExpr> left, std::unique_ptr<ConjunctionExpr> right)
      : conjunction_type_(type), left_(std::move(left)), right_(std::move(right))
  {}
  ConjunctionExpr(std::unique_ptr<ComparisonExpr> expr) : expr_(std::move(expr)) {}
  virtual ~ConjunctionExpr() = default;

  ExprType type() const override { return ExprType::CONJUNCTION; }

  AttrType value_type() const override { return BOOLEANS; }

  RC get_value(const Tuple &tuple, Value &value) const override;

  auto *left() { return left_.get(); }
  auto *right() { return right_.get(); }
  auto *expr() { return expr_.get(); }

  Type conjunction_type() const { return conjunction_type_; }

  //  std::vector<std::unique_ptr<Expression>> &children() { return children_; }

  void set_query_context(class QueryContext *context) override
  {
    Expression::set_query_context(context);
    if (left_) {
      left_->set_query_context(context);
    }
    if (right_) {
      right_->set_query_context(context);
    }
    if (expr_) {
      expr_->set_query_context(context);
    }
  }

private:
  Type conjunction_type_;
  //  std::vector<std::unique_ptr<Expression>> children_;
  std::unique_ptr<ConjunctionExpr> left_;
  std::unique_ptr<ConjunctionExpr> right_;
  std::unique_ptr<ComparisonExpr>  expr_;
};

/**
 * @brief 算术表达式
 * @ingroup Expression
 */
class ArithmeticExpr : public Expression
{
public:
  enum class Type
  {
    ADD,
    SUB,
    MUL,
    DIV,
    NEGATIVE,
  };

public:
  ArithmeticExpr(Type type, Expression *left, Expression *right);
  ArithmeticExpr(Type type, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
  virtual ~ArithmeticExpr() = default;

  ExprType type() const override { return ExprType::ARITHMETIC; }

  AttrType value_type() const override;

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC try_get_value(Value &value) const override;

  Type arithmetic_type() const { return arithmetic_type_; }

  std::unique_ptr<Expression> &left() { return left_; }
  std::unique_ptr<Expression> &right() { return right_; }

  void set_query_context(class QueryContext *context) override
  {
    Expression::set_query_context(context);
    if (left_) {
      left_->set_query_context(context);
    }
    if (right_) {
      right_->set_query_context(context);
    }
  }

private:
  RC calc_value(const Value &left_value, const Value &right_value, Value &value) const;

private:
  Type                        arithmetic_type_;
  std::unique_ptr<Expression> left_;
  std::unique_ptr<Expression> right_;
};

class AggregateExpr : public Expression
{
public:
  AggregateExpr(AttrExprNode aggr, Table *table) : aggr_(std::move(aggr)), table_(table) {}
  AggregateExpr() = default;

  void set_table(Table *table) { table_ = table; }

  AggregateFunc aggr_func() const { return aggr_.func; }
  const Table  *table() const { return table_; }
  const char   *field_name() const { return aggr_.attr.attribute_name.c_str(); }
  const char   *table_name() const { return table_ != nullptr ? table_->name() : aggr_.attr.relation_name.c_str(); }

  Field field() const { return {table_, table_->table_meta().field(field_name())}; }

  ~AggregateExpr() override = default;
  RC get_value(const Tuple &tuple, Value &value) const override;

  RC       try_get_value(Value &value) const override { return Expression::try_get_value(value); }
  ExprType type() const override { return ExprType::AGGREGATE; }
  AttrType value_type() const override;

  const AttrExprNode &node() const { return aggr_; }

private:
  AttrExprNode aggr_;
  Table       *table_{};
};

class SqlExpr : public Expression
{
public:
  SqlExpr(ParsedSqlNode *node) : node_(node) {}
  ~SqlExpr();
  ExprType type() const override { return ExprType::SQL; }
  AttrType value_type() const override { return AttrType::UNDEFINED; }
  RC       get_value(const Tuple &tuple, Value &value) const override { return try_get_value(value); }
  RC       try_get_value(Value &value) const override;

  ParsedSqlNode *node() { return node_.get(); }

  RC materialize() const;

  void set_operator(class PhysicalOperator *oper) { operator_ = oper; }

  //  const class PhysicalOperator *get_operator() const { return operator_; }
  class PhysicalOperator *get_operator() { return operator_; }

  class PhysicalOperator *release_operator()
  {
    let temp  = operator_;
    operator_ = nullptr;
    return temp;
  }

  auto &result() { return result_; }

  size_t select_count();

  bool is_aggregate_func();
  bool non_select = false;

  void clear_result() const;

private:
  mutable std::vector<ValueListTuple *> result_;
  std::unique_ptr<ParsedSqlNode>        node_;
  class PhysicalOperator               *operator_{};
  mutable std::vector<Value>            values_;  // used for select;
};

class ExpressionsExpr : public Expression
{
public:
  ~ExpressionsExpr() override;
  ExpressionsExpr(std::vector<Expression *> expressions) : expressions_(std::move(expressions)) {}
  RC          get_value(const Tuple &tuple, Value &value) const override;
  RC          try_get_value(Value &value) const override { return Expression::try_get_value(value); }
  ExprType    type() const override { return ExprType::EXPRESSIONS; }
  AttrType    value_type() const override { return UNDEFINED; }
  std::string name() const override { return Expression::name(); }
  void        set_name(std::string name) override { Expression::set_name(name); }
  void        set_query_context(struct QueryContext *context) override
  {
    Expression::set_query_context(context);
    for (const auto &item : expressions_) {
      item->set_query_context(context);
    }
  }

  auto count() const { return expressions_.size(); }

  SqlExpr *as_select_expr() const
  {
    if (count() == 1) {
      return dynamic_cast<SqlExpr *>(expressions_[0]);
    }
    return nullptr;
  }

  const auto expressions() const { return expressions_; }

private:
  std::vector<Expression *> expressions_;
  mutable ValueListTuple   *result_ = nullptr;
};

enum class Order : std::uint8_t
{
  PreOrder,
  PostOrder,
};

template <typename TraverseExprType>
  requires std::derived_from<TraverseExprType, Expression>
RC traverse_expressions(Expression *root, Order order, const std::function<RC(TraverseExprType *)> &func);

template <typename TraverseExprType>
  requires std::derived_from<TraverseExprType, Expression>
RC traverse_expressions(
    Expression *father, Expression *root, Order order, const std::function<RC(Expression *, TraverseExprType *)> &func);

template <typename TraverseExprType>
  requires std::derived_from<TraverseExprType, Expression>
RC traverse_condition_tree(Expression *father, const ConditionTree *root, Order order,
    const std::function<RC(Expression *, TraverseExprType *)> &func)
{
  RC rc = RC::SUCCESS;
  if (root == nullptr) {
    return rc;
  }
  if (root->node != nullptr) {
    rc = traverse_expressions<TraverseExprType>(father, root->node->left.get(), order, func);
    if (OB_FAIL(rc)) {
      return rc;
    }
    rc = traverse_expressions<TraverseExprType>(father, root->node->right.get(), order, func);
    if (OB_FAIL(rc)) {
      return rc;
    }
  } else {
    rc = traverse_condition_tree<TraverseExprType>(father, root->left.get(), order, func);
    if (OB_FAIL(rc)) {
      return rc;
    }
    rc = traverse_condition_tree<TraverseExprType>(father, root->right.get(), order, func);
    if (OB_FAIL(rc)) {
      return rc;
    }
  }
  return rc;
}

template <typename TraverseExprType>
  requires std::derived_from<TraverseExprType, Expression>
RC traverse_expressions(
    Expression *father, Expression *root, Order order, const std::function<RC(Expression *, TraverseExprType *)> &func)
{
  RC rc = RC::SUCCESS;
  if (root == nullptr) {
    return rc;
  }
  Expression *new_father = father;

  if (order == Order::PreOrder) {
    if (auto *traverse_expr = dynamic_cast<TraverseExprType *>(root)) {
      new_father = root;
      rc         = func(father, traverse_expr);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
  }

  switch (root->type()) {
    case ExprType::NONE: break;
    case ExprType::STAR: break;
    case ExprType::FIELD: break;
    case ExprType::VALUE: break;
    case ExprType::CAST: break;
    case ExprType::COMPARISON: break;
    case ExprType::CONJUNCTION: {
      if (auto *conjunction = dynamic_cast<ConjunctionExpr *>(root)) {
        rc = traverse_expressions<TraverseExprType>(new_father, conjunction->left(), order, func);
        if (rc != RC::SUCCESS) {
          return rc;
        }
        rc = traverse_expressions<TraverseExprType>(new_father, conjunction->right(), order, func);
        if (rc != RC::SUCCESS) {
          return rc;
        }
        rc = traverse_expressions<TraverseExprType>(new_father, conjunction->expr(), order, func);
        if (rc != RC::SUCCESS) {
          return rc;
        }
      }
    } break;
    case ExprType::ARITHMETIC:
      if (auto *arithmetic_expr = dynamic_cast<ArithmeticExpr *>(root)) {
        rc = traverse_expressions<TraverseExprType>(new_father, arithmetic_expr->left().get(), order, func);
        if (rc != RC::SUCCESS) {
          return rc;
        }
        rc = traverse_expressions<TraverseExprType>(new_father, arithmetic_expr->right().get(), order, func);
        if (rc != RC::SUCCESS) {
          return rc;
        }
      }
      break;
    case ExprType::AGGREGATE: break;
    case ExprType::SQL: {
      let   select_expr = static_cast<SqlExpr *>(root);
      auto &node        = select_expr->node()->selection;
      traverse_condition_tree<TraverseExprType>(new_father, node.conditions.get(), order, func);
      for (let_ref expr : node.select_exprs) {
        traverse_expressions<TraverseExprType>(new_father, expr.get(), order, func);
      }
    } break;
    case ExprType::EXPRESSIONS: {
      let expressions_expr = static_cast<ExpressionsExpr *>(root);
      for (let_ref e : expressions_expr->expressions()) {
        traverse_expressions<TraverseExprType>(new_father, e, order, func);
      }
    } break;
  }

  if (order == Order::PostOrder) {
    if (auto *traverse_expr = dynamic_cast<TraverseExprType *>(root)) {
      rc = func(father, traverse_expr);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
  }

  return rc;
}

template <typename TraverseExprType>
  requires std::derived_from<TraverseExprType, Expression>
RC traverse_expressions(Expression *root, Order order, const std::function<RC(TraverseExprType *)> &func)
{
  return traverse_expressions<TraverseExprType>(
      nullptr, root, order, [&func](Expression *father, TraverseExprType *expr) { return func(expr); });
}

template <typename T>
inline bool exist_type_in_expr(Expression *expr)
{
  RC rc = RC::NOTFOUND;
  traverse_expressions<T>(expr, Order::PreOrder, [&](T *) { rc = RC::SUCCESS; });
  return rc == RC::SUCCESS;
}
