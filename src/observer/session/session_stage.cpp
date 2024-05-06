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
// Created by Longda on 2021/4/13.
//

#include "session_stage.h"

#include "sql/stmt/select_stmt.h"
#include <string>

#include "common/conf/ini.h"
#include "common/log/log.h"
#include "common/lang/mutex.h"
#include "common/lang/string.h"
#include "common/seda/callback.h"
#include "event/session_event.h"
#include "event/sql_event.h"
#include "net/server.h"
#include "net/communicator.h"
#include "session/session.h"
#include "sql/operator/query_context.h"
#include "sql/operator/predicate_physical_operator.h"
#include "sql/operator/table_scan_physical_operator.h"

using namespace common;

// Constructor
SessionStage::SessionStage(const char *tag) : Stage(tag) {}

// Destructor
SessionStage::~SessionStage() {}

// Parse properties, instantiate a stage object
Stage *SessionStage::make_stage(const std::string &tag)
{
  SessionStage *stage = new (std::nothrow) SessionStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new ExecutorStage failed");
    return nullptr;
  }
  stage->set_properties();
  return stage;
}

// Set properties for this object set in stage specific properties
bool SessionStage::set_properties()
{
  //  std::string stageNameStr(stage_name_);
  //  std::map<std::string, std::string> section = g_properties()->get(
  //    stageNameStr);
  //
  //  std::map<std::string, std::string>::iterator it;
  //
  //  std::string key;

  return true;
}

// Initialize stage params and validate outputs
bool SessionStage::initialize() { return true; }

// Cleanup after disconnection
void SessionStage::cleanup() {}

void SessionStage::handle_event(StageEvent *event)
{
  // right now, we just support only one event.
  handle_request(event);

  event->done_immediate();
  return;
}

void SessionStage::handle_request(StageEvent *event)
{
  SessionEvent *sev = dynamic_cast<SessionEvent *>(event);
  if (nullptr == sev) {
    LOG_ERROR("Cannot cat event to sessionEvent");
    return;
  }

  std::string sql = sev->query();
  if (common::is_blank(sql.c_str())) {
    return;
  }

  Session::set_current_session(sev->session());
  sev->session()->set_current_request(sev);
  auto sql_event = std::make_unique<SQLStageEvent>(sev, sql);
  (void)handle_sql(sql_event.get());

  Communicator *communicator    = sev->get_communicator();
  bool          need_disconnect = false;
  RC            rc              = communicator->write_result(sev, need_disconnect);
  LOG_INFO("write result return %s", strrc(rc));
  if (need_disconnect) {
//    Server::close_connection(communicator);
  }
  sev->session()->set_current_request(nullptr);
  Session::set_current_session(nullptr);
}

