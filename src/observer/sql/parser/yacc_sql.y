
%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_set>
#include <algorithm>
#include <utility>

#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/parser/parse_defs.h"
#include "sql/parser/yacc_sql.hpp"
#include "sql/parser/lex_sql.h"
#include "sql/expr/expression.h"

using namespace std;

string token_name(const char *sql_string, YYLTYPE *llocp)
{
  return string(sql_string + llocp->first_column, llocp->last_column - llocp->first_column + 1);
}

int yyerror(YYLTYPE *llocp, const char *sql_string, ParsedSqlResult *sql_result, yyscan_t scanner, const char *msg)
{
  std::unique_ptr<ParsedSqlNode> error_sql_node = std::make_unique<ParsedSqlNode>(SCF_ERROR);
  error_sql_node->error.error_msg = msg;
  error_sql_node->error.line = llocp->first_line;
  error_sql_node->error.column = llocp->first_column;
  sql_result->add_sql_node(std::move(error_sql_node));
  return 0;
}

ArithmeticExpr *create_arithmetic_expression(ArithmeticExpr::Type type,
                                             Expression *left,
                                             Expression *right,
                                             const char *sql_string,
                                             YYLTYPE *llocp)
{
  ArithmeticExpr *expr = new ArithmeticExpr(type, left, right);
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}

%}

%define api.pure full
%define parse.error verbose
/** 启用位置标识 **/
%locations
%lex-param { yyscan_t scanner }
/** 这些定义了在yyparse函数中的参数 **/
%parse-param { const char * sql_string }
%parse-param { ParsedSqlResult * sql_result }
%parse-param { void * scanner }

//标识tokens
%token  SEMICOLON
        CREATE
        DROP
        TABLE
        VIEW
        TABLES
        INDEX
        CALC
        SELECT
        DESC
        SHOW
        SYNC
        INSERT
        DELETE
        UPDATE
        LBRACE
        RBRACE
        UNIQUE
        ORDER
        BY
        ASC
        COMMA
        TRX_BEGIN
        TRX_COMMIT
        TRX_ROLLBACK
        INT_T
        STRING_T
        FLOAT_T
        DATE_T
        NULL_T
        TEXT_T
        HELP
        EXIT
        DOT //QUOTE
        INTO
        VALUES
        FROM
        WHERE
        AND
        OR
        SET
        ON
        LOAD
        DATA
        INFILE
        EXPLAIN
        IS
        EQ
        LT
        GT
        LE
        GE
        NE
        SUM
        MAX
        MIN
        COUNT
        AVG
        INNER
        JOIN
        GROUP
        IN
        NOT
        EXISTS
        AS
        LENGTH
        ROUND
        DATE_FORMAT
        LIKE
        HAVING

/** union 中定义各种数据类型，真实生成的代码也是union类型，所以不能有非POD类型的数据 **/
%union {
  ParsedSqlNode *                   sql_node;
  ConditionSqlNode *                condition;
  Value *                           value;
  enum CompOp                       comp;
  RelAttrSqlNode *                  rel_attr;
  std::vector<AttrInfoSqlNode> *    attr_infos;
  AttrInfoSqlNode *                 attr_info;
  Expression *                      expression;
  std::vector<Expression *> *       expression_list;
  TableType                         table_type;
  std::vector<Value> *              value_list;
  ConditionTree *                   condition_tree;
  std::vector<RelAttrSqlNode> *     rel_attr_list;
  GroupBy *                         group_by;
  ExprList *                        select_list;
  std::vector<NameWithAlias> *      relation_list;
  JoinList *                        join_list;
  std::vector<std::string> *        index_list_attr;
  bool                              unique_flag_type;
  AggregateFunc                     aggregate_func;
  AttrExprNode *                    aggregate_node;
  AttrExprNode *                    expr_node;
  char *                            string;
  int                               number;
  float                             floats;
  NameWithAlias *                   name_with_alias;
  std::unordered_map<std::string, std::unique_ptr<Expression>>* update_list;
}

%token <number> NUMBER
%token <floats> FLOAT
%token <string> _id
%token <string> DATE_STR
%token <string> SSS
//非终结符

