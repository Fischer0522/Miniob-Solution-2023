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
// Created by Wangyunlai on 2021/5/14.
//

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_set>

#include "common/log/log.h"
#include "common/lang/bitmap.h"
#include "sql/expr/tuple_cell.h"
#include "sql/parser/parse.h"
#include "sql/parser/value.h"
#include "storage/record/record.h"
#include "expression.h"

class Table;

/**
 * @defgroup Tuple
 * @brief Tuple 元组，表示一行数据，当前返回客户端时使用
 * @details
 * tuple是一种可以嵌套的数据结构。
 * 比如select t1.a+t2.b from t1, t2;
 * 需要使用下面的结构表示：
 * @code {.cpp}
 *  Project(t1.a+t2.b)
 *        |
 *      Joined
 *      /     \
 *   Row(t1) Row(t2)
 * @endcode
 *
 */

/**
 * @brief 元组的结构，包含哪些字段(这里成为Cell)，每个字段的说明
 * @ingroup Tuple
 */
class TupleSchema
{
public:
  void                 append_cell(const TupleCellSpec &cell) { cells_.push_back(cell); }
  void                 append_cell(const char *table, const char *field) { append_cell(TupleCellSpec(table, field)); }
  void                 append_cell(const char *alias) { append_cell(TupleCellSpec(alias)); }
  int                  cell_num() const { return static_cast<int>(cells_.size()); }
  const TupleCellSpec &cell_at(int i) const { return cells_[i]; }

private:
  std::vector<TupleCellSpec> cells_;
};

/**
 * @brief 元组的抽象描述
 * @ingroup Tuple
 */
class Tuple
{
public:
  Tuple()          = default;
  virtual ~Tuple() = default;

  /**
   * @brief 获取元组中的Cell的个数
   * @details 个数应该与tuple_schema一致
   */
  virtual int cell_num() const = 0;

  /**
   * @brief 获取指定位置的Cell
   *
   * @param index 位置
   * @param[out] cell  返回的Cell
   */
  virtual RC cell_at(int index, Value &cell) const = 0;

  /**
   * @brief 根据cell的描述，获取cell的值
   *
   * @param spec cell的描述
   * @param[out] cell 返回的cell
   */
  virtual RC           find_cell(const TupleCellSpec &spec, Value &cell) const = 0;
  virtual const Table *get_table() { return nullptr; };

  virtual std::string to_string() const
  {
    std::string str;
    const int   cell_num = this->cell_num();
    for (int i = 0; i < cell_num - 1; i++) {
      Value cell;
      cell_at(i, cell);
      str += cell.to_string();
      str += ", ";
    }

    if (cell_num > 0) {
      Value cell;
      cell_at(cell_num - 1, cell);
      str += cell.to_string();
    }
    return str;
  }
};

/**
 * @brief 一行数据的元组
 * @ingroup Tuple
 * @details 直接就是获取表中的一条记录
 */
class RowTuple : public Tuple
{
public:
  RowTuple() = default;
  virtual ~RowTuple()
  {
    for (FieldExpr *spec : speces_) {
      delete spec;
    }
    speces_.clear();
  }
  const Table *get_table() override { return this->table_; }
  void         set_record(Record *record)
  {
    this->null_field_set_.clear();
    this->record_ = record;
    Value bitmap_val;
    auto  table_meta = table_->table_meta();
    cell_at(table_meta.sys_field_num(), bitmap_val);
    std::string    s          = bitmap_val.get_string();
    char          *bitmap_str = s.data();
    common::Bitmap bitmap(bitmap_str, 32);
    int            start_index = table_meta.sys_field_num() + table_meta.null_field_num();
    for (int i = start_index; i < table_meta.field_num(); i++) {
      int field_index = i - start_index;
      if (bitmap.get_bit(field_index)) {
        null_field_set_.insert(table_meta.field(i)->name());
      }
    }
  }

  void set_schema(const Table *table, const std::vector<FieldMeta> *fields)
  {
    table_ = table;
    this->speces_.clear();
    this->speces_.reserve(fields->size());
    for (const FieldMeta &field : *fields) {
      speces_.push_back(new FieldExpr(table, &field));
    }
  }

  int cell_num() const override { return speces_.size(); }