RC SessionStage::handle_sql_expr(SqlExpr &root, SQLStageEvent *sql_event)
{
  new_call_stack;
  RC                                                   rc = RC::SUCCESS;
  std::vector<SqlExpr *>                               sql_exprs;
  std::unordered_map<Expression *, const TableMap *>   table_map;
  std::unordered_map<Expression *, Expression *>       father_map;
  std::unordered_map<ParsedSqlNode *, ParsedSqlNode *> sql_node_father_map;
  (void)traverse_expressions<SqlExpr>(nullptr, &root, Order::PreOrder, [&](Expression *father, SqlExpr *expr) {
    // update's value comes first
    for (let_ref[_, update_expr] : expr->node()->update.update_list) {
      expr->non_select = true;
      (void)traverse_expressions<SqlExpr>(
          nullptr, update_expr.get(), Order::PreOrder, [&](Expression *father, SqlExpr *expr) {
            sql_exprs.push_back(expr);
            father_map[expr] = father;
            if (let father_select = dynamic_cast<SqlExpr *>(father)) {
              sql_node_father_map[expr->node()] = father_select->node();
            }
            return RC::SUCCESS;
          });
    }
    // create's select comes first
    if (expr->node()->flag == SCF_CREATE_TABLE) {
      let_ref ct = expr->node()->create_table;
      if (ct.is_from_select) {
        expr->non_select = true;
        (void)traverse_expressions<SqlExpr>(
            nullptr, expr->node()->create_table.select.get(), Order::PreOrder, [&](Expression *father, SqlExpr *expr) {
              sql_exprs.push_back(expr);
              father_map[expr] = father;
              if (let father_select = dynamic_cast<SqlExpr *>(father)) {
                sql_node_father_map[expr->node()] = father_select->node();
              }
              return RC::SUCCESS;
            });
      }
    }
    sql_exprs.push_back(expr);

    if (!expr->non_select) {
      father_map[expr] = father;
      if (let father_select = dynamic_cast<SqlExpr *>(father)) {
        sql_node_father_map[expr->node()] = father_select->node();
      }
    }
    return RC::SUCCESS;
  });

  sql_event->get_query_context()->sql_node_father_map_ = std::move(sql_node_father_map);

  std::vector<PhysicalOperator *> physical_operators;

  let process_sql_expr = [&](SqlExpr *expr) {
    // set node in sql_event to execute it
    //      auto sql_node = std::make_unique<ParsedSqlNode>();
    //      sql_node->selection = expr->node()->selection;
    expr->node()->query_context = sql_event->get_query_context();
    expr->node()->sql_event     = sql_event;

    sql_event->set_sql_node(std::unique_ptr<ParsedSqlNode>(expr->node()));

    // sql node do not own the sql node, this is just workaround
    DEFER(release, (void)sql_event->sql_node().release();)

    // 在此处会创建对应的数据结构 Statement，
    // 根据SCF的类型，来创建对应的Statement，之后设置导sql_event当中，继续向后传递
    rc = resolve_stage_.handle_request(sql_event);
    if (OB_FAIL(rc)) {
      LOG_TRACE("failed to do resolve. rc=%s", strrc(rc));
      return rc;
    }
    if (let select_stmt = dynamic_cast<SelectStmt *>(sql_event->stmt())) {
      table_map[expr] = &select_stmt->table_map();
      sql_event->get_query_context()->insert(expr->node(), select_stmt->table_map());
    }
    // 在当中会生成physical_operator,如果生成了po则在write_result当中通过火山模型的方式进行执行
    // 而对于不存在physical_operator的，通常是一些单节点的执行计划，如create_table，此时就可以直接执行。
    rc = optimize_stage_.handle_request(sql_event);
    if (rc != RC::UNIMPLENMENT && rc != RC::SUCCESS) {
      LOG_TRACE("failed to do optimize. rc=%s", strrc(rc));
      return rc;
    }

    expr->set_operator(sql_event->physical_operator().release());
    physical_operators.push_back(expr->get_operator());

    return RC::SUCCESS;
  };

  {
    for (let_ref expr : sql_exprs) {
      if (expr->non_select) {
        continue;
      }
      rc = process_sql_expr(expr);
      if (rc != RC::SUCCESS) {
        return_rc(rc);
      }
    }
  }

  // check comparison validity
  for (let oper : physical_operators) {
    rc = traverse_physical_operators(oper, Order::PreOrder, [](PhysicalOperator *oper) {
      if (let predicate = dynamic_cast<PredicatePhysicalOperator *>(oper)) {
        return traverse_expressions<ComparisonExpr>(predicate->expression(), Order::PreOrder, [](ComparisonExpr *expr) {
          if (expr->comp() == IN_OP || expr->comp() == NOT_IN_OP) {
            let right = expr->right().get();
            if (let expressions_expr = dynamic_cast<ExpressionsExpr *>(right)) {
              if (let select_expr = expressions_expr->as_select_expr()) {
                // can only select single col
                if (select_expr->select_count() != 1) {
                  return RC::COMPARE_ERROR;
                }
              }
            }
          }
          return RC::SUCCESS;
        });
      }
      return RC::SUCCESS;
    });

    if (OB_FAIL(rc)) {
      sql_event->session_event()->sql_result()->set_return_code(rc);
      LOG_TRACE("failed to do traverse. rc=%s", strrc(rc));
      return rc;
    }
  }

  std::unordered_map<Expression *, PhysicalOperator *>       select2table_scan;
  std::unordered_map<PhysicalOperator *, PhysicalOperator *> table_scan_father;
  // back patch field with aliased table name
  for (let select : sql_exprs) {
    let oper = select->get_operator();
    rc       = traverse_physical_operators(oper, Order::PreOrder, [&](PhysicalOperator *oper) {
      if (let table_scan = dynamic_cast<TableScanPhysicalOperator *>(oper)) {
        let_mut ctx = sql_event->get_query_context();
        if (let found = table_map.find(select); found != table_map.end()) {
          ctx->insert(table_scan, *found->second);
        }
      }
      if (let predicate = dynamic_cast<PredicatePhysicalOperator *>(oper)) {
        return traverse_expressions<FieldExpr>(predicate->expression(), Order::PreOrder, [&](FieldExpr *field_expr) {
          if (field_expr->table() == nullptr) {
            let         table_name = field_expr->table_name();
            Expression *expr       = select;
            while (expr != nullptr) {
              let found_map = table_map.find(expr);
              assert(found_map != table_map.end());
              let iter = found_map->second->find(table_name);
              if (iter != found_map->second->end()) {
                field_expr->set_table(iter->second.table);
              } else {
                expr = father_map[expr];
              }
            }
          }
          return RC::SUCCESS;
        });
      }
      return RC::SUCCESS;
    });
    if (OB_FAIL(rc)) {
      sql_event->session_event()->sql_result()->set_return_code(rc);
      LOG_TRACE("failed to do traverse. rc=%s", strrc(rc));
      return rc;
    }
  }

  if (sql_exprs.back()->non_select) {
    rc = process_sql_expr(sql_exprs.back());
    if (rc != RC::SUCCESS) {
      return_rc(rc);
    }
  }

  return rc;
}

