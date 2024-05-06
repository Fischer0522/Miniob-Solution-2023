#pragma once

#include <vector>
#include <memory>

#include "sql/operator/logical_operator.h"
#include "storage/field/field.h"


class SortLogicalOperator : public LogicalOperator 
{
public:
  SortLogicalOperator(const std::vector<Field> &fields);
  virtual ~SortLogicalOperator() = default;

  LogicalOperatorType type() const override
  {
    return LogicalOperatorType::SORT;
  }

  const std::vector<Field> &fields() const
  {
    return fields_;
  }

private:
  std::vector<Field> fields_;
};