  RC cell_at(int index, Value &cell) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }

    FieldExpr       *field_expr = speces_[index];
    const FieldMeta *field_meta = field_expr->field().meta();
    cell.set_type(field_meta->type());
    cell.set_data(this->record_->data() + field_meta->offset(), field_meta->len());
    if (index != 0) {
      if (null_field_set_.contains(field_meta->name())) {
        if (field_meta->is_allow_null()) {
          cell.set_null();
        } else {
          return RC::INVALID_ARGUMENT;
        }
      }
    }
    if (field_meta->type() == TEXTS) {
      auto* non_const_table = const_cast<Table *>(table_);
      std::string res_str;
      for (const auto page_num :cell.get_text_page_numbers()) {
        Record part_page_record;
        non_const_table->get_text_data(page_num,&part_page_record);
        std::string part_page_str(part_page_record.data(),part_page_record.len());
        res_str += part_page_str;
      }
      cell.set_text_string(res_str.c_str());
    }

    return RC::SUCCESS;
  }

  Field cell_field(int index)
  {
    FieldExpr *field_expr = speces_[index];
    return field_expr->field();
  }

  RC find_cell(const TupleCellSpec &spec, Value &cell) const override
  {
    const char *table_name = spec.table_name();
    const char *field_name = spec.field_name();
    if (0 != strcmp(table_name, table_->name())) {
      return RC::NOTFOUND;
    }

    for (size_t i = 0; i < speces_.size(); ++i) {
      const FieldExpr *field_expr = speces_[i];
      const Field     &field      = field_expr->field();
      if (0 == strcmp(field_name, field.field_name())) {
        return cell_at(i, cell);
      }
    }
    return RC::NOTFOUND;
  }

#if 0
  RC cell_spec_at(int index, const TupleCellSpec *&spec) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }
    spec = speces_[index];
    return RC::SUCCESS;
  }
#endif

  Record &record() { return *record_; }

  const Record &record() const { return *record_; }

private:
  Record                         *record_ = nullptr;
  const Table                    *table_  = nullptr;
  std::vector<FieldExpr *>        speces_;
  std::unordered_set<std::string> null_field_set_;
};

/**
 * @brief 从一行数据中，选择部分字段组成的元组，也就是投影操作
 * @ingroup Tuple
 * @details 一般在select语句中使用。
 * 投影也可以是很复杂的操作，比如某些字段需要做类型转换、重命名、表达式运算、函数计算等。
 * 当前的实现是比较简单的，只是选择部分字段，不做任何其他操作。
 */
class ProjectTuple : public Tuple
{
public:
  ProjectTuple() = default;
  virtual ~ProjectTuple()
  {
    for (TupleCellSpec *spec : speces_) {
      delete spec;
    }
    speces_.clear();
  }

  void set_tuple(Tuple *tuple) { this->tuple_ = tuple; }

  void add_cell_spec(TupleCellSpec *spec) { speces_.push_back(spec); }
  int  cell_num() const override { return speces_.size(); }

  RC cell_at(int index, Value &cell) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      return RC::INTERNAL;
    }
    if (tuple_ == nullptr) {
      return RC::INTERNAL;
    }

    const TupleCellSpec *spec = speces_[index];
    return tuple_->find_cell(*spec, cell);
  }

  RC find_cell(const TupleCellSpec &spec, Value &cell) const override { return tuple_->find_cell(spec, cell); }

#if 0
  RC cell_spec_at(int index, const TupleCellSpec *&spec) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      return RC::NOTFOUND;
    }
    spec = speces_[index];
    return RC::SUCCESS;
  }
#endif
private:
  std::vector<TupleCellSpec *> speces_;
  Tuple                       *tuple_ = nullptr;
};

class ExpressionTuple : public Tuple
{
public:
  ExpressionTuple(std::vector<std::unique_ptr<Expression>> &expressions) : expressions_(expressions) {}

  virtual ~ExpressionTuple() {}

  int cell_num() const override { return expressions_.size(); }

  RC cell_at(int index, Value &cell) const override
  {
    if (index < 0 || index >= static_cast<int>(expressions_.size())) {
      return RC::INTERNAL;
    }

    const Expression *expr = expressions_[index].get();
    return expr->try_get_value(cell);
  }

  RC find_cell(const TupleCellSpec &spec, Value &cell) const override
  {
    for (const std::unique_ptr<Expression> &expr : expressions_) {
      if (0 == strcmp(spec.alias(), expr->name().c_str())) {
        return expr->try_get_value(cell);
      }
    }
    return RC::NOTFOUND;
  }

private:
  const std::vector<std::unique_ptr<Expression>> &expressions_;
};

