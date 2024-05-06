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
// Created by Meiyi
//

#pragma once

#include "pch.h"
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>
#include <string>

#include "sql/parser/value.h"

class Expression;

/**
 * @defgroup SQLParser SQL Parser
 */

#define DEFINE_AGGRS     \
  DEFINE_AGGR(None)      \
  DEFINE_AGGR(Count)     \
  DEFINE_AGGR(CountStar) \
  DEFINE_AGGR(Min)       \
  DEFINE_AGGR(Max)       \
  DEFINE_AGGR(Sum)       \
  DEFINE_AGGR(Avg)

enum class AggregateFunc : uint8_t
{
#define DEFINE_AGGR(name) name,
  DEFINE_AGGRS
#undef DEFINE_AGGR
};

/**
 * @brief 描述一个属性
 * @ingroup SQLParser
 * @details 属性，或者说字段(column, field)
 * Rel -> Relation
 * Attr -> Attribute
 */
struct RelAttrSqlNode
{
  std::string relation_name;   ///< relation name (may be NULL) 表名
  std::string attribute_name;  ///< attribute name              属性名
  std::string sort_way;        ///< 排序方式
};

struct AttrExprNode
{
  AggregateFunc              func;
  RelAttrSqlNode             attr;
  std::vector<class Field *> fields_replacement;  ///< used for '*'
  explicit AttrExprNode(RelAttrSqlNode attr) : func(AggregateFunc::None), attr(std::move(attr)) {}
  explicit AttrExprNode(AggregateFunc func) : func(func) {}
};

inline RelAttrSqlNode *make_single_field(std::string field_name)
{
  auto node            = new RelAttrSqlNode;
  node->attribute_name = std::move(field_name);
  return node;
}

struct ExprList
{
  std::vector<std::unique_ptr<Expression>> exprs;
};

inline std::string get_display_name(const RelAttrSqlNode &node)
{
  if (node.relation_name.empty()) {
    return node.attribute_name;
  }
  return node.relation_name + "." + node.attribute_name;
  //  return "";
}

// inline std::string get_display_name(const AttrExprNode &node)
//{
//   if (node.func == AggregateFunc::None) {
//     return get_display_name(node.attr);
//   }
//
//   if (node.func == AggregateFunc::CountStar) {
//     return "COUNT(*)";
//   }
//   const char *func;
//   switch (node.func) {
//     case AggregateFunc::None: func = ""; break;
//     case AggregateFunc::Count: func = "COUNT"; break;
//     case AggregateFunc::Min: func = "MIN"; break;
//     case AggregateFunc::Max: func = "MAX"; break;
//     case AggregateFunc::Avg: func = "AVG"; break;
//     default:;
//   }
//   return std::string(func) + "(" + get_display_name(node.attr) + ")";
// }

inline RelAttrSqlNode *make_single_table_field(std::string table_name, std::string field_name)
{
  auto node            = new RelAttrSqlNode;
  node->relation_name  = std::move(table_name);
  node->attribute_name = std::move(field_name);
  return node;
}

inline AttrExprNode *make_aggr(AggregateFunc func) { return new AttrExprNode(func); }

/**
 * @brief 描述比较运算符
 * @ingroup SQLParser
 */
enum CompOp
{
  EQUAL_TO,       ///< "="
  LESS_EQUAL,     ///< "<="
  NOT_EQUAL,      ///< "<>"
  LESS_THAN,      ///< "<"
  GREAT_EQUAL,    ///< ">="
  GREAT_THAN,     ///< ">"
  IS_OP,          // 仅用于在null当中判断
  IS_NOT_OP,      // 仅用于在null当中判断
  IN_OP,          ///< "IN"
  NOT_IN_OP,      ///< "NOT IN
  EXISTS_OP,      ///< "EXISTS"
  NOT_EXISTS_OP,  ///< "NOT EXISTS"
  LIKE_OP,        ///< "like"
  NOT_LIKE_OP,    ///< "not like"
  NO_OP
};

inline bool is_range_comp(CompOp op) { return op == IN_OP || op == NOT_IN_OP; }

/**
 * @brief 表示一个条件比较
 * @ingroup SQLParser
 * @details 条件比较就是SQL查询中的 where a>b 这种。
 * 一个条件比较是有两部分组成的，称为左边和右边。
 * 左边和右边理论上都可以是任意的数据，比如是字段（属性，列），也可以是数值常量。
 * 这个结构中记录的仅仅支持字段和值。
 */
struct ConditionSqlNode
{
  //  int left_is_attr;              ///< TRUE if left-hand side is an attribute
  //                                 ///< 1时，操作符左边是属性名，0时，是属性值
  //  Value          left_value;     ///< left-hand side value if left_is_attr = FALSE
  //  RelAttrSqlNode left_attr;      ///< left-hand side attribute
  std::unique_ptr<class Expression> left;
  CompOp                            comp;  ///< comparison operator
  std::unique_ptr<class Expression> right;
  //  int            right_is_attr;  ///< TRUE if right-hand side is an attribute
  //                                 ///< 1时，操作符右边是属性名，0时，是属性值
  //  RelAttrSqlNode right_attr;     ///< right-hand side attribute if right_is_attr = TRUE 右边的属性
  //  Value          right_value;    ///< right-hand side value if right_is_attr = FALSE
};

