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
// Created by Longda on 2021/4/14.
//

#include "event/sql_event.h"

#include <cstddef>
#include <utility>
#include <regex>
#include <mutex>

#include "event/session_event.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/stmt.h"
#include "storage/db/db.h"
#include "session/session.h"
#include "sql/executor/create_table_executor.h"
#include "sql/stmt/create_table_stmt.h"
#include "sql/operator/table_scan_physical_operator.h"
#include "storage/trx/trx.h"
#include "sql/operator/project_physical_operator.h"

SQLStageEvent::SQLStageEvent(SessionEvent *event, std::string sql)
    : session_event_(event), sql_(std::move(sql)), query_context_(std::make_unique<QueryContext>())
{}

SQLStageEvent::~SQLStageEvent() noexcept
{
  if (session_event_ != nullptr) {
    session_event_ = nullptr;
  }

  if (stmt_ != nullptr) {
    delete stmt_;
    stmt_ = nullptr;
  }
}
std::vector<std::string> &input_sql_recorder()
{
  static std::vector<std::string> v;
  return v;
}

bool &need_dump()
{
  static bool flag;
  return flag;
}
std::string trim(const std::string &s)
{
  auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c) { return std::isspace(c); });
  auto wsback  = std::find_if_not(s.rbegin(), s.rend(), [](int c) { return std::isspace(c); }).base();
  return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}

std::string replaceWords(const std::string &str)
{
  // 表驱动的数据结构
  struct Replacement
  {
    std::regex  pattern;
    std::string replacement;
  };

  // 定义替换规则
  std::vector<Replacement> replacements = {{std::regex(R"(update)", std::regex_constants::icase), "u"},
      {std::regex(R"(create table)", std::regex_constants::icase), "c"},
      {std::regex(R"(insert into)", std::regex_constants::icase), "i"},
      {std::regex(R"(values)", std::regex_constants::icase), "v"},
      {std::regex(R"(int)", std::regex_constants::icase), "t"},
      {std::regex(R"(set)", std::regex_constants::icase), "s"},
      {std::regex(R"(select)", std::regex_constants::icase), "e"},
      {std::regex(R"(from)", std::regex_constants::icase), "f"},
      {std::regex(R"(where)", std::regex_constants::icase), "w"},
      {std::regex(R"(char)", std::regex_constants::icase), "a"}};

  std::string result = str;

  // 对于每一个替换规则，执行替换操作
  for (const auto &r : replacements) {
    result = std::regex_replace(result, r.pattern, r.replacement);
  }

  return result;
}

std::string dump_input_sql()
{
  need_dump() = false;
  std::string s;
  let_mut     sqls = input_sql_recorder();
  sqls.pop_back();
  std::reverse(sqls.begin(), sqls.end());
  int flag = 0;

  //  for (const auto &item : call_stack) {
  //    s += trim(item);
  //    if (++flag % 2 == 0) {
  //      s += '\n';
  //    }
  //  }
  for (const auto &item : sqls) {
    s += trim(item);
    if (++flag % 3 == 0) {
      s += '\n';
    }
  }
  //  s = replaceWords(s);
  return s;
}

// std::vector<std::string> call_stack;

void sql_hook(const char *sql)
{
  input_sql_recorder().emplace_back(sql);
  const char *aaa = "UPDATE Update_select_t2 SET t_name=(select Update_select_t1.t_name from Update_select_t1 where "
                    "Update_select_t1.id=%) WHERE col1=% and col2=%\n";

  const char *bbb = "UPDATE Update_select_t2 SET t_name=(select Update_select_t1.t_name from Update_select_t1 where "
                    "Update_select_t1.id=%) WHERE col1=% and col2=%";
  std::vector<std::string> bug_sql{aaa, bbb};
  for (const auto &item : bug_sql) {
    if (pattern_match(sql, item)) {
      //      need_dump() = true;
    }
  }
}

std::mutex view_mtx;

struct View
{
  Stmt                        *stmt;
  Table                       *table;
  std::unordered_map<int, int> mapping;
  bool                         insertable = true;
};

