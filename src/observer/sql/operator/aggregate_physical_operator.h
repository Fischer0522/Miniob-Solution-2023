//
// Created by code4love on 23-10-9.
//

#pragma once

#include <memory>
#include "sql/operator/physical_operator.h"
#include "sql/expr/expression.h"
#include "aggregate_logical_operator.h"

class FilterStmt;

struct AggregateKey
{
  std::vector<Value> group_bys_;

  auto operator==(const AggregateKey &other) const -> bool
  {
    for (uint32_t i = 0; i < other.group_bys_.size(); i++) {
      if (group_bys_[i].compare(other.group_bys_[i]) != 0) {
        return false;
      }
    }
    return true;
  }
};

namespace std {

/** Implements std::hash on AggregateKey */
template <>
struct hash<AggregateKey>
{
  auto operator()(const AggregateKey &agg_key) const -> std::size_t
  {
    size_t curr_hash = 0;
    for (const auto &key : agg_key.group_bys_) {
      curr_hash ^= std::hash<Value>()(key);
      curr_hash *= 133;
    }
    return curr_hash;
  }
};
}  // namespace std

struct AggregateValue
{
  /** The aggregate values */
  std::vector<Value> values;
};

/**
 * @brief
 * @ingroup PhysicalOperator
 */
class AggregatePhysicalOperator : public PhysicalOperator
{
public:
  AggregatePhysicalOperator(std::unique_ptr<PhysicalOperator> child, const AggregateLogicalOperator &aggrs);
  virtual ~AggregatePhysicalOperator() = default;

  PhysicalOperatorType type() const override { return PhysicalOperatorType::AGGREGATE; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  Tuple *current_tuple() override;

private:
  void           Combine(AggregateValue &result, const AggregateValue &input);
  AggregateValue make_init_value();

  using HashMap = std::unordered_map<AggregateKey, AggregateValue>;
  HashMap                    hashmap_{};
  std::vector<AggregateExpr> aggregates_;
  std::vector<Field>         group_bys_;

  std::unique_ptr<AggregateTuple>                          tuple_;
  HashMap::iterator                        iter_ = {};
  AggregateKey                             MakeAggregateKey(Tuple *tuple);
  AggregateValue                           MakeAggregateValue(Tuple *tuple);
};