/** type 定义了各种解析后的结果输出的是什么类型。类型对应了 union 中的定义的成员变量名称 **/
%type <name_with_alias>     table_name
%type <number>              type
%type <string>              ID
%type <condition>           condition
%type <value>               value
%type <number>              number
%type <comp>                comp_op
%type <rel_attr>            rel_attr
%type <attr_infos>          attr_def_list
%type <attr_info>           attr_def
%type <value_list>          value_list
%type <condition_tree>      where
%type <condition_tree>      having
%type <condition_tree>      condition_tree
%type <rel_attr>            order_by_
%type <rel_attr_list>       order_by_list
%type <rel_attr_list>       order_by_list_all
%type <group_by>            group_by
%type <join_list>           inner_join_list
%type <select_list>         select_list
%type<update_list>          update_list
%type<unique_flag_type>     unique_flag
%type <aggregate_func>      aggr_func
%type <aggregate_node>      aggr
%type <relation_list>       rel_list
%type <index_list_attr>     index_list
%type <expression>          expression
%type <expression_list>     expression_list
%type <table_type>          table_type
%type <sql_node>            calc_stmt
%type <sql_node>            select_stmt
%type <sql_node>            insert_stmt
%type <sql_node>            update_stmt
%type <sql_node>            delete_stmt
%type <sql_node>            create_table_stmt
%type <sql_node>            drop_table_stmt
%type <sql_node>            show_tables_stmt
%type <sql_node>            desc_table_stmt
%type <sql_node>            create_index_stmt
%type <sql_node>            drop_index_stmt
%type <sql_node>            sync_stmt
%type <sql_node>            begin_stmt
%type <sql_node>            commit_stmt
%type <sql_node>            rollback_stmt
%type <sql_node>            load_data_stmt
%type <sql_node>            explain_stmt
%type <sql_node>            set_variable_stmt
%type <sql_node>            help_stmt
%type <sql_node>            exit_stmt
%type <sql_node>            command_wrapper
// commands should be a list but I use a single command instead
%type <sql_node>            commands

%left '+' '-'
%left '*' '/'

%left OR
%left AND
%nonassoc UMINUS
%%

commands: command_wrapper opt_semicolon  //commands or sqls. parser starts here.
  {
    std::unique_ptr<ParsedSqlNode> sql_node = std::unique_ptr<ParsedSqlNode>($1);
    sql_result->add_sql_node(std::move(sql_node));
  }
  ;

command_wrapper:
    calc_stmt
  | select_stmt
  | insert_stmt
  | update_stmt
  | delete_stmt
  | create_table_stmt
  | drop_table_stmt
  | show_tables_stmt
  | desc_table_stmt
  | create_index_stmt
  | drop_index_stmt
  | sync_stmt
  | begin_stmt
  | commit_stmt
  | rollback_stmt
  | load_data_stmt
  | explain_stmt
  | set_variable_stmt
  | help_stmt
  | exit_stmt
    ;

exit_stmt:      
    EXIT {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      $$ = new ParsedSqlNode(SCF_EXIT);
    };

help_stmt:
    HELP {
      $$ = new ParsedSqlNode(SCF_HELP);
    };

sync_stmt:
    SYNC {
      $$ = new ParsedSqlNode(SCF_SYNC);
    }
    ;

begin_stmt:
    TRX_BEGIN  {
      $$ = new ParsedSqlNode(SCF_BEGIN);
    }
    ;

commit_stmt:
    TRX_COMMIT {
      $$ = new ParsedSqlNode(SCF_COMMIT);
    }
    ;

rollback_stmt:
    TRX_ROLLBACK  {
      $$ = new ParsedSqlNode(SCF_ROLLBACK);
    }
    ;

ID:
    _id {
      $$ = $1;
    }
    | DATA {
      $$ = strdup(token_name(sql_string, &@$).c_str());
    }
    | aggr_func {
      $$ = strdup(token_name(sql_string, &@$).c_str());
    }
    ;

drop_table_stmt:    /*drop table 语句的语法解析树*/
    DROP TABLE ID {
      $$ = new ParsedSqlNode(SCF_DROP_TABLE);
      $$->drop_table.relation_name = $3;
      free($3);
    };