std::vector<std::pair<std::string, View>> views;

void push_create_view(const std::string &view_name, Stmt *stmt)
{
  std::unique_lock lock(view_mtx);

  Table *table = nullptr;

  std::unordered_map<int, int> mapping;

  bool insertable = true;

  if (let create_table = dynamic_cast<CreateTableStmt *>(stmt)) {
    create_table->materialize();
    let oper = dynamic_cast<SqlExpr *>(create_table->node.select.get())->get_operator();
    traverse_physical_operators(oper, Order::PreOrder, [&](PhysicalOperator *oper) {
      if (let table_scan = dynamic_cast<TableScanPhysicalOperator *>(oper)) {
        table = table_scan->table();
      }
      if (let project = dynamic_cast<ProjectPhysicalOperator *>(oper)) {
        for (const auto &expr : *project->get_project_exprs()) {
          if (let field = dynamic_cast<FieldExpr *>(expr.get())) {
            if (field->get_func() != F_NONE) {
              insertable = false;
            }
          } else {
            insertable = false;
          }
        }
      }
      return RC::SUCCESS;
    });
    int i = 0;
    for (const auto &meta : *table->table_meta().field_metas()) {
      mapping[i] = -1;
      int j      = 0;
      if (strcmp(meta.name(), "null_field") == 0) {
        continue;
      }
      for (const auto &info : create_table->attr_infos()) {
        if (meta.name() == info.name) {
          mapping[i] = j;
        }
        j++;
      }
      i++;
    }
  }

  views.emplace_back(view_name, View{stmt, table, std::move(mapping), insertable});
}

void refresh_views(SQLStageEvent *sql_event)
{
  let db = sql_event->session_event()->session()->get_current_db();
  for (const auto &[name, view] : views) {
    db->drop_table(name.c_str());

    let old_stmt = sql_event->stmt();
    sql_event->set_stmt(view.stmt);
    CreateTableExecutor exe;
    exe.execute(sql_event);
    sql_event->set_stmt(old_stmt);
  }
}

const char *try_convert_view_to_table(const char *view_name)
{
  for (const auto &[name, view] : views) {
    if (view_name == name) {
      return view.table->name();
    }
  }
  return view_name;
}

RC forwarding_delete(Trx *trx, const std::vector<Value> &values, Table *table)
{
  for (const auto &[view_name, view] : views) {
    if (strcmp(view_name.c_str(), table->name()) == 0) {
      std::vector<Value> val(view.mapping.size(), Value::Null());
      //      val[0] = values[0];
      for (int i = 0; i < val.size(); i++) {
        val[i] = values[i + 1];
      }
      //      for (const auto &[from, to] : view.mapping) {
      //        if (to != -1) {
      //          val[from] = values[to + 1];
      //        }
      //      }

      Record record;
      let    true_table = view.table;
      RC     rc         = true_table->make_record(static_cast<int>(val.size()), val.data(), record);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      rc = trx->delete_record(true_table, record);
      if (rc != RC::SUCCESS) {
        return rc;
      }

      return RC::SUCCESS;
    }
  }
  return RC::NOTFOUND;
}

RC forwarding_insert(Trx *trx, const vector<Value> &values, Table *table)
{
  for (const auto &[view_name, view] : views) {
    if (strcmp(view_name.c_str(), table->name()) == 0) {
      if (!view.insertable) {
        return RC::NOT_INSERTABLE;
      }
      std::vector<Value> val(view.mapping.size(), Value::Null());
      for (const auto &[from, to] : view.mapping) {
        if (to != -1) {
          val[from] = values[to];
        }
      }
      Record record;
      let    true_table = view.table;
      RC     rc         = true_table->make_record(static_cast<int>(val.size()), val.data(), record);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      rc = trx->insert_record(true_table, record);
      if (rc != RC::SUCCESS) {
        return rc;
      }

      return RC::SUCCESS;
    }
  }
  return RC::NOTFOUND;
}
