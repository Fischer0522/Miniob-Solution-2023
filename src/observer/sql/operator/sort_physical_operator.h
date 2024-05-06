#pragma once

#include "sql/operator/physical_operator.h"
#include <vector>
#include <memory>
#include "storage/field/field.h"

/**
 * @brief sort物理算子
 * @ingroup PhysicalOperator
 */

class SortPhysicalOperator : public PhysicalOperator
{
public:
  SortPhysicalOperator() {}
  SortPhysicalOperator(const std::vector<Field> &fields) : fields_(fields) {}

  virtual ~SortPhysicalOperator() = default;

  PhysicalOperatorType type() const override { return PhysicalOperatorType::SORT; }
  void add_expressions(std::vector<std::unique_ptr<Expression>> &&expressions) { expressions_.swap(expressions); }
  RC   open(Trx *trx) override;
  RC   next() override;
  RC   close() override;
  const std::vector<std::unique_ptr<Expression>> &expressions() const { return expressions_; }

  int cell_num() const { return tuple_.cell_num(); }

  Tuple *current_tuple() override;
  void   set_query_context(QueryContext *context) override
  {
    PhysicalOperator::set_query_context(context);
    for (auto &expr : expressions_) {
      expr->set_query_context(context);
    }
  }

private:
  ProjectTuple                             tuple_;
  std::vector<Field>                       fields_;
  std::vector<ValueListTuple>              tuples_;
  std::vector<ValueListTuple *>            tuple_ptrs_;
  std::vector<std::unique_ptr<Expression>> expressions_;
  std::vector<std::pair<int, bool>>        valid_area_;
  int                                      tmp_ = 0;
};