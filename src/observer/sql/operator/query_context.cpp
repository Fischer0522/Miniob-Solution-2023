//
// Created by code4love on 23-10-15.
//

#include "query_context.h"
#include "storage/table/table.h"
const char *QueryContext::find_table_name(PhysicalOperator *oper, const std::string& name)
{
  if (let found = oper_table_map_map_.find(oper); found != oper_table_map_map_.end()) {
    if (let iter = found->second.find(name); iter != found->second.end()) {
      return iter->second.table->name();
    }
  }
  return nullptr;
}