show_tables_stmt:
    SHOW TABLES {
      $$ = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
    ;

desc_table_stmt:
    DESC ID  {
      $$ = new ParsedSqlNode(SCF_DESC_TABLE);
      $$->desc_table.relation_name = $2;
      free($2);
    }
    ;

create_index_stmt:    /*create index 语句的语法解析树*/
    CREATE unique_flag INDEX ID ON ID LBRACE index_list RBRACE
    {
      $$ = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = $$->create_index;
      if($2 == true){
        create_index.is_unique = true ;
      }
      create_index.index_name = $4;
      create_index.relation_name = $6;
      create_index.attribute_name.swap(*$8);
      free($4);
      free($6);
      free($8);
    }
    ;

unique_flag:
    /* empty */
    {
      $$ = false;
    }
    | UNIQUE
    {
      $$ = true ;
    }
    ;

index_list:
    ID
    {
      $$ = new std::vector<std::string> ;
      $$->emplace_back($1) ;
    }
    | ID COMMA index_list
    {
      if($3 != nullptr){
        $$ = $3;
      }else{
        $$ = new std::vector<std::string> ;
      }
      $$->emplace_back($1);
    }
    ;

drop_index_stmt:      /*drop index 语句的语法解析树*/
    DROP INDEX ID ON ID
    {
      $$ = new ParsedSqlNode(SCF_DROP_INDEX);
      $$->drop_index.index_name = $3;
      $$->drop_index.relation_name = $5;
      free($3);
      free($5);
    }
    ;

table_type:
    VIEW {
        $$ = VIEW_T;
    }
    | TABLE {
        $$ = TABLE_T;
    }
    ;

create_table_stmt:    /*create table 语句的语法解析树*/
    CREATE table_type ID AS select_stmt
    {
      $$ = new ParsedSqlNode(SCF_CREATE_TABLE);
      $$->create_table.relation_name = $3;
      $$->create_table.select = std::make_unique<SqlExpr>($5);
      $$->create_table.is_from_select = true;
      $$->create_table.type = $2;
      free($3);
    }
    | CREATE table_type ID LBRACE attr_def attr_def_list RBRACE AS select_stmt {
      $$ = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = $$->create_table;
      create_table.relation_name = $3;
      free($3);

      std::vector<AttrInfoSqlNode> *src_attrs = $6;

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*$5);
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete $5;

      $$->create_table.select = std::make_unique<SqlExpr>($9);
      $$->create_table.is_from_select = true;
      $$->create_table.type = $2;
    }
    | CREATE table_type ID LBRACE attr_def attr_def_list RBRACE select_stmt {
      $$ = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = $$->create_table;
      create_table.relation_name = $3;
      free($3);

      std::vector<AttrInfoSqlNode> *src_attrs = $6;

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*$5);
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete $5;

      $$->create_table.select = std::make_unique<SqlExpr>($8);
      $$->create_table.is_from_select = true;
      $$->create_table.type = $2;
    }
    | CREATE table_type ID LBRACE attr_def attr_def_list RBRACE
    {
      if ($2 != TABLE_T) {
        yyerror(&@$, sql_string, sql_result, scanner, "only table can be created");
      }
      $$ = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = $$->create_table;
      create_table.relation_name = $3;
      free($3);

      std::vector<AttrInfoSqlNode> *src_attrs = $6;

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*$5);
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete $5;
      $$->create_table.type = $2;
    }
    ;
attr_def_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA attr_def attr_def_list
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<AttrInfoSqlNode>;
      }
      $$->emplace_back(*$2);
      delete $2;
    }
    ;
    
attr_def:
    ID type LBRACE number RBRACE
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->length = $4;
      $$->is_allow_null = true;
      free($1);
    }
    | ID type LBRACE number RBRACE NULL_T
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->length = $4;
      $$->is_allow_null = true;
      free($1);
    }
    | ID type LBRACE number RBRACE NOT NULL_T
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
       $$->length = $4;
       $$->is_allow_null = false;
       free($1);
    }
    | ID {
      $$ = new AttrInfoSqlNode;
      $$->name = $1;
      $$->length = 4;
      $$->is_allow_null = true;
      free($1);
    }
    | ID type
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->length = 4;
      if ($$->type == TEXTS) {
        $$->length = 36;
      }
      $$->is_allow_null = true;
      free($1);
    }
    | ID type NULL_T
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->length = 4;
      if ($$->type == TEXTS) {
        $$->length = 36;
      }
      $$->is_allow_null = true;
      free($1);
    }
    | ID type NOT NULL_T
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->length = 4;
      if ($$->type == TEXTS) {
        $$->length = 36;
      }
      $$->is_allow_null = false;
      free($1);
    }
    ;
