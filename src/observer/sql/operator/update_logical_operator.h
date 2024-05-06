#pragma once

#include "sql/operator/logical_operator.h"

/**
 * @brief 逻辑算子，用于执行update语句
 * @ingroup LogicalOperator
 */
class UpdateLogicalOperator : public LogicalOperator 
{
public:
  UpdateLogicalOperator(Table *table,std::unordered_map<std::string,Value> field_and_value);
  virtual ~UpdateLogicalOperator() = default;

  LogicalOperatorType type() const override
  {
    return LogicalOperatorType::UPDATE;
  }
  Table *table() const
  {
    return table_;
  }
  std::unordered_map<std::string,Value> field_and_value() {
    return std::move(field_and_value_);
  }

  bool fragile = false;

private:
  Table *table_ = nullptr;
  std::unordered_map<std::string,Value> field_and_value_;
};