struct ConditionTree
{
  std::unique_ptr<ConditionTree> left;
  enum
  {
    AND_OP,
    OR_OP
  } op;
  std::unique_ptr<ConditionTree> right;

  std::unique_ptr<ConditionSqlNode> node;
};

struct GroupBy
{
  std::vector<std::unique_ptr<Expression>> exprs;
};

typedef std::pair<std::string, std::string> NameWithAlias;

struct JoinList
{
  std::vector<NameWithAlias>   relations;
  std::vector<ConditionTree *> conditions;
};
struct UpdateList
{
  std::unordered_map<std::string, Value> attr_and_value;
};

/**
 * @brief 描述一个select语句
 * @ingroup SQLParser
 * @details 一个正常的select语句描述起来比这个要复杂很多，这里做了简化。
 * 一个select语句由三部分组成，分别是select, from, where。
 * select部分表示要查询的字段，from部分表示要查询的表，where部分表示查询的条件。
 * 比如 from 中可以是多个表，也可以是另一个查询语句，这里仅仅支持表，也就是 relations。
 * where 条件 conditions，这里表示使用AND串联起来多个条件。正常的SQL语句会有OR，NOT等，
 * 甚至可以包含复杂的表达式。
 */

struct SelectSqlNode
{
  std::vector<std::unique_ptr<Expression>> select_exprs;  ///< select clause
  std::vector<NameWithAlias>               relations;     ///< 查询的表
  std::unique_ptr<ConditionTree>           conditions;    ///< 查询条件
  std::unique_ptr<ConditionTree>           having;        ///< having
  JoinList                                 join_list;
  GroupBy                                  group_by;  ///<
  std::vector<RelAttrSqlNode>              order_lists;
};

enum FieldFunction
{
  F_NONE,
  F_LENGTH,
  F_ROUND,
  F_DATE_FORMAT
};

/**
 * @brief 算术表达式计算的语法树
 * @ingroup SQLParser
 */
struct CalcSqlNode
{
  std::vector<Expression *> expressions;  ///< calc clause

  ~CalcSqlNode();
};

/**
 * @brief 描述一个insert语句
 * @ingroup SQLParser
 * @details 于Selects类似，也做了很多简化
 */
struct InsertSqlNode
{
  std::string        relation_name;  ///< Relation to insert into
  std::vector<Value> values;         ///< 要插入的值
};

/**
 * @brief 描述一个delete语句
 * @ingroup SQLParser
 */
struct DeleteSqlNode
{
  std::string                    relation_name;  ///< Relation to delete from
  std::unique_ptr<ConditionTree> conditions;
};

/**
 * @brief 描述一个update语句
 * @ingroup SQLParser
 */
struct UpdateSqlNode
{
  std::string relation_name;                                                 ///< Relation to update
  std::unordered_map<std::string, std::unique_ptr<Expression>> update_list;  // 更新的字段，一个attr_name对应一个value
  std::unique_ptr<ConditionTree> conditions;
};

/**
 * @brief 描述一个属性
 * @ingroup SQLParser
 * @details 属性，或者说字段(column, field)
 * Rel -> Relation
 * Attr -> Attribute
 */
struct AttrInfoSqlNode
{
  AttrType    type;    ///< Type of attribute
  std::string name;    ///< Attribute name
  size_t      length;  ///< Length of attribute
  bool        is_allow_null;
};

enum TableType
{
  TABLE_T,
  VIEW_T,
};

/**
 * @brief 描述一个create table语句
 * @ingroup SQLParser
 * @details 这里也做了很多简化。
 */
struct CreateTableSqlNode
{
  std::string                  relation_name;  ///< Relation name
  std::vector<AttrInfoSqlNode> attr_infos;     ///< attributes
  std::unique_ptr<Expression>  select;
  bool                         is_from_select = false;
  TableType                    type           = TABLE_T;
};

/**
 * @brief 描述一个drop table语句
 * @ingroup SQLParser
 */
struct DropTableSqlNode
{
  std::string relation_name;  ///< 要删除的表名
};

/**
 * @brief 描述一个create index语句
 * @ingroup SQLParser
 * @details 创建索引时，需要指定索引名，表名，字段名。
 * 正常的SQL语句中，一个索引可能包含了多个字段，这里仅支持一个字段。
 */
struct CreateIndexSqlNode
{
  std::string index_name;      ///< Index name
  std::string relation_name;   ///< Relation name
  std::vector<std::string> attribute_name;  ///< Attribute name
  bool is_unique = false;
};

/**
 * @brief 描述一个drop index语句
 * @ingroup SQLParser
 */