number:
    NUMBER {$$ = $1;}
    ;
type:
    INT_T      { $$=INTS; }
    | STRING_T { $$=CHARS; }
    | FLOAT_T  { $$=FLOATS; }
    | DATE_T   { $$=DATES;  }
    | TEXT_T   { $$=TEXTS;}
    ;
insert_stmt:        /*insert   语句的语法解析树*/
    INSERT INTO ID VALUES LBRACE value value_list RBRACE 
    {
      $$ = new ParsedSqlNode(SCF_INSERT);
      $$->insertion.relation_name = $3;
      if ($7 != nullptr) {
        $$->insertion.values.swap(*$7);
      }
      $$->insertion.values.emplace_back(*$6);
      std::reverse($$->insertion.values.begin(), $$->insertion.values.end());
      delete $6;
      free($3);
    }
    ;

value_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA value value_list  { 
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<Value>;
      }
      $$->emplace_back(*$2);
      delete $2;
    }
    ;
value:
    NUMBER {
      $$ = new Value((int)$1);
      $$->set_name(token_name(sql_string, &@$));
      @$ = @1;
    }
    | FLOAT {
      $$ = new Value((float)$1);
      $$->set_name(token_name(sql_string, &@$));
      @$ = @1;
    }
    | '-' NUMBER {
      $$ = new Value((int)-$2);
      $$->set_name(token_name(sql_string, &@$));
      @$ = @1;
    }
    | '-' FLOAT {
      $$ = new Value((float)-$2);
      $$->set_name(token_name(sql_string, &@$));
      @$ = @1;
    }
    |DATE_STR {
      char *tmp = common::substr($1,1,strlen($1)-2);
      $$ = value_init_date(tmp);
      $$->set_name(token_name(sql_string, &@$));
      free(tmp);
      if (!$$->get_date().isValid()) {
        return -1;
      }
    }
    |NULL_T {
      $$ = new Value("");
      $$->set_name(token_name(sql_string, &@$));
      $$->set_null();
    }
    |SSS {
      char *tmp = common::substr($1,1,strlen($1)-2);
      $$  = new Value(tmp);
      free(tmp);
    }

    ;
    
delete_stmt:    /*  delete 语句的语法解析树*/
    DELETE FROM ID where 
    {
      $$ = new ParsedSqlNode(SCF_DELETE);
      $$->deletion.relation_name = $3;
      if ($4 != nullptr) {
        $$->deletion.conditions.reset($4);
      }
      free($3);
    }
    ;
update_stmt:      /*  update 语句的语法解析树*/
    UPDATE ID SET ID EQ expression update_list where
    {
      $$ = new ParsedSqlNode(SCF_UPDATE);
      $$->update.relation_name = $2;
      if ($7 != nullptr) {
         $$->update.update_list.swap(*$7);
         delete $7;
      }
      $$->update.update_list.insert(std::make_pair($4, std::unique_ptr<Expression>($6)));
      if ($8 != nullptr) {
        $$->update.conditions.reset($8);
      }
      free($2);
      free($4);
    }
    ;
table_name:
    ID {
      $$ = new NameWithAlias($1, "");
      free($1);
    }
    | ID AS ID {
      $$ = new NameWithAlias($1, $3);
      free($1);
      free($3);
    }
    | ID ID {
      $$ = new NameWithAlias($1, $2);
      free($1);
      free($2);
    }
    ;
