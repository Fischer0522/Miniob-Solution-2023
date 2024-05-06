//
// Created by fischer on 23-10-20.
//

#include "join_stmt.h"

// RC JoinStmt::create(Db *db,Table *left_table,Table *right_table,std::unordered_map<std::string,Table*> *tables,
//      ConditionSqlNode *condition,FilterTree *&tree)
//{
//   RC rc = RC::SUCCESS;
//   tree = new FilterTree;
//   FilterUnit *filter_unit = nullptr;
//   rc = create_join_filter_unit(db,left_table,right_table,tables,condition,filter_unit);
//   tree->filter_unit.reset(filter_unit);
//
//   return rc;
//
// }
RC JoinStmt::create_join_filter_unit(Db *db, Table *left_table, Table *right_table,
    std::unordered_map<std::string, table_entry> *tables, ConditionSqlNode &condition, FilterUnit *&filter_unit)
{
  RC rc = RC::SUCCESS;

  CompOp comp = condition.comp;
  if (comp < EQUAL_TO || comp >= NO_OP) {
    LOG_WARN("invalid compare operator : %d", comp);
    return RC::INVALID_ARGUMENT;
  }

  filter_unit = new FilterUnit;

  // traverse expression tree to fill table in field expr
  auto fill_table = [&](Table *table, Expression *expr) {
    return traverse_expressions<FieldExpr>(expr, Order::PreOrder, [&](FieldExpr *field_expr) {
      RC rc = RC::SUCCESS;

      if (field_expr->table() != nullptr) {
        return rc;
      }

      Table           *table      = nullptr;
      const FieldMeta *field_meta = nullptr;
      rc                          = get_table_and_field(db, table, tables, field_expr->node(), table, field_meta);
      if (rc != RC::SUCCESS || field_expr->node().relation_name.empty()) {
        LOG_WARN("cannot find attr");
        return rc;
      }
      field_expr->field() = Field(table, field_meta);
      return RC::SUCCESS;
    });
  };

  {
    rc = fill_table(left_table, condition.left.get());
    if (rc != RC::SUCCESS) {
      return rc;
    }
    FilterObj filter_obj{std::move(condition.left)};
    //    filter_obj.init_attr(Field(table, field));
    filter_unit->set_left(std::move(filter_obj));
  }

  {
    rc = fill_table(right_table, condition.right.get());
    if (rc != RC::SUCCESS) {
      return rc;
    }
    FilterObj filter_obj{std::move(condition.right)};
    //    filter_obj.init_attr(Field(table, field));
    filter_unit->set_right(std::move(filter_obj));
  }

  filter_unit->set_comp(comp);

  // 检查两个类型是否能够比较
  if (!filter_unit->is_comparable()) {
    rc = RC::NOT_COMPARABLE;
  }

  return rc;
}
RC JoinStmt::create(Db *db, Table *left_table, Table *right_table, std::unordered_map<std::string, table_entry> *tables,
    const ConditionTree *root, FilterTree *&tree)
{
  RC rc = RC::SUCCESS;
  if (root == nullptr) {
    tree = nullptr;
    return rc;
  }

  tree = new FilterTree;
  if (root->node != nullptr) {
    FilterUnit *filter_unit = nullptr;
    rc                      = create_join_filter_unit(
        db, left_table, right_table, tables, const_cast<ConditionSqlNode &>(*root->node), filter_unit);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    tree->filter_unit.reset(filter_unit);
  } else {
    // recursive create
    FilterTree *left = nullptr;
    rc               = create(db, left_table, right_table, tables, root->left.get(), left);
    tree->left.reset(left);

    tree->type = root->op == ConditionTree::AND_OP ? ConjunctionExpr::Type::AND : ConjunctionExpr::Type::OR;

    FilterTree *right = nullptr;
    rc                = create(db, right_table, right_table, tables, root->right.get(), right);
    tree->right.reset(right);
  }
  return rc;
}
