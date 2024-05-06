#include "common/log/log.h"
#include "sql/operator/sort_physical_operator.h"
#include "sql/operator/project_physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
#include <algorithm>
#include <execution>

// 首先open的时候调用完next，然后排序，后面next的时候直接内存
// bool compareOrderBy(const ValueListTuple *p_lhs, const ValueListTuple *p_rhs)
//{
//
//}
RC SortPhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
    return RC::SUCCESS;
  }

  PhysicalOperator *child = children_[0].get();
  RC                rc    = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  std::vector<std::unique_ptr<Expression>> *exp = static_cast<ProjectPhysicalOperator *>(child)->get_project_exprs();
  std::vector<std::pair<const char *, const char *>> table_field_names;
  for (const auto &expr : *exp) {
    FieldExpr *fexpr = dynamic_cast<FieldExpr *>(expr.get());
    table_field_names.emplace_back(fexpr->table_name(), fexpr->field_name());
  }
  for (const Field &field : fields_) {
    const char *field_to_find = field.field_name();
    const char *table_to_find = field.table_name();
    bool        sort_way      = field.get_sort_way();

    auto it =
        std::find(table_field_names.begin(), table_field_names.end(), std::make_pair(table_to_find, field_to_find));

    if (it != table_field_names.end()) {
      int index = std::distance(table_field_names.begin(), it);
      valid_area_.emplace_back(index, sort_way);
    }
  }

  rc = RC::SUCCESS;
  while (RC::SUCCESS == (rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }
    let value_list_tuple = static_cast<ValueListTuple *>(tuple);

    tuples_.emplace_back(std::move(*value_list_tuple));
    assert(value_list_tuple->cell_num() == 0);
  }

  for (auto &tuple : tuples_) {
    tuple.tmp_valid_area_ = &valid_area_;
    tuple_ptrs_.push_back(&tuple);
  }

  std::unordered_map<const ValueListTuple *, std::vector<Value>> tuple_cells;
  for (const auto &tuple : tuples_) {
    let_ref            area = tuple.tmp_valid_area_;
    std::vector<Value> vals;
    Value              val;
    for (int i = 0; i < tuple.cell_num(); i++) {
      tuple.cell_at(i, val);
      vals.push_back(std::move(val));
    }
    tuple_cells[&tuple] = std::move(vals);
  }

  let compareOrderBy = [&](const ValueListTuple *p_lhs, const ValueListTuple *p_rhs) {
    assert(p_lhs != nullptr && p_rhs != nullptr);
    assert(p_lhs->tmp_valid_area_ != nullptr && p_rhs->tmp_valid_area_ != nullptr);
    let_ref lhs = *p_lhs;
    let_ref rhs = *p_rhs;
    // sortway为真表示升序，假为降序
    std::unordered_set<int> record_has;
    for (int i = (int)lhs.tmp_valid_area_->size() - 1; i >= 0; i--) {
      record_has.insert((*lhs.tmp_valid_area_)[i].first);
      let_ref tmpA = tuple_cells[p_lhs][(*lhs.tmp_valid_area_)[i].first];
      //      Value tmpA;
      //      lhs.cell_at((*lhs.tmp_valid_area_)[i].first, tmpA);
      let_ref tmpB = tuple_cells[p_rhs][(*rhs.tmp_valid_area_)[i].first];
      //      Value tmpB;
      //      rhs.cell_at((*lhs.tmp_valid_area_)[i].first, tmpB);
      if ((*lhs.tmp_valid_area_)[i].second)  // 如果是升序
      {
        int diff = tmpA.compare(tmpB);
        if (diff == 0) {
          continue;
        } else {
          if (diff < 0) {
            return true;
          } else {
            return false;
          }
        }
      } else {
        int diff = tmpA.compare(tmpB);
        if (diff == 0) {
          continue;
        } else {
          if (diff > 0) {
            return true;
          } else {
            return false;
          }
        }
      }
    }

    for (int i = 0; i < lhs.cell_num(); i++) {
      auto it = record_has.find(i);
      if (it == record_has.end()) {
        let_ref tmpA = tuple_cells[p_lhs][i];
        let_ref tmpB = tuple_cells[p_rhs][i];
        //        Value tmpA;
        //        lhs.cell_at(i, tmpA);
        //        Value tmpB;
        //        rhs.cell_at(i, tmpB);
        int diff = tmpA.compare(tmpB);
        if (diff == 0) {
          continue;
        } else {
          if (diff < 0) {
            return true;
          } else {
            return false;
          }
        }
      } else {
        continue;
      }
    }

    return true;
  };

  std::sort(std::execution::par, tuple_ptrs_.begin(), tuple_ptrs_.end(), compareOrderBy);

  if (rc == RC::RECORD_EOF) {
    return RC::SUCCESS;
  }

  return rc;
}

RC SortPhysicalOperator::next()
{
  if (tmp_ == tuples_.size()) {
    return RC::RECORD_EOF;
  }
  return RC::SUCCESS;
}

RC SortPhysicalOperator::close()
{
  if (!children_.empty()) {
    children_[0]->close();
  }
  return RC::SUCCESS;
}
Tuple *SortPhysicalOperator::current_tuple() { return static_cast<Tuple *>(tuple_ptrs_[tmp_++]); }