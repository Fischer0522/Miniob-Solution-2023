#pragma once


#include <vector>
#include "sql/operator/physical_operator.h"
#include "sql/parser/parse.h"


class UpdateStmt;

class UpdatePhysicalOperator : public PhysicalOperator
{
public:
  UpdatePhysicalOperator(Table *table,std::unordered_map<std::string,Value>  field_and_value):table_(table),field_and_value_(std::move(field_and_value))
  {}

  virtual ~UpdatePhysicalOperator() = default;

  PhysicalOperatorType type() const override
  {
    return PhysicalOperatorType::UPDATE;
  }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  Tuple *current_tuple() override { return nullptr; }

  bool fragile = false;

private:
  Table *table_ = nullptr;
  std::unordered_map<std::string,Value> field_and_value_;
  Trx *trx_ = nullptr;
};
