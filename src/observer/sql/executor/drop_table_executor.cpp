//
// Created by 于芯邑 on 2023/10/12.
//

#include "drop_table_executor.h"
#include "sql/stmt/stmt.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "session/session.h"
#include "sql/stmt/drop_table_stmt.h"
#include "storage/db/db.h"
RC DropTableExecutor::execute(SQLStageEvent *sql_event) {
  RC rc = RC::SUCCESS;
  Stmt *stmt = sql_event->stmt();
  SessionEvent *session_event = sql_event->session_event();
  Session *session = session_event->session();
  ASSERT(stmt->type() == StmtType::DROP_TABLE,
      "drop table executor can not run this command: %d",static_cast<int>(stmt->type()));
  DropTableStmt * drop_table_stmt = static_cast<DropTableStmt *>(stmt);

  const char* table_name = drop_table_stmt->table_name().c_str();
  Db* db = session->get_current_db();
  rc = db->drop_table(table_name);
  return rc;
}