select_stmt:        /*  select 语句的语法解析树*/
    SELECT select_list FROM table_name rel_list inner_join_list where group_by having order_by_list_all
    {
      $$ = new ParsedSqlNode(SCF_SELECT);
      if ($2 != nullptr) {
        //std::reverse($2->nodes.begin(), $2->nodes.end());
        $$->selection.select_exprs.swap($2->exprs);
        delete $2;
      }
      if ($5 != nullptr) {
        $$->selection.relations.swap(*$5);
        delete $5;
      }
      $$->selection.relations.push_back(*$4);
      std::reverse($$->selection.relations.begin(), $$->selection.relations.end());

      // check relations' validity
      std::unordered_set<std::string> aliases;
        for (auto &rel : $$->selection.relations) {
            const auto& alias = rel.second;
            if (alias.empty()) continue;
            if (aliases.find(alias) != aliases.end()) {
            yyerror(&@$, sql_string, sql_result, scanner, "duplicate alias");
            }
            aliases.insert(alias);
        }

      if ($6 != nullptr) {
        $$->selection.join_list = (*$6);
        $$->selection.join_list.relations.push_back(*$4);
        std::reverse($$->selection.join_list.relations.begin(),$$->selection.join_list.relations.end());
        std::reverse($$->selection.join_list.conditions.begin(),$$->selection.join_list.conditions.end());
        $$->selection.relations=$$->selection.join_list.relations;
      }

      if ($7 != nullptr) {
        $$->selection.conditions.reset($7);
      }

      if ($8 != nullptr) {
        $$->selection.group_by = std::move(*$8);
        delete $8;
      }
      if ($9 != nullptr) {
        $$->selection.having.reset($9);
      }
      if ($10 != nullptr) {
        $$->selection.order_lists.swap(*$10);
        delete $10;
      }

      delete $4;
    }
    ;

order_by_list_all:
    /* empty */
    {
      $$ = nullptr;
    }
    | ORDER BY order_by_list
    {
      $$ = $3;
    }
    ;

order_by_list:
    order_by_
    {
      $$ = new std::vector<RelAttrSqlNode>;
      $$->emplace_back(*$1);
      delete $1;
    }
    | order_by_ COMMA order_by_list
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<RelAttrSqlNode>;
      }
      $$->emplace_back(*$1);
      delete $1;
    }
    ;

order_by_:
    ID
    {
      $$ = new RelAttrSqlNode;
      $$->attribute_name  = $1;
      $$->sort_way = "ASC";
      free($1);
    }
    | ID ASC
    {
      $$ = new RelAttrSqlNode;
      $$->attribute_name  = $1;
      $$->sort_way = "ASC";
      free($1);
    }
    | ID DESC
    {
      $$ = new RelAttrSqlNode;
      $$->attribute_name  = $1;
      $$->sort_way = "DESC";
      free($1);
    }
    | ID DOT ID
    {
      $$ = new RelAttrSqlNode;
      $$->relation_name  = $1;
      $$->attribute_name = $3;
      $$->sort_way = "ASC";
      free($1);
      free($3);
    }
    | ID DOT ID ASC
    {
      $$ = new RelAttrSqlNode;
      $$->relation_name  = $1;
      $$->attribute_name = $3;
      $$->sort_way = "ASC";
      free($1);
      free($3);
    }
    | ID DOT ID DESC
    {
      $$ = new RelAttrSqlNode;
      $$->relation_name  = $1;
      $$->attribute_name = $3;
      $$->sort_way = "DESC";
      free($1);
      free($3);
    }
    ;
calc_stmt:
    CALC expression_list
    {
      $$ = new ParsedSqlNode(SCF_CALC);
      std::reverse($2->begin(), $2->end());
      $$->calc.expressions.swap(*$2);
      delete $2;
    }
    | SELECT expression_list
    {
      $$ = new ParsedSqlNode(SCF_CALC);
      std::reverse($2->begin(), $2->end());
      $$->calc.expressions.swap(*$2);
      delete $2;
    }
    ;

expression_list:
    expression
    {
      $$ = new std::vector<Expression*>;
      $$->emplace_back($1);
    }
    | expression ID
    {
      $$ = new std::vector<Expression*>;
      $1->set_name($2); free($2);
      $$->emplace_back($1);
    }
    | expression AS ID
    {
      $$ = new std::vector<Expression*>;
      $1->set_name($3); free($3);
      $$->emplace_back($1);
    }
    | expression COMMA expression_list
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<Expression *>;
      }
      $$->emplace_back($1);
    }
    | expression ID COMMA expression_list
    {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new std::vector<Expression *>;
      }
      $1->set_name($2); free($2);
      $$->emplace_back($1);
    }
    | expression AS ID COMMA expression_list
    {
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new std::vector<Expression *>;
      }
      $1->set_name($3); free($3);
      $$->emplace_back($1);
    }
    ;
