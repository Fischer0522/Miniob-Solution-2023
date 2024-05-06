//
// Created by code4love on 23-10-9.
//

#pragma once

#include <vector>
#include <memory>

#include "sql/operator/logical_operator.h"
#include "sql/expr/expression.h"
#include "storage/field/field.h"

/**
 * @brief aggregate
 * @ingroup LogicalOperator
 * @details 从表中获取数据后，可能需要过滤，投影，连接等等。
 */
class AggregateLogicalOperator : public LogicalOperator
{
public:
  explicit AggregateLogicalOperator(std::vector<AggregateExpr> aggrs, std::vector<Field> group_bys);
  virtual ~AggregateLogicalOperator() = default;

  LogicalOperatorType type() const override { return LogicalOperatorType::AGGREGATE; }

  //
  //  const std::vector<AggregateFunc> &aggr_funcs() const { return aggr_funcs_; }
  //
  //  const std::vector<std::string> &aggr_field_names() const { return aggr_field_names_; }
  const std::vector<AggregateExpr> &aggregates() const { return aggrs_; }

  std::vector<AggregateExpr> aggrs_;
  std::vector<Field>         group_bys_;
};