/**
 * @brief 一些常量值组成的Tuple
 * @ingroup Tuple
 */
class ValueListTuple : public Tuple
{
public:
  ValueListTuple()          = default;
  virtual ~ValueListTuple() = default;

  ValueListTuple(std::vector<Value> cells) : cells_(std::move(cells)) {}
  ValueListTuple(ValueListTuple &&other)
  {
    tmp_valid_area_ = other.tmp_valid_area_;
    cells_.swap(other.cells_);
  }

  void set_cells(const std::vector<Value> &cells) { cells_ = cells; }

  virtual int cell_num() const override { return static_cast<int>(cells_.size()); }

  virtual RC cell_at(int index, Value &cell) const override
  {
    if (index < 0 || index >= cell_num()) {
      return RC::NOTFOUND;
    }

    cell = cells_[index];
    return RC::SUCCESS;
  }

  virtual RC find_cell(const TupleCellSpec &spec, Value &cell) const override { return RC::INTERNAL; }

  auto &cells() const { return cells_; }
  // 如果是负值就是降序，正值就是升序
  std::vector<std::pair<int, bool>> *tmp_valid_area_;

private:
  std::vector<Value> cells_;
};

/**
 * @brief 将两个tuple合并为一个tuple
 * @ingroup Tuple
 * @details 在join算子中使用
 */
class JoinedTuple : public Tuple
{
public:
  JoinedTuple()          = default;
  virtual ~JoinedTuple() = default;

  void set_left(Tuple *left) { left_ = left; }
  void set_right(Tuple *right) { right_ = right; }

  int cell_num() const override { return left_->cell_num() + right_->cell_num(); }

  RC cell_at(int index, Value &value) const override
  {
    const int left_cell_num = left_->cell_num();
    if (index > 0 && index < left_cell_num) {
      return left_->cell_at(index, value);
    }

    if (index >= left_cell_num && index < left_cell_num + right_->cell_num()) {
      return right_->cell_at(index - left_cell_num, value);
    }

    return RC::NOTFOUND;
  }

  RC find_cell(const TupleCellSpec &spec, Value &value) const override
  {
    RC rc = RC::NOTFOUND;

    if (spec.table_index <= 0) {
      rc = left_->find_cell(spec, value);
    }

    if (rc == RC::SUCCESS || rc != RC::NOTFOUND) {
      return rc;
    }

    if (spec.table_index <= -1 || spec.table_index >= 1) {
      const_cast<TupleCellSpec&>(spec).table_index--;
      rc = right_->find_cell(spec, value);
    }

    return rc;
  }

private:
  Tuple *left_  = nullptr;
  Tuple *right_ = nullptr;
};

class AggregateTuple : public Tuple
{
public:
  int cell_num() const override { return fields.size() + aggregate_values.size(); }

  RC find_cell(const TupleCellSpec &spec, Value &cell) const override
  {
    std::size_t index = 0;

    // search fields
    for (; index < fields.size(); index++) {
      const auto &field = fields[index];
      if (strcmp(spec.table_name(), field->table_name()) == 0 && strcmp(spec.field_name(), field->field_name()) == 0) {
        return cell_at(index, cell);
      }
    }

    // search aggregates
    for (index = 0; index < aggregate_values.size(); index++) {
      const auto &aggr = aggregate_values[index];
      if (strcmp(spec.alias(), aggr->name().c_str()) == 0) {
        return cell_at(index + fields.size(), cell);
      }
    }

    return RC::NOTFOUND;
  }

  RC cell_at(int index, Value &cell) const override
  {
    if (index < field_values.size()) {
      return field_values[index]->try_get_value(cell);
    }

    return aggregate_values[index - field_values.size()]->try_get_value(cell);
  }

  void add_field(Field *field, std::unique_ptr<ValueExpr> value)
  {
    fields.push_back(field);
    field_values.push_back(std::move(value));
  }

  void add_aggregate(std::unique_ptr<ValueExpr> value) { aggregate_values.push_back(std::move(value)); }

private:
  std::vector<Field *>                    fields;
  std::vector<std::unique_ptr<ValueExpr>> field_values;
  std::vector<std::unique_ptr<ValueExpr>> aggregate_values;
};