expression:
    expression '+' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::ADD, $1, $3, sql_string, &@$);
    }
    | expression '-' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::SUB, $1, $3, sql_string, &@$);
    }
    | expression '*' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::MUL, $1, $3, sql_string, &@$);
    }
    | expression '/' expression {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::DIV, $1, $3, sql_string, &@$);
    }
    | LBRACE expression RBRACE {
      $$ = $2;
      $$->set_name(token_name(sql_string, &@$));
    }
    | '-' expression %prec UMINUS {
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, $2, nullptr, sql_string, &@$);
    }
    | value {
      $$ = new ValueExpr(*$1);
      $$->set_name(token_name(sql_string, &@$));
      delete $1;
    }
    | rel_attr {
      $$ = new FieldExpr(*$1);
      // $$->set_name(token_name(sql_string, &@$));
      delete $1;
    }
    | LENGTH LBRACE value RBRACE {
      if (RC::SUCCESS != $3->apply_length_func()) {
        yyerror(&@$, sql_string, sql_result, scanner, "length function's parameter must be string");
      }
      $$ = new ValueExpr(*$3); delete $3;
    }
    | LENGTH LBRACE rel_attr RBRACE {
      let f = new FieldExpr(*$3);
      $$ = f;
      $$->set_name(token_name(sql_string, &@$));
      f->set_func(F_LENGTH);
      delete $3;
    }
    | ROUND LBRACE value RBRACE {
      if (RC::SUCCESS != $3->apply_round_func()) {
          yyerror(&@$, sql_string, sql_result, scanner, "round function's parameter must be float");
      }
      $$ = new ValueExpr(*$3); delete $3;
    }
    | ROUND LBRACE rel_attr RBRACE {
      let f = new FieldExpr(*$3);
      $$ = f;
      $$->set_name(token_name(sql_string, &@$));
      f->set_func(F_ROUND);
      delete $3;
    }
    | ROUND LBRACE value COMMA value RBRACE {
      if ($5->attr_type() != AttrType::INTS) {
        yyerror(&@$, sql_string, sql_result, scanner, "round function's second parameter must be int");
      }
      if (RC::SUCCESS != $3->apply_round_func($5->get_int())) {
        yyerror(&@$, sql_string, sql_result, scanner, "round function's first parameter must be float");
      }
      delete $5;
      $$ = new ValueExpr(*$3); delete $3;
    }
    | ROUND LBRACE rel_attr COMMA value RBRACE {
      let f = new FieldExpr(*$3);
      $$ = f;
      $$->set_name(token_name(sql_string, &@$));
      f->set_func(F_ROUND);
      if ($5->attr_type() != AttrType::INTS) {
        yyerror(&@$, sql_string, sql_result, scanner, "round function's second parameter must be int");
      }
      f->decimals = $5->get_int();
      delete $5;
      delete $3;
    }
    | DATE_FORMAT LBRACE value COMMA SSS RBRACE {
      if ($3->attr_type() != AttrType::DATES) {
        yyerror(&@$, sql_string, sql_result, scanner, "date_format function's first parameter must be date");
      }
      char *tmp = common::substr($5,1,strlen($5)-2);
      if (RC::SUCCESS != $3->apply_date_format_func(tmp)) {
        yyerror(&@$, sql_string, sql_result, scanner, "date_format function's second parameter must be string");
      }
      $$ = new ValueExpr(*$3); delete $3;
      free(tmp);
      free($5);
    }
    | DATE_FORMAT LBRACE rel_attr COMMA SSS RBRACE {
      let f = new FieldExpr(*$3);
      $$ = f;
      $$->set_name(token_name(sql_string, &@$));
      f->set_func(F_DATE_FORMAT);

      char *tmp = common::substr($5,1,strlen($5)-2);
      f->date_format = tmp;
      free(tmp);
      free($5);

      delete $3;
    }
    | aggr {
      $$ = new AggregateExpr(*$1, nullptr);
      $$->set_name(token_name(sql_string, &@$));
      delete $1;
    }
    | select_stmt {
      $$ = new SqlExpr($1);
      $$->set_name(token_name(sql_string, &@$));
    }
    ;

