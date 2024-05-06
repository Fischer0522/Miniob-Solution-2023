

#include "sql/operator/update_logical_operator.h"

UpdateLogicalOperator::UpdateLogicalOperator(Table *table, std::unordered_map<std::string,Value> field_and_value):table_(table),field_and_value_(std::move(field_and_value)){}