/**
 * 处理一个SQL语句经历这几个阶段。
 * 虽然看起来流程比较多，但是对于大多数SQL来说，更多的可以关注parse和executor阶段。
 * 通常只有select、delete等带有查询条件的语句才需要进入optimize。
 * 对于DDL语句，比如create table、create index等，没有对应的查询计划，可以直接搜索
 * create_table_executor、create_index_executor来看具体的执行代码。
 * select、delete等DML语句，会产生一些执行计划，如果感觉繁琐，可以跳过optimize直接看
 * execute_stage中的执行，通过explain语句看需要哪些operator，然后找对应的operator来
 * 调试或者看代码执行过程即可。
 */
RC SessionStage::handle_sql(SQLStageEvent *sql_event)
{
  need_dump() = false;
  refresh_views(sql_event);

  RC rc = query_cache_stage_.handle_request(sql_event);
  if (OB_FAIL(rc)) {
    LOG_TRACE("failed to do query cache. rc=%s", strrc(rc));
    sql_event->session_event()->sql_result()->set_return_code(rc);
    return rc;
  }

  rc = parse_stage_.handle_request(sql_event);
  if (OB_FAIL(rc)) {
    sql_event->session_event()->sql_result()->set_return_code(rc);
    LOG_TRACE("failed to do parse. rc=%s", strrc(rc));
    return rc;
  }

  // steal the root from sql event
  std::unique_ptr<ParsedSqlNode> node = std::move(sql_event->sql_node());
  SqlExpr                        root(node.release());

  rc = handle_sql_expr(root, sql_event);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  sql_event->set_operator(std::unique_ptr<PhysicalOperator>(root.release_operator()));
  rc = execute_stage_.handle_request(sql_event);
  if (OB_FAIL(rc)) {
    sql_event->session_event()->sql_result()->set_return_code(rc);
    LOG_TRACE("failed to do traverse. rc=%s", strrc(rc));
    return rc;
  }
  return rc;
}
