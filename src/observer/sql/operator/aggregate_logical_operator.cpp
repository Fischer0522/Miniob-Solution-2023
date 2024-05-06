//
// Created by code4love on 23-10-9.
//
// pr test
#include "aggregate_logical_operator.h"
AggregateLogicalOperator::AggregateLogicalOperator(std::vector<AggregateExpr> aggrs, std::vector<Field> group_bys)
    : aggrs_(std::move(aggrs)), group_bys_(std::move(group_bys))
{}