select_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | expression select_list {
      if ($2 != nullptr) {
        $$ = $2;
      } else {
        $$ = new ExprList;
      }
      $$->exprs.emplace_back($1);
    }
    | expression ID select_list {
        if (auto* t = dynamic_cast<FieldExpr*>($1)) {
            if (t->node().attribute_name == "*") {
                yyerror(&@$, sql_string, sql_result, scanner, "can not use * AS alias in select list");
            }
        }

      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new ExprList;
      }
      $1->set_name($2);
      free($2);
      $$->exprs.emplace_back($1);
    }
    | expression AS ID select_list {
        if (auto* t = dynamic_cast<FieldExpr*>($1)) {
            if (t->node().attribute_name == "*") {
                yyerror(&@$, sql_string, sql_result, scanner, "can not use * AS alias in select list");
            }
        }
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new ExprList;
      }
      $1->set_name($3);
      free($3);
      $$->exprs.emplace_back($1);
    }
    | COMMA expression select_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new ExprList;
      }
      $$->exprs.emplace_back($2);
    }
    | COMMA expression ID select_list {
        if (auto* t = dynamic_cast<FieldExpr*>($2)) {
            if (t->node().attribute_name == "*") {
                yyerror(&@$, sql_string, sql_result, scanner, "can not use * AS alias in select list");
            }
        }
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new ExprList;
      }
      $2->set_name($3);
      free($3);
      $$->exprs.emplace_back($2);
    }
    | COMMA expression AS ID select_list {
        if (auto* t = dynamic_cast<FieldExpr*>($2)) {
            if (t->node().attribute_name == "*") {
                yyerror(&@$, sql_string, sql_result, scanner, "can not use * AS alias in select list");
            }
        }
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new ExprList;
      }
      $2->set_name($4);
      free($4);
      $$->exprs.emplace_back($2);
    }
    ;

rel_attr:
    '*' {
      $$ = make_single_field("*");
    }
    | ID {
      $$ = make_single_field($1);
      free($1);
    }
    | ID DOT ID {
      $$ = make_single_table_field($1, $3);
      free($1);
      free($3);
    }
    | ID DOT '*' {
      $$ = make_single_table_field($1, "*");
      free($1);
    }
    ;

aggr:
    aggr_func LBRACE rel_attr RBRACE {
        if (get_display_name(*$3) == "*" && $1 == AggregateFunc::Count) {
            $$ = make_aggr(AggregateFunc::CountStar);
        } else {
            $$ = make_aggr($1);
            $$->attr = *$3;
        }
        delete $3;
    }
    ;

aggr_func:
    AVG { $$ = AggregateFunc::Avg; }
    | SUM { $$ = AggregateFunc::Sum; }
    | COUNT { $$ = AggregateFunc::Count; }
    | MAX { $$ = AggregateFunc::Max; }
    | MIN { $$ = AggregateFunc::Min; }
    ;

rel_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA table_name rel_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<NameWithAlias>;
      }

      $$->push_back(std::move(*$2));
      delete $2;
    }
    ;
inner_join_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | INNER JOIN table_name ON condition_tree inner_join_list {
      if ($6 != nullptr) {
        $$ = $6;
      } else {
        $$ = new JoinList;
      }
      $$->conditions.push_back($5);
      $$->relations.push_back(std::move(*$3));
      delete $3;
    }
update_list:
     /* empty */
    {
    $$ = nullptr;
    }
    | COMMA ID EQ expression update_list {
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new std::unordered_map<std::string, std::unique_ptr<Expression>>;

      }
      $$->insert(std::make_pair($2, std::unique_ptr<Expression>($4)));
      free($2);
    }

where:
    /* empty */
    {
      $$ = nullptr;
    }
    | WHERE condition_tree {
      $$ = $2;  
    }
    ;
