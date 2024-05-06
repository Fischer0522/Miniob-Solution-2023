//
// Created by code4love on 23-10-9.
//

#include "aggregate_physical_operator.h"
#include "aggregate_logical_operator.h"
RC AggregatePhysicalOperator::open(Trx *trx)
{
  RC rc = RC::SUCCESS;
  // empty
  if (children_.empty()) {
    return rc;
  }

  let_mut_ref child = children_[0];
  if (child == nullptr) {
    return rc;
  }

  child->open(trx);

  hashmap_ = {};
  // TODO: support aggregate init value

  bool has_no_tuple = true;

  while (RC::SUCCESS == (rc = child->next())) {
    has_no_tuple = false;
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      return RC::INTERNAL;
      LOG_WARN("failed to get tuple from operator");
      break;
    }

    const auto &key = MakeAggregateKey(tuple);
    if (hashmap_.count(key) == 0) {
      hashmap_[key] = make_init_value();
    }
    Combine(hashmap_[key], MakeAggregateValue(tuple));
  }

  if (has_no_tuple && group_bys_.empty()) {
    hashmap_[{}] = make_init_value();
  }

  if (rc == RC::RECORD_EOF) {
    return RC::SUCCESS;
  }

  // no group by
  return rc;
}
RC AggregatePhysicalOperator::next()
{
  RC rc = RC::SUCCESS;

  if (iter_ == nullptr) {
    iter_ = hashmap_.begin();
  } else {
    iter_++;
  }
  if (iter_ == hashmap_.end()) {
    return RC::RECORD_EOF;
  }

  return rc;
}
RC     AggregatePhysicalOperator::close() { return RC::RECORD_EOF; }
Tuple *AggregatePhysicalOperator::current_tuple()
{
  if (iter_ == hashmap_.end()) {
    return nullptr;
  }
  tuple_ = std::make_unique<AggregateTuple>();

  for (int i = 0; i < group_bys_.size(); i++) {
    auto &val  = iter_->first.group_bys_[i];
    auto  expr = std::make_unique<ValueExpr>(val);
    expr->set_name(group_bys_[i].meta()->name());

    tuple_->add_field(&group_bys_[i], std::move(expr));
  }

  auto &aggr_value = iter_->second;
  for (int i = 0; i < aggr_value.values.size(); i++) {
    auto &val = aggr_value.values[i];
    val.clear_denominator();
    auto expr = std::make_unique<ValueExpr>(val);
    expr->set_name(aggregates_[i].name());

    tuple_->add_aggregate(std::move(expr));
  }

  return tuple_.get();
}

void AggregatePhysicalOperator::Combine(AggregateValue &result, const AggregateValue &input)
{
  // TODO 处理为null的情况
  for (int i = 0; i < aggregates_.size(); i++) {
    auto &res_val = result.values[i];
    auto &in_val  = input.values[i];
    switch (aggregates_[i].aggr_func()) {
      case AggregateFunc::None: break;
      case AggregateFunc::CountStar: {
        assert(!res_val.is_null());
        res_val.set_int(res_val.get_int() + 1);
      } break;
      case AggregateFunc::Max: {
        if (in_val.is_null()) {
          break;
        }
        if (res_val.is_null()) {
          res_val = in_val;
        } else {
          if (in_val.compare(res_val) > 0) {
            res_val = in_val;
          }
        }
      } break;
      case AggregateFunc::Min: {
        if (in_val.is_null()) {
          break;
        }
        if (res_val.is_null()) {
          res_val = in_val;
        } else {
          if (in_val.compare(res_val) < 0) {
            res_val = in_val;
          }
        }
      } break;
      case AggregateFunc::Avg: {
        if (in_val.is_null()) {
          break;
        }
        if (res_val.is_null()) {
          res_val = in_val;
        } else {
          res_val += in_val;
        }
        res_val.increase_denominator();
      } break;
      case AggregateFunc::Sum: {
        if (in_val.is_null()) {
          break;
        }
        if (res_val.is_null()) {
          res_val = in_val;
        } else {
          res_val += in_val;
        }
      } break;
      case AggregateFunc::Count: {
        if (res_val.is_null()) {
          if (in_val.is_null()) {
            res_val = Value(0);
          } else {
            res_val = Value(1);
          }
        } else {
          if (in_val.is_null()) {
            break;
          }
          res_val.set_int(res_val.get_int() + 1);
        }
      } break;
    }
  }
}
AggregatePhysicalOperator::AggregatePhysicalOperator(
    std::unique_ptr<PhysicalOperator> child, const AggregateLogicalOperator &aggrs)
    : aggregates_(aggrs.aggrs_), group_bys_(aggrs.group_bys_)
{
  children_.push_back(std::move(child));
}
AggregateValue AggregatePhysicalOperator::make_init_value()
{
  std::vector<Value> values;
  for (const auto &aggr : aggregates_) {
    switch (aggr.aggr_func()) {
      case AggregateFunc::Count:
      case AggregateFunc::CountStar: values.emplace_back(0); break;
      default: values.push_back(Value::Null());
    }
  }
  return AggregateValue(values);
}
AggregateKey AggregatePhysicalOperator::MakeAggregateKey(Tuple *tuple)
{
  if (group_bys_.empty()) {
    return {};
  }

  AggregateKey key;
  Value        value;

  key.group_bys_.reserve(group_bys_.size());

  for (const auto &field : group_bys_) {
    tuple->find_cell(TupleCellSpec(field.table_name(), field.field_name()), value);
    key.group_bys_.push_back(std::move(value));
  }

  return key;
}
AggregateValue AggregatePhysicalOperator::MakeAggregateValue(Tuple *tuple)
{
  AggregateValue value;
  Value          val;

  value.values.reserve(aggregates_.size());

  for (const auto &aggr : aggregates_) {
    // TODO: in single table, table name is empty
    tuple->find_cell(TupleCellSpec(aggr.table_name(), aggr.field_name()), val);
    // TODO 处理为null的情况
    value.values.push_back(std::move(val));
  }

  return value;
}
