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
// Created by Wangyunlai on 2023/6/13.
//

#include "sql/executor/create_table_executor.h"

#include "session/session.h"
#include "common/log/log.h"
#include "storage/table/table.h"
#include "sql/stmt/create_table_stmt.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "storage/db/db.h"
#include "storage/trx/trx.h"

RC CreateTableExecutor::execute(SQLStageEvent *sql_event)
{
  Stmt    *stmt    = sql_event->stmt();
  Session *session = sql_event->session_event()->session();
  ASSERT(stmt->type() == StmtType::CREATE_TABLE,
      "create table executor can not run this command: %d",
      static_cast<int>(stmt->type()));

  CreateTableStmt *create_table_stmt = static_cast<CreateTableStmt *>(stmt);

  if (create_table_stmt->init_execute && create_table_stmt->node.type == VIEW_T) {
    sql_event->release_query_context();
    create_table_stmt->init_execute = false;
    push_create_view(create_table_stmt->node.relation_name, create_table_stmt);
    // steal it!
    sql_event->set_stmt(nullptr);
    return RC::SUCCESS;
  }

  create_table_stmt->materialize();

  const int attribute_count = static_cast<int>(create_table_stmt->attr_infos().size());

  const char *table_name = create_table_stmt->table_name().c_str();
  Table      *table      = nullptr;
  RC          rc         = session->get_current_db()->create_table(
      table_name, attribute_count, create_table_stmt->attr_infos().data(), table);

  if (rc != RC::SUCCESS) {
    return rc;
  }

  //  if (create_table_stmt.)
  Record record;
  if (table != nullptr) {
    let trx = session->current_trx();
    for (const auto &tuple : create_table_stmt->values_to_insert) {
      let_ref values = tuple->cells();
      rc     = table->make_record(static_cast<int>(values.size()), values.data(), record);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to make record. rc=%s", strrc(rc));
        return rc;
      }

      rc = trx->insert_record(table, record);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to insert record by transaction. rc=%s", strrc(rc));
        return rc;
      }
    }
  }

  return rc;
}