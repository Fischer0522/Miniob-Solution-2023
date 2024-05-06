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
// Created by WangYunlai on 2022/6/27.
//

#include "common/log/log.h"
#include "sql/operator/update_physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"
#include "sql/stmt/update_stmt.h"

RC UpdatePhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
    return RC::SUCCESS;
  }

  std::unique_ptr<PhysicalOperator> &child = children_[0];
  RC rc = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  trx_ = trx;

  return RC::SUCCESS;
}

RC UpdatePhysicalOperator::next()
{
  LOG_ERROR("test1018");
  RC rc = RC::SUCCESS;
  if (children_.empty()) {
    return RC::RECORD_EOF;
  }
  
  PhysicalOperator *child = children_[0].get();
  while (RC::SUCCESS == (rc = child->next())) {
    bool is_need_update = false;
    LOG_ERROR("test1019");
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }
    if (fragile) {
      return_rc(RC::FRAGILE);
    }
    RowTuple *row_tuple = static_cast<RowTuple *>(tuple);
    Record &record = row_tuple->record();
    LOG_TRACE("test-UPO-system");
    //首先构造数组把所有数据拿出来
    vector<Value> values;
    int value_num = row_tuple->cell_num();
    int start_index = table_->table_meta().null_field_num() + table_->table_meta().sys_field_num();
    for(int i = start_index ; i< value_num ;i++){
        Value value_tmp;
        row_tuple->cell_at(i,value_tmp);
        Field field_tmp = row_tuple->cell_field(i);
        LOG_WARN("test-ontime");
        if (this->field_and_value_.contains(field_tmp.field_name())) {
            LOG_WARN("test-passed");
            if (field_tmp.attr_type() == TEXTS) {
              value_tmp.set_type(TEXTS);
              value_tmp.set_text_string(field_and_value_[field_tmp.field_name()].get_string());
              is_need_update = true;
            } else {
              if (value_tmp.compare(field_and_value_[field_tmp.field_name()]) != 0) {
                is_need_update = true;
              }
              value_tmp= field_and_value_[field_tmp.field_name()];
            }
        }
        values.push_back(value_tmp);
    }

    //然后创建record
    Record record_tmp ;
    rc = table_->make_record(static_cast<int>(values.size()), values.data(), record_tmp);
    if (rc != RC::SUCCESS) {
        return rc;
    }
    if (!is_need_update) {
        continue;
    }
    //最后先删后加
    rc = trx_->delete_record(table_, record);
    rc = trx_->insert_record(table_, record_tmp);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to update record: %s", strrc(rc));
      return rc;
    }
  }

  return RC::RECORD_EOF;
}

RC UpdatePhysicalOperator::close()
{
  if (!children_.empty()) {
    children_[0]->close();
  }
  return RC::SUCCESS;
}
