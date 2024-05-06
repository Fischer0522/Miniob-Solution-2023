/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/5/22.
//

#include "sql/stmt/update_stmt.h"
#include "common/log/log.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "sql/stmt/filter_stmt.h"
#include "event/sql_event.h"

UpdateStmt::UpdateStmt(Table *table, std::unordered_map<std::string, Value> update_list, FilterStmt *filter_stmt)
    : table_(table), field_and_value_(std::move(update_list)), filter_stmt_(filter_stmt)
{}
// 此处将Node转换成更结构化的Stmt，具体涉及一些数据结构的赋值和错误处理
RC UpdateStmt::create(Db *db, const UpdateSqlNode &update, Stmt *&stmt)
{
  // First get the table name
  const char *raw_table_name = update.relation_name.c_str();
  const char *table_name     = try_convert_view_to_table(raw_table_name);
  // 先不过滤，主打一个最小模型
  if (nullptr == db || nullptr == table_name) {
    LOG_WARN("invalid argument. db=%s, table_name=%s", db, table_name);
    return_rc(RC::INVALID_ARGUMENT);
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return_rc(RC::SCHEMA_TABLE_NOT_EXIST);
  }
  // TODO 在此处需要处理为null的值
  std::unordered_map<std::string, Value> update_list;

  Value update_val;
  RC    rc      = RC::SUCCESS;
  bool  fragile = false;
  for (const auto &[attr_name, update_expr] : update.update_list) {
    rc = update_expr->try_get_value(update_val);
    if (rc != RC::SUCCESS) {
      fragile = true;
      continue;
    }
    if (update_val.attr_type() == TUPLE_SET) {
      fragile = true;
      continue;
    }

    // 转field字段
    // 构造Field还要构造Fieldmeta
    bool found = false;
    if (attr_name.empty()) {
      LOG_WARN("attr_name or value is empty");
      return_rc(RC::INVALID_ARGUMENT);
    }
    if (update_val.data() == nullptr) {
      fragile = true;
      continue;
    }
    for (int i = 0; i < table->table_meta().field_num(); i++) {
      if (strcmp(table->table_meta().field(i)->name(), attr_name.c_str()) == 0) {
        found = true;
      }
    }
    if (!found) {
      LOG_WARN("no such field:%s", attr_name.c_str());
      return_rc(RC::INVALID_ARGUMENT);
    }
    // 如果该field存在，再检验field的类型和Value的类型能不能对上
    const FieldMeta *field_meta = table->table_meta().field(attr_name.c_str());
    if (update_val.is_null()) {
      if (field_meta->is_allow_null()) {
        update_val.set_null(field_meta->type());
      } else {
        fragile = true;
        continue;
        //        LOG_WARN("field:%s is not allowed null", field_meta->name());
        //        return_rc(RC::INVALID_ARGUMENT);
      }
    }
    if (field_meta->type() != update_val.attr_type()) {
      //      LOG_WARN("invalid type,field:%d,value:%d", field_meta, update_val.attr_type());
      //      return_rc(RC::INVALID_ARGUMENT);
      rc = update_val.cast_to(field_meta->type());
      if (rc != RC::SUCCESS) {
        fragile = true;
        continue;
      }
    }
    update_list.insert({attr_name, update_val});
  }

  // const int field_num = table_meta.field_num() - table_meta.sys_field_num();
  //  if (field_num != value_num) {
  //    LOG_WARN("schema mismatch. value num=%d, field num in schema=%d", value_num, field_num);
  //    return RC::SCHEMA_FIELD_MISSING;
  //  }

  // check fields type
  // const int sys_field_num = table_meta.sys_field_num();
  // for (int i = 0; i < value_num; i++) {
  //   const FieldMeta *field_meta = table_meta.field(i + sys_field_num);
  //   const AttrType field_type = field_meta->type();
  //   const AttrType value_type = values[i].attr_type();
  //   if (field_type != value_type) {  // TODO try to convert the value type to field type
  //     LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
  //         table_name, field_meta->name(), field_type, value_type);
  //     return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  //   }
  // }

  // 最后处理statement
  TableMap table_map;
  table_map.insert(std::make_pair(std::string(table_name), table_entry(table)));
  FilterTree *root     = nullptr;
  let         sql_node = container_of(&update, ParsedSqlNode, update);
  rc                   = FilterStmt::create(db, table, &table_map, update.conditions.get(), sql_node, root);
  let filter_stmt      = new FilterStmt(std::unique_ptr<FilterTree>(root));

  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create filter statement. rc=%d:%s", rc, strrc(rc));
    return_rc(rc);
  }
  // everything alright
  let_mut update_stmt  = new UpdateStmt(table, std::move(update_list), filter_stmt);
  update_stmt->fragile = fragile;
  stmt                 = update_stmt;

  return RC::SUCCESS;
}