having:
    /* empty */
    {
      $$ = nullptr;
    }
    | HAVING condition_tree {
      $$ = $2;
    }
    ;
group_by:
    /* empty */
    {
      $$ = nullptr;
    }
    | GROUP BY expression_list {
      $$ = new GroupBy;
      if ($3 != nullptr) {
          for (auto e : *$3) {
              $$->exprs.emplace_back(e);
          }
          delete $3;
      }
    }
    ;
condition_tree:
    /* empty */
    {
      $$ = nullptr;
    }
    | LBRACE condition_tree RBRACE {
      $$ = $2;
    }
    | condition {
      $$ = new ConditionTree;
      $$->node.reset($1);
    }
    | condition_tree AND condition_tree {
      $$ = new ConditionTree;
      $$->op = ConditionTree::AND_OP;
      $$->left.reset($1);
      $$->right.reset($3);
    }
    | condition_tree OR condition_tree {
      $$ = new ConditionTree;
      $$->op = ConditionTree::OR_OP;
      $$->left.reset($1);
      $$->right.reset($3);
    }
    ;
condition:
    expression comp_op expression {
        $$ = new ConditionSqlNode;
        $$->left = std::unique_ptr<Expression>($1);
        $$->right = std::unique_ptr<Expression>($3);
        $$->comp = $2;
    }
    | EXISTS LBRACE select_stmt RBRACE {
        $$ = new ConditionSqlNode;
        $$->left.reset(new SqlExpr($3));
        $$->comp = EXISTS_OP;
    }
    | NOT EXISTS LBRACE select_stmt RBRACE {
        $$ = new ConditionSqlNode;
        $$->left.reset(new SqlExpr($4));
        $$->comp = NOT_EXISTS_OP;
    }
    | expression IN LBRACE expression_list RBRACE {
        $$ = new ConditionSqlNode;
        $$->left = std::unique_ptr<Expression>($1);
        $$->right = std::make_unique<ExpressionsExpr>(std::move(*$4));
        delete $4;
        $$->comp = IN_OP;
    }
    | expression NOT IN LBRACE expression_list RBRACE {
        $$ = new ConditionSqlNode;
        $$->left = std::unique_ptr<Expression>($1);
        $$->right = std::make_unique<ExpressionsExpr>(std::move(*$5));
        delete $5;
        $$->comp = NOT_IN_OP;
    }
    ;

comp_op:
      EQ { $$ = EQUAL_TO; }
    | LT { $$ = LESS_THAN; }
    | GT { $$ = GREAT_THAN; }
    | LE { $$ = LESS_EQUAL; }
    | GE { $$ = GREAT_EQUAL; }
    | NE { $$ = NOT_EQUAL; }
    | IS { $$ = IS_OP;}
    | IS NOT{ $$ = IS_NOT_OP;}
    | LIKE { $$ = LIKE_OP; }
    | NOT LIKE { $$ = NOT_LIKE_OP; }
    ;

load_data_stmt:
    LOAD DATA INFILE SSS INTO TABLE ID 
    {
      char *tmp_file_name = common::substr($4, 1, strlen($4) - 2);
      
      $$ = new ParsedSqlNode(SCF_LOAD_DATA);
      $$->load_data.relation_name = $7;
      $$->load_data.file_name = tmp_file_name;
      free($7);
      free(tmp_file_name);
    }
    ;

explain_stmt:
    EXPLAIN command_wrapper
    {
      $$ = new ParsedSqlNode(SCF_EXPLAIN);
      $$->explain.sql_node = std::unique_ptr<ParsedSqlNode>($2);
    }
    ;

set_variable_stmt:
    SET ID EQ value
    {
      $$ = new ParsedSqlNode(SCF_SET_VARIABLE);
      $$->set_variable.name  = $2;
      $$->set_variable.value = *$4;
      free($2);
      delete $4;
    }
    ;

opt_semicolon: /*empty*/
    | SEMICOLON
    ;
%%
//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, ParsedSqlResult *sql_result) {
  yyscan_t scanner;
  yylex_init(&scanner);
  scan_string(s, scanner);
  int result = yyparse(s, sql_result, scanner);
  yylex_destroy(scanner);
  return result;
}