struct DropIndexSqlNode
{
  std::string index_name;     ///< Index name
  std::string relation_name;  ///< Relation name
};

/**
 * @brief 描述一个desc table语句
 * @ingroup SQLParser
 * @details desc table 是查询表结构信息的语句
 */
struct DescTableSqlNode
{
  std::string relation_name;
};

/**
 * @brief 描述一个load data语句
 * @ingroup SQLParser
 * @details 从文件导入数据到表中。文件中的每一行就是一条数据，每行的数据类型、字段个数都与表保持一致
 */
struct LoadDataSqlNode
{
  std::string relation_name;
  std::string file_name;
};

/**
 * @brief 设置变量的值
 * @ingroup SQLParser
 * @note 当前还没有查询变量
 */
struct SetVariableSqlNode
{
  std::string name;
  Value       value;
};

class ParsedSqlNode;

/**
 * @brief 描述一个explain语句
 * @ingroup SQLParser
 * @details 会创建operator的语句，才能用explain输出执行计划。
 * 一个command就是一个语句，比如select语句，insert语句等。
 * 可能改成SqlCommand更合适。
 */
struct ExplainSqlNode
{
  std::unique_ptr<ParsedSqlNode> sql_node;
};

/**
 * @brief 解析SQL语句出现了错误
 * @ingroup SQLParser
 * @details 当前解析时并没有处理错误的行号和列号
 */
struct ErrorSqlNode
{
  std::string error_msg;
  int         line;
  int         column;
};

/**
 * @brief 表示一个SQL语句的类型
 * @ingroup SQLParser
 */
enum SqlCommandFlag
{
  SCF_ERROR = 0,
  SCF_CALC,
  SCF_SELECT,
  SCF_INSERT,
  SCF_UPDATE,
  SCF_DELETE,
  SCF_CREATE_TABLE,
  SCF_DROP_TABLE,
  SCF_CREATE_INDEX,
  SCF_DROP_INDEX,
  SCF_SYNC,
  SCF_SHOW_TABLES,
  SCF_DESC_TABLE,
  SCF_BEGIN,  ///< 事务开始语句，可以在这里扩展只读事务
  SCF_COMMIT,
  SCF_CLOG_SYNC,
  SCF_ROLLBACK,
  SCF_LOAD_DATA,
  SCF_HELP,
  SCF_EXIT,
  SCF_EXPLAIN,
  SCF_SET_VARIABLE,  ///< 设置变量
};
/**
 * @brief 表示一个SQL语句
 * @ingroup SQLParser
 */
class ParsedSqlNode
{
public:
  enum SqlCommandFlag flag;
  ErrorSqlNode        error;
  CalcSqlNode         calc;
  SelectSqlNode       selection;
  InsertSqlNode       insertion;
  DeleteSqlNode       deletion;
  UpdateSqlNode       update;
  CreateTableSqlNode  create_table;
  DropTableSqlNode    drop_table;
  CreateIndexSqlNode  create_index;
  DropIndexSqlNode    drop_index;
  DescTableSqlNode    desc_table;
  LoadDataSqlNode     load_data;
  ExplainSqlNode      explain;
  SetVariableSqlNode  set_variable;

  class QueryContext  *query_context;  // just for trick use
  class SQLStageEvent *sql_event;

public:
  ParsedSqlNode();
  explicit ParsedSqlNode(SqlCommandFlag flag);
};

/**
 * @brief 表示语法解析后的数据
 * @ingroup SQLParser
 */
class ParsedSqlResult
{
public:
  void                                         add_sql_node(std::unique_ptr<ParsedSqlNode> sql_node);
  std::vector<std::unique_ptr<ParsedSqlNode>> &sql_nodes() { return sql_nodes_; }

private:
  std::vector<std::unique_ptr<ParsedSqlNode>> sql_nodes_;  ///< 这里记录SQL命令。虽然看起来支持多个，但是当前仅处理一个
};
Value *value_init_date(const char *v);

// template <typename TraversedNodeType>
// auto traverse_ast(auto&& node, ref<std::function<RC(TraversedNodeType)>> func)
//{
//   // if node is select
//   using NodeType = std::remove_cvref_t<decltype(node)>;
//
//   if SAME_TYPE (NodeType, TraversedNodeType) {
//     func(node);
//   }
//
//   if SAME_TYPE (NodeType, ParsedSqlNode) {
//     traverse_ast<TraversedNodeType>(node.selection, func);
//   } else if SAME_TYPE (NodeType, SelectSqlNode) {
//     if (node.conditions) {
//       traverse_ast<TraversedNodeType>(*node.conditions, func);
//     }
//   } else if SAME_TYPE (NodeType, ConditionTree) {
//     if (node.left) {
//       traverse_ast<TraversedNodeType>(*node.left, func);
//     }
//     if (node.right) {
//       traverse_ast<TraversedNodeType>(*node.right, func);
//     }
//     if (node.node) {
//       traverse_ast<TraversedNodeType>(*node.node, func);
//     }
//   }
// }