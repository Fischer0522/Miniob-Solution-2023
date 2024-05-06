//
// Created by code4love on 23-10-15.
//

#ifndef MINIDB_QUERY_CONTEXT_H
#define MINIDB_QUERY_CONTEXT_H

#include "storage/table/table.h"
struct QueryContext
{
  void push_operator(class PhysicalOperator *operator_) { operator_stack_.push_back(operator_); }
  void pop_operator() { operator_stack_.pop_back(); }

  const std::vector<class PhysicalOperator *> &operator_stack() const { return operator_stack_; }

  void insert(class PhysicalOperator *oper, TableMap table_map)
  {
    oper_table_map_map_[oper] = std::move(table_map);
  }

  void insert(class ParsedSqlNode *node, TableMap table_map) { sql_node_table_map_map_[node] = std::move(table_map); }

  TableMap &get_table_map(class PhysicalOperator *oper)
  {
    return oper_table_map_map_[oper];
  }

  TableMap &get_table_map(class ParsedSqlNode *node)
  {
    return sql_node_table_map_map_[node];
  }

  ParsedSqlNode *get_parent_sql_node(class ParsedSqlNode *sql_node)
  {

    if (let found = sql_node_father_map_.find(sql_node); found != sql_node_father_map_.end()) {
      return found->second;
    }
    return nullptr;
  }

  const char *find_table_name(class PhysicalOperator *oper, const std::string &name);

  std::unordered_map<class ParsedSqlNode *, class ParsedSqlNode *> sql_node_father_map_;
  std::vector<class PhysicalOperator *>                            operator_stack_;
  // TODO: finish select * from aggregation_func as a where id = (select min(id) from aggregation_func as b where a.id =
  // b.id)
  std::vector<class PhysicalOperator *>                                                        alias_stack_;
  std::unordered_map<class PhysicalOperator *, TableMap> oper_table_map_map_;
  std::unordered_map<class ParsedSqlNode *, TableMap>                                          sql_node_table_map_map_;
};

#endif  // MINIDB_QUERY_CONTEXT_H
