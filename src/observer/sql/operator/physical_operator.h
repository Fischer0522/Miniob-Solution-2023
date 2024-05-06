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
// Created by WangYunlai on 2022/6/7.
//

#pragma once

#include <vector>
#include <memory>
#include <string>

#include "common/rc.h"
#include "sql/expr/tuple.h"
#include "query_context.h"

class Record;
class TupleCellSpec;
class Trx;

/**
 * @brief 物理算子
 * @defgroup PhysicalOperator
 * @details 物理算子描述执行计划将如何执行，比如从表中怎么获取数据，如何做投影，怎么做连接等
 */

/**
 * @brief 物理算子类型
 * @ingroup PhysicalOperator
 */
enum class PhysicalOperatorType
{
  TABLE_SCAN,
  INDEX_SCAN,
  NESTED_LOOP_JOIN,
  EXPLAIN,
  PREDICATE,
  PROJECT,
  CALC,
  STRING_LIST,
  DELETE,
  INSERT,
  AGGREGATE,
  SORT,
  UPDATE,
};

/**
 * @brief 与LogicalOperator对应，物理算子描述执行计划将如何执行
 * @ingroup PhysicalOperator
 */
class PhysicalOperator
{
public:
  PhysicalOperator() = default;

  virtual ~PhysicalOperator();

  /**
   * 这两个函数是为了打印时使用的，比如在explain中
   */
  virtual std::string name() const;
  virtual std::string param() const;

  virtual PhysicalOperatorType type() const = 0;

  virtual RC open(Trx *trx) = 0;
  virtual RC next()         = 0;
  virtual RC close()        = 0;

  virtual Tuple *current_tuple() = 0;

  void add_child(std::unique_ptr<PhysicalOperator> oper) { children_.emplace_back(std::move(oper)); }

  std::vector<std::unique_ptr<PhysicalOperator>>       &children() { return children_; }
  const std::vector<std::unique_ptr<PhysicalOperator>> &children() const { return children_; }

  virtual void set_query_context(QueryContext *context)
  {
    query_context_ = context;

    for (const auto &child : children_) {
      if (child) {
        child->set_query_context(context);
      }
    }
  }

protected:
  std::vector<std::unique_ptr<PhysicalOperator>> children_;
  QueryContext                                  *query_context_;
};
//
///**
// * @brief 物化算子
// * @ingroup PhysicalOperator
// * @details 物化算子是一个特殊的物理算子，它会将数据写入到临时表中，然后再从临时表中读取数据
// */
//class MaterializedPhysicalOperator : public PhysicalOperator
//{
//public:
//  MaterializedPhysicalOperator() = default;
//
//  virtual ~MaterializedPhysicalOperator() {}
//
//  virtual RC open(Trx *trx) override {}
//  RC         next() override;
//  virtual RC close() override {}
//
//  virtual Tuple *current_tuple() override { return *iter_; }
//
//  RC materialize(std::unique_ptr<PhysicalOperator> oper);
//
//  virtual void set_query_context(QueryContext *context) override {}
//
//private:
//  using TupleSet = std::vector<Tuple *>;
//  TupleSet           tuples_;
//  TupleSet::iterator iter_{};
//};

RC traverse_physical_operators(PhysicalOperator *oper, Order order, const std::function<RC(PhysicalOperator *)>& f);
