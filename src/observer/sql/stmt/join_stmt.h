//
// Created by fischer on 23-10-20.
//

#ifndef MINIDB_JOIN_STMT_H
#define MINIDB_JOIN_STMT_H

#include "filter_stmt.h"
class JoinStmt
{
public:
  JoinStmt(std::unique_ptr<FilterTree> tree) : filter_tree_(std::move(tree)) {}
  JoinStmt() = default;
  static RC create_join_filter_unit(Db *db, Table *left_table, Table *right_table,
      std::unordered_map<std::string, table_entry> *tables, ConditionSqlNode &condition, FilterUnit *&filter_unit);
  //  static RC create(Db *db,Table *left_table,Table *right_table,std::unordered_map<std::string,Table*> *tables,
  //       ConditionSqlNode &condition,FilterTree *&tree);
  static RC create(Db *db, Table *left_table, Table *right_table, std::unordered_map<std::string, table_entry> *tables,
      const ConditionTree *root, FilterTree *&tree);
  FilterTree *filter_tree() { return filter_tree_.get(); }

private:
  std::unique_ptr<FilterTree> filter_tree_;
};

#endif  // MINIDB_JOIN_STMT_H
