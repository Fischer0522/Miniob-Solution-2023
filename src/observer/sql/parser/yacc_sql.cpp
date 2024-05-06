/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "yacc_sql.y"


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


#line 117 "yacc_sql.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "yacc_sql.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEMICOLON = 3,                  /* SEMICOLON  */
  YYSYMBOL_CREATE = 4,                     /* CREATE  */
  YYSYMBOL_DROP = 5,                       /* DROP  */
  YYSYMBOL_TABLE = 6,                      /* TABLE  */
  YYSYMBOL_VIEW = 7,                       /* VIEW  */
  YYSYMBOL_TABLES = 8,                     /* TABLES  */
  YYSYMBOL_INDEX = 9,                      /* INDEX  */
  YYSYMBOL_CALC = 10,                      /* CALC  */
  YYSYMBOL_SELECT = 11,                    /* SELECT  */
  YYSYMBOL_DESC = 12,                      /* DESC  */
  YYSYMBOL_SHOW = 13,                      /* SHOW  */
  YYSYMBOL_SYNC = 14,                      /* SYNC  */
  YYSYMBOL_INSERT = 15,                    /* INSERT  */
  YYSYMBOL_DELETE = 16,                    /* DELETE  */
  YYSYMBOL_UPDATE = 17,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 18,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 19,                    /* RBRACE  */
  YYSYMBOL_UNIQUE = 20,                    /* UNIQUE  */
  YYSYMBOL_ORDER = 21,                     /* ORDER  */
  YYSYMBOL_BY = 22,                        /* BY  */
  YYSYMBOL_ASC = 23,                       /* ASC  */
  YYSYMBOL_COMMA = 24,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 25,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 26,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 27,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 28,                     /* INT_T  */
  YYSYMBOL_STRING_T = 29,                  /* STRING_T  */
  YYSYMBOL_FLOAT_T = 30,                   /* FLOAT_T  */
  YYSYMBOL_DATE_T = 31,                    /* DATE_T  */
  YYSYMBOL_NULL_T = 32,                    /* NULL_T  */
  YYSYMBOL_TEXT_T = 33,                    /* TEXT_T  */
  YYSYMBOL_HELP = 34,                      /* HELP  */
  YYSYMBOL_EXIT = 35,                      /* EXIT  */
  YYSYMBOL_DOT = 36,                       /* DOT  */
  YYSYMBOL_INTO = 37,                      /* INTO  */
  YYSYMBOL_VALUES = 38,                    /* VALUES  */
  YYSYMBOL_FROM = 39,                      /* FROM  */
  YYSYMBOL_WHERE = 40,                     /* WHERE  */
  YYSYMBOL_AND = 41,                       /* AND  */
  YYSYMBOL_OR = 42,                        /* OR  */
  YYSYMBOL_SET = 43,                       /* SET  */
  YYSYMBOL_ON = 44,                        /* ON  */
  YYSYMBOL_LOAD = 45,                      /* LOAD  */
  YYSYMBOL_DATA = 46,                      /* DATA  */
  YYSYMBOL_INFILE = 47,                    /* INFILE  */
  YYSYMBOL_EXPLAIN = 48,                   /* EXPLAIN  */
  YYSYMBOL_IS = 49,                        /* IS  */
  YYSYMBOL_EQ = 50,                        /* EQ  */
  YYSYMBOL_LT = 51,                        /* LT  */
  YYSYMBOL_GT = 52,                        /* GT  */
  YYSYMBOL_LE = 53,                        /* LE  */
  YYSYMBOL_GE = 54,                        /* GE  */
  YYSYMBOL_NE = 55,                        /* NE  */
  YYSYMBOL_SUM = 56,                       /* SUM  */
  YYSYMBOL_MAX = 57,                       /* MAX  */
  YYSYMBOL_MIN = 58,                       /* MIN  */
  YYSYMBOL_COUNT = 59,                     /* COUNT  */
  YYSYMBOL_AVG = 60,                       /* AVG  */
  YYSYMBOL_INNER = 61,                     /* INNER  */
  YYSYMBOL_JOIN = 62,                      /* JOIN  */
  YYSYMBOL_GROUP = 63,                     /* GROUP  */
  YYSYMBOL_IN = 64,                        /* IN  */
  YYSYMBOL_NOT = 65,                       /* NOT  */
  YYSYMBOL_EXISTS = 66,                    /* EXISTS  */
  YYSYMBOL_AS = 67,                        /* AS  */
  YYSYMBOL_LENGTH = 68,                    /* LENGTH  */
  YYSYMBOL_ROUND = 69,                     /* ROUND  */
  YYSYMBOL_DATE_FORMAT = 70,               /* DATE_FORMAT  */
  YYSYMBOL_LIKE = 71,                      /* LIKE  */
  YYSYMBOL_HAVING = 72,                    /* HAVING  */
  YYSYMBOL_NUMBER = 73,                    /* NUMBER  */
  YYSYMBOL_FLOAT = 74,                     /* FLOAT  */
  YYSYMBOL__id = 75,                       /* _id  */
  YYSYMBOL_DATE_STR = 76,                  /* DATE_STR  */
  YYSYMBOL_SSS = 77,                       /* SSS  */
  YYSYMBOL_78_ = 78,                       /* '+'  */
  YYSYMBOL_79_ = 79,                       /* '-'  */
  YYSYMBOL_80_ = 80,                       /* '*'  */
  YYSYMBOL_81_ = 81,                       /* '/'  */
  YYSYMBOL_UMINUS = 82,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 83,                  /* $accept  */
  YYSYMBOL_commands = 84,                  /* commands  */
  YYSYMBOL_command_wrapper = 85,           /* command_wrapper  */
  YYSYMBOL_exit_stmt = 86,                 /* exit_stmt  */
  YYSYMBOL_help_stmt = 87,                 /* help_stmt  */
  YYSYMBOL_sync_stmt = 88,                 /* sync_stmt  */
  YYSYMBOL_begin_stmt = 89,                /* begin_stmt  */
  YYSYMBOL_commit_stmt = 90,               /* commit_stmt  */
  YYSYMBOL_rollback_stmt = 91,             /* rollback_stmt  */
  YYSYMBOL_ID = 92,                        /* ID  */
  YYSYMBOL_drop_table_stmt = 93,           /* drop_table_stmt  */
  YYSYMBOL_show_tables_stmt = 94,          /* show_tables_stmt  */
  YYSYMBOL_desc_table_stmt = 95,           /* desc_table_stmt  */
  YYSYMBOL_create_index_stmt = 96,         /* create_index_stmt  */
  YYSYMBOL_unique_flag = 97,               /* unique_flag  */
  YYSYMBOL_index_list = 98,                /* index_list  */
  YYSYMBOL_drop_index_stmt = 99,           /* drop_index_stmt  */
  YYSYMBOL_table_type = 100,               /* table_type  */
  YYSYMBOL_create_table_stmt = 101,        /* create_table_stmt  */
  YYSYMBOL_attr_def_list = 102,            /* attr_def_list  */
  YYSYMBOL_attr_def = 103,                 /* attr_def  */
  YYSYMBOL_number = 104,                   /* number  */
  YYSYMBOL_type = 105,                     /* type  */
  YYSYMBOL_insert_stmt = 106,              /* insert_stmt  */
  YYSYMBOL_value_list = 107,               /* value_list  */
  YYSYMBOL_value = 108,                    /* value  */
  YYSYMBOL_delete_stmt = 109,              /* delete_stmt  */
  YYSYMBOL_update_stmt = 110,              /* update_stmt  */
  YYSYMBOL_table_name = 111,               /* table_name  */
  YYSYMBOL_select_stmt = 112,              /* select_stmt  */
  YYSYMBOL_order_by_list_all = 113,        /* order_by_list_all  */
  YYSYMBOL_order_by_list = 114,            /* order_by_list  */
  YYSYMBOL_order_by_ = 115,                /* order_by_  */
  YYSYMBOL_calc_stmt = 116,                /* calc_stmt  */
  YYSYMBOL_expression_list = 117,          /* expression_list  */
  YYSYMBOL_expression = 118,               /* expression  */
  YYSYMBOL_select_list = 119,              /* select_list  */
  YYSYMBOL_rel_attr = 120,                 /* rel_attr  */
  YYSYMBOL_aggr = 121,                     /* aggr  */
  YYSYMBOL_aggr_func = 122,                /* aggr_func  */
  YYSYMBOL_rel_list = 123,                 /* rel_list  */
  YYSYMBOL_inner_join_list = 124,          /* inner_join_list  */
  YYSYMBOL_update_list = 125,              /* update_list  */
  YYSYMBOL_where = 126,                    /* where  */
  YYSYMBOL_having = 127,                   /* having  */
  YYSYMBOL_group_by = 128,                 /* group_by  */
  YYSYMBOL_condition_tree = 129,           /* condition_tree  */
  YYSYMBOL_condition = 130,                /* condition  */
  YYSYMBOL_comp_op = 131,                  /* comp_op  */
  YYSYMBOL_load_data_stmt = 132,           /* load_data_stmt  */
  YYSYMBOL_explain_stmt = 133,             /* explain_stmt  */
  YYSYMBOL_set_variable_stmt = 134,        /* set_variable_stmt  */
  YYSYMBOL_opt_semicolon = 135             /* opt_semicolon  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  88
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1145

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  83
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  53
/* YYNRULES -- Number of rules.  */
#define YYNRULES  167
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  328

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   333


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,    80,    78,     2,    79,     2,    81,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    82
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   230,   230,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   250,   251,   252,   253,   254,
     255,   256,   257,   261,   267,   272,   278,   284,   290,   296,
     299,   302,   308,   315,   321,   329,   347,   350,   357,   362,
     374,   385,   388,   394,   403,   422,   441,   464,   467,   480,
     489,   498,   507,   514,   526,   538,   552,   555,   556,   557,
     558,   559,   562,   578,   581,   592,   597,   602,   607,   612,
     621,   626,   635,   646,   663,   667,   672,   679,   735,   738,
     745,   751,   764,   771,   778,   785,   794,   803,   814,   821,
     831,   836,   842,   848,   857,   867,   879,   882,   885,   888,
     891,   895,   898,   903,   908,   914,   921,   927,   934,   944,
     956,   968,   981,   986,   994,   997,  1005,  1021,  1036,  1044,
    1059,  1077,  1080,  1084,  1089,  1096,  1108,  1109,  1110,  1111,
    1112,  1117,  1120,  1133,  1136,  1148,  1151,  1164,  1167,  1173,
    1176,  1182,  1185,  1197,  1200,  1203,  1207,  1213,  1221,  1227,
    1232,  1237,  1244,  1254,  1255,  1256,  1257,  1258,  1259,  1260,
    1261,  1262,  1263,  1267,  1280,  1288,  1298,  1299
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SEMICOLON", "CREATE",
  "DROP", "TABLE", "VIEW", "TABLES", "INDEX", "CALC", "SELECT", "DESC",
  "SHOW", "SYNC", "INSERT", "DELETE", "UPDATE", "LBRACE", "RBRACE",
  "UNIQUE", "ORDER", "BY", "ASC", "COMMA", "TRX_BEGIN", "TRX_COMMIT",
  "TRX_ROLLBACK", "INT_T", "STRING_T", "FLOAT_T", "DATE_T", "NULL_T",
  "TEXT_T", "HELP", "EXIT", "DOT", "INTO", "VALUES", "FROM", "WHERE",
  "AND", "OR", "SET", "ON", "LOAD", "DATA", "INFILE", "EXPLAIN", "IS",
  "EQ", "LT", "GT", "LE", "GE", "NE", "SUM", "MAX", "MIN", "COUNT", "AVG",
  "INNER", "JOIN", "GROUP", "IN", "NOT", "EXISTS", "AS", "LENGTH", "ROUND",
  "DATE_FORMAT", "LIKE", "HAVING", "NUMBER", "FLOAT", "_id", "DATE_STR",
  "SSS", "'+'", "'-'", "'*'", "'/'", "UMINUS", "$accept", "commands",
  "command_wrapper", "exit_stmt", "help_stmt", "sync_stmt", "begin_stmt",
  "commit_stmt", "rollback_stmt", "ID", "drop_table_stmt",
  "show_tables_stmt", "desc_table_stmt", "create_index_stmt",
  "unique_flag", "index_list", "drop_index_stmt", "table_type",
  "create_table_stmt", "attr_def_list", "attr_def", "number", "type",
  "insert_stmt", "value_list", "value", "delete_stmt", "update_stmt",
  "table_name", "select_stmt", "order_by_list_all", "order_by_list",
  "order_by_", "calc_stmt", "expression_list", "expression", "select_list",
  "rel_attr", "aggr", "aggr_func", "rel_list", "inner_join_list",
  "update_list", "where", "having", "group_by", "condition_tree",
  "condition", "comp_op", "load_data_stmt", "explain_stmt",
  "set_variable_stmt", "opt_semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-205)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-115)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    1097,    26,    70,   895,   863,   198,    12,  -205,   -21,   -15,
     198,  -205,  -205,  -205,  -205,  -205,   198,     2,  1097,    50,
      55,  -205,  -205,  -205,  -205,  -205,  -205,  -205,  -205,  -205,
    -205,  -205,  -205,  -205,  -205,  -205,  -205,  -205,  -205,  -205,
    -205,  -205,  -205,  -205,    71,   198,   198,   198,   863,   895,
    -205,  -205,  -205,  -205,  -205,  -205,  -205,   100,   102,   112,
    -205,  -205,  -205,  -205,  -205,   948,  -205,    59,  -205,  -205,
    -205,   473,  -205,  -205,   114,   895,  -205,   433,    58,  -205,
    -205,  -205,   198,   198,    91,    95,    99,  -205,  -205,  -205,
    -205,   198,   -12,  -205,    61,   575,   -11,  1020,  1020,  1020,
    -205,  -205,  -205,    79,   895,   198,   895,   895,   895,   895,
     125,   137,   646,   895,   198,   948,   980,   340,  -205,   198,
     113,   115,   198,   -20,    76,   122,   198,   145,   198,   198,
     717,  -205,   -38,   139,   148,    77,   107,   144,   146,  -205,
    -205,  -205,   147,     8,     8,  -205,  -205,   895,   150,   198,
     717,  -205,   504,   771,    23,   895,  -205,   167,   149,   154,
     803,  -205,   124,  -205,   138,   198,    82,   152,  -205,  -205,
     863,  -205,  -205,  -205,  -205,  -205,   -20,  -205,   -20,   101,
     103,   895,  -205,  -205,   863,  -205,   198,   340,   895,  -205,
     198,  -205,   198,   116,   -20,   803,   118,   161,   559,    87,
    -205,   895,   176,   168,  -205,  -205,  -205,  -205,  -205,    -1,
     198,   173,   182,   183,   184,   185,  -205,  -205,   771,  -205,
     149,   156,   115,   187,  1010,    19,   188,   145,   142,  -205,
    -205,  -205,  -205,  -205,  -205,   191,   -42,  -205,   895,   803,
     803,    13,   198,   198,   157,  -205,   178,   152,     4,  -205,
    -205,  -205,  -205,  -205,   198,   158,   -20,   200,  -205,   145,
     210,  -205,   895,   214,  -205,    62,  -205,   192,   198,   115,
    -205,   216,   222,  -205,   228,  -205,  -205,   145,  -205,   206,
     230,   181,   187,  -205,   240,  -205,   241,   895,   212,  -205,
     198,  -205,   -22,  -205,   803,   895,   803,   243,  -205,  -205,
    -205,   246,   895,  -205,  -205,   235,   -16,  -205,    87,   248,
    -205,  -205,    13,  -205,  -205,   198,  -205,    11,  -205,   244,
    -205,  -205,   198,   198,     7,  -205,  -205,  -205
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    36,     0,     0,   114,     0,     0,    25,     0,     0,
       0,    26,    27,    28,    24,    23,     0,     0,     0,     0,
     166,    22,    21,    14,    15,    16,    17,     9,    10,    11,
      12,    13,     8,     5,     7,     6,     4,     3,    18,    19,
      20,    42,    41,    37,     0,     0,     0,     0,   114,     0,
      70,    30,   127,   129,   130,   128,   126,     0,     0,     0,
      65,    66,    29,    69,    71,     0,   121,   122,   102,   113,
      88,    90,   103,   112,    31,     0,    89,    90,     0,    34,
      31,    33,     0,     0,     0,     0,     0,   164,     1,   167,
       2,     0,     0,    32,     0,   114,     0,     0,     0,     0,
      65,    66,   101,     0,     0,     0,     0,     0,     0,     0,
      91,     0,   114,     0,     0,     0,   121,   122,   115,     0,
       0,   137,     0,     0,     0,     0,     0,     0,     0,     0,
     122,   100,     0,     0,     0,     0,     0,     0,     0,   124,
     123,    93,    92,    96,    97,    98,    99,     0,     0,     0,
     122,   118,    90,    92,    97,     0,   116,    74,   131,     0,
     143,    72,     0,   165,     0,     0,    52,    47,    43,    40,
     114,    67,    68,   104,   105,   106,     0,   107,     0,     0,
       0,     0,    94,   125,   114,   119,     0,   122,     0,   117,
       0,    76,     0,   133,     0,   143,     0,     0,     0,   138,
     145,     0,     0,     0,    57,    58,    59,    60,    61,    53,
       0,     0,     0,     0,     0,     0,    95,   120,    92,    75,
     131,     0,   137,    63,     0,     0,     0,     0,   159,   153,
     154,   155,   156,   157,   158,     0,     0,   161,     0,   143,
     143,   135,     0,     0,     0,    54,     0,    47,    46,   108,
     109,   110,   111,   132,     0,   141,     0,     0,   144,     0,
       0,   160,     0,     0,   162,   148,   146,   147,     0,   137,
     163,    38,     0,    56,     0,    55,    48,     0,    45,     0,
       0,   139,    63,    62,     0,   149,     0,     0,     0,    73,
       0,    35,    49,    44,   143,     0,   143,    78,    64,   150,
     151,     0,     0,    39,    50,     0,   133,   142,   140,     0,
      77,   152,   135,    51,   134,     0,   136,    82,    79,    80,
      84,    83,     0,     0,    85,    81,    87,    86
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -205,  -205,   253,  -205,  -205,  -205,  -205,  -205,  -205,    -5,
    -205,  -205,  -205,  -205,  -205,   -14,  -205,  -205,  -205,    28,
      67,  -205,  -205,  -205,    -4,   -95,  -205,  -205,  -191,    21,
    -205,   -44,  -205,  -205,    10,   186,   -68,   -24,  -205,   117,
      63,   -25,   -28,  -204,  -205,  -205,  -188,  -205,  -205,  -205,
    -205,  -205,  -205
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    19,    20,    21,    22,    23,    24,    25,    26,    67,
      27,    28,    29,    30,    44,   272,    31,    45,    32,   211,
     167,   274,   209,    33,   257,    68,    34,    35,   158,    69,
     310,   318,   319,    37,   141,    95,    78,    72,    73,    74,
     193,   222,   269,   161,   297,   281,   199,   200,   238,    38,
      39,    40,    90
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      79,   220,   133,   135,   137,    84,   126,   225,   131,   118,
     304,    85,    50,    70,    76,    48,    82,   244,   255,   326,
      81,    36,   263,   320,    83,   239,   240,   118,   163,   264,
     327,   245,    41,    42,   321,   171,   172,   268,   258,    36,
      92,    93,    94,   305,   151,   221,    43,   322,    86,   156,
      88,   266,   267,    60,    61,   127,    63,    64,    89,   132,
     239,   240,   156,   279,   246,   289,   110,   106,   107,   108,
     109,   277,   117,   134,   136,   138,    46,   120,   121,    47,
      91,   212,   185,   213,   151,   189,   125,   148,   108,   109,
     130,   106,   107,   108,   109,   103,   175,   119,   140,   223,
     142,   176,   189,  -101,  -101,   128,   306,   150,   308,   153,
     204,   205,   206,   207,   157,   208,   217,   162,    97,   185,
      98,   166,    80,   169,   170,    51,   177,    80,   239,   240,
      99,   178,   111,    80,   122,    52,    53,    54,    55,    56,
     106,   107,   108,   109,   184,   123,   124,   187,   168,   147,
     217,   159,   191,   164,    62,   160,    48,   182,   173,   139,
     203,   282,    80,    80,    80,   182,   165,   174,   179,   183,
     180,   181,   194,   192,   201,   202,   210,   221,   214,   227,
     215,   218,   242,    51,   226,   219,   243,   157,    80,    71,
      77,   216,   248,    52,    53,    54,    55,    56,   216,    80,
      80,   249,   250,   251,   252,   166,   259,   261,    80,   262,
     275,   256,    62,    51,    80,    80,    80,    66,   254,   283,
      80,   280,    80,    52,    53,    54,    55,    56,    80,   285,
     273,    80,   287,   239,   190,    96,    80,   270,   271,    80,
     290,   291,    62,    80,    51,    80,    80,   292,   260,   157,
     294,   102,   295,   296,    52,    53,    54,    55,    56,   299,
     300,   112,   302,   288,   309,   311,    80,   313,   323,   278,
     315,    87,   286,    62,    80,   276,   303,   247,   298,   325,
     284,   314,    80,   253,   316,   271,     0,     0,     0,     0,
      71,     0,   143,   144,   145,   146,     0,   301,   293,   152,
       0,   154,   145,    80,     0,   307,     0,    80,     0,    80,
     317,     0,     0,     0,     0,     0,     0,   324,   317,     0,
       0,     0,     0,     0,     0,     0,     0,    80,     0,     0,
       0,     0,     0,    71,     0,     0,     0,     0,     0,     0,
     -91,   152,     0,   -91,     0,     0,   198,     0,     0,     0,
       0,    48,     0,     0,     0,     0,     0,     0,    49,    80,
      80,     0,     0,     0,   155,     0,     0,    71,     0,     0,
       0,    80,    50,     0,   152,     0,   103,     0,     0,  -114,
       0,   224,     0,     0,     0,    80,    51,   241,     0,     0,
       0,     0,     0,     0,     0,     0,    52,    53,    54,    55,
      56,     0,     0,     0,     0,     0,     0,    80,    57,    58,
      59,     0,     0,    60,    61,    62,    63,    64,     0,    65,
      66,     0,     0,     0,   265,   198,   198,     0,     0,     0,
       0,     0,    80,     0,     0,     0,     0,     0,     0,    80,
      80,     0,     0,     0,    48,     0,     0,     0,    71,     0,
       0,    49,     0,     0,     0,     0,     0,   113,     0,     0,
       0,     0,     0,     0,     0,    50,     0,     0,     0,     0,
       0,     0,  -114,    71,     0,     0,     0,     0,     0,    51,
     198,    71,   198,     0,     0,     0,     0,     0,   312,    52,
      53,    54,    55,    56,     0,     0,     0,   104,     0,     0,
     114,    57,    58,    59,     0,     0,    60,    61,    62,    63,
      64,   106,   115,   116,   109,    48,     0,     0,     0,    51,
       0,     0,    49,     0,     0,     0,     0,     0,   113,    52,
      53,    54,    55,    56,     0,     0,    50,     0,     0,     0,
     105,     0,     0,  -114,     0,     0,     0,     0,    62,     0,
      51,   106,   107,   108,   109,     0,     0,     0,     0,     0,
      52,    53,    54,    55,    56,     0,     0,     0,     0,     0,
       0,   186,    57,    58,    59,     0,     0,    60,    61,    62,
      63,    64,   106,   115,   116,   109,    48,     0,     0,     0,
       0,     0,     0,    49,     0,     0,     0,     0,     0,    75,
       0,     0,     0,     0,     0,     0,     0,    50,   228,   229,
     230,   231,   232,   233,   234,     0,     0,     0,     0,     0,
       0,    51,     0,   235,   236,     0,     0,     0,     0,     0,
     237,    52,    53,    54,    55,    56,     0,   106,   107,   108,
     109,     0,   129,    57,    58,    59,     0,     0,    60,    61,
      62,    63,    64,   106,   115,   116,   109,    48,     0,     0,
       0,     0,     0,     0,    49,     0,     0,     0,     0,     0,
      75,     0,     0,     0,     0,     0,     0,     0,    50,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    51,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    52,    53,    54,    55,    56,     0,     0,     0,
       0,     0,     0,   149,    57,    58,    59,     0,     0,    60,
      61,    62,    63,    64,   106,   115,   116,   109,    48,     0,
       0,     0,     0,     0,     0,    49,     0,     0,     0,     0,
       0,    75,     0,     0,     0,     0,     0,     0,     0,    50,
       0,     0,     0,   103,     0,     0,  -114,     0,     0,     0,
       0,     0,     0,    51,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    52,    53,    54,    55,    56,     0,     0,
       0,     0,    48,     0,     0,    57,    58,    59,     0,    49,
      60,    61,    62,    63,    64,   188,    65,    66,     0,     0,
       0,     0,     0,    50,     0,     0,     0,     0,     0,     0,
    -114,     0,     0,     0,    48,     0,     0,    51,     0,     0,
       0,   195,     0,     0,     0,     0,     0,    52,    53,    54,
      55,    56,     0,     0,     0,    50,     0,     0,     0,    57,
      58,    59,     0,     0,    60,    61,    62,    63,    64,    51,
      65,    66,     0,     0,     0,     0,     0,     0,     0,    52,
      53,    54,    55,    56,     0,     0,     0,     0,   196,   197,
       0,    57,    58,    59,    48,     0,    60,    61,    62,    63,
      64,    49,    65,    66,     0,     0,     0,    75,     0,     0,
       0,     0,     0,     0,     0,    50,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    48,     0,     0,    51,
       0,     0,     0,    49,     0,     0,     0,     0,     0,    52,
      53,    54,    55,    56,     0,     0,     0,    50,     0,     0,
       0,    57,    58,    59,     0,     0,    60,    61,    62,    63,
      64,    51,    65,    66,     0,     0,     0,     0,     0,     0,
       0,    52,    53,    54,    55,    56,     0,     0,     0,    48,
       0,     0,     0,    57,    58,    59,    49,     0,    60,    61,
      62,    63,    64,     0,    65,    66,     0,     0,     0,     0,
      50,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    48,     0,     0,    51,     0,     0,     0,    49,     0,
       0,     0,     0,     0,    52,    53,    54,    55,    56,     0,
       0,     0,    50,     0,     0,     0,    57,    58,    59,     0,
       0,   100,   101,    62,    63,    64,    51,    65,    66,   131,
       0,     0,     0,     0,     0,     0,    52,    53,    54,    55,
      56,     0,     0,     0,     0,     0,     0,     0,    57,    58,
      59,     0,    50,    60,    61,    62,    63,    64,     0,   228,
     229,   230,   231,   232,   233,   234,    51,     0,     0,     0,
       0,     0,     0,     0,   235,   236,    52,    53,    54,    55,
      56,   237,     0,     0,     0,     0,     0,     0,   106,   107,
     108,   109,     0,    60,    61,    62,    63,    64,     0,   132,
      66,     1,     2,     0,     0,     0,     0,     3,     4,     5,
       6,     7,     8,     9,    10,     0,     0,     0,     0,     0,
       0,     0,    11,    12,    13,     0,     0,     0,     0,     0,
       0,    14,    15,     0,     0,     0,     0,     0,     0,     0,
      16,     0,    17,     0,     0,    18
};

static const yytype_int16 yycheck[] =
{
       5,   192,    97,    98,    99,    10,    18,   195,    19,    77,
      32,    16,    32,     3,     4,    11,    37,    18,   222,    12,
       8,     0,    64,    12,    39,    41,    42,    95,   123,    71,
      23,    32,     6,     7,    23,    73,    74,    24,    19,    18,
      45,    46,    47,    65,   112,    61,    20,    36,    46,   117,
       0,   239,   240,    73,    74,    67,    76,    77,     3,    79,
      41,    42,   130,   254,    65,   269,    71,    78,    79,    80,
      81,    67,    77,    97,    98,    99,     6,    82,    83,     9,
       9,   176,   150,   178,   152,   153,    91,   111,    80,    81,
      95,    78,    79,    80,    81,    36,    19,    39,   103,   194,
     105,    24,   170,    80,    81,    44,   294,   112,   296,   114,
      28,    29,    30,    31,   119,    33,   184,   122,    18,   187,
      18,   126,     5,   128,   129,    46,    19,    10,    41,    42,
      18,    24,    18,    16,    43,    56,    57,    58,    59,    60,
      78,    79,    80,    81,   149,    50,    47,   152,   127,    24,
     218,    38,   157,    77,    75,    40,    11,   147,    19,    80,
     165,   256,    45,    46,    47,   155,    44,    19,    24,    19,
      24,    24,    18,    24,    50,    37,    24,    61,    77,    18,
      77,   186,     6,    46,    66,   190,    18,   192,    71,     3,
       4,   181,    19,    56,    57,    58,    59,    60,   188,    82,
      83,    19,    19,    19,    19,   210,    18,    65,    91,    18,
      32,    24,    75,    46,    97,    98,    99,    80,    62,    19,
     103,    63,   105,    56,    57,    58,    59,    60,   111,    19,
      73,   114,    18,    41,    67,    49,   119,   242,   243,   122,
      24,    19,    75,   126,    46,   128,   129,    19,   227,   254,
      44,    65,    22,    72,    56,    57,    58,    59,    60,    19,
      19,    75,    50,   268,    21,    19,   149,    32,    24,   248,
      22,    18,   262,    75,   157,   247,   290,   210,   282,   323,
     259,   306,   165,   220,   312,   290,    -1,    -1,    -1,    -1,
     104,    -1,   106,   107,   108,   109,    -1,   287,   277,   113,
      -1,   115,   116,   186,    -1,   295,    -1,   190,    -1,   192,
     315,    -1,    -1,    -1,    -1,    -1,    -1,   322,   323,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   210,    -1,    -1,
      -1,    -1,    -1,   147,    -1,    -1,    -1,    -1,    -1,    -1,
       0,   155,    -1,     3,    -1,    -1,   160,    -1,    -1,    -1,
      -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,    18,   242,
     243,    -1,    -1,    -1,    24,    -1,    -1,   181,    -1,    -1,
      -1,   254,    32,    -1,   188,    -1,    36,    -1,    -1,    39,
      -1,   195,    -1,    -1,    -1,   268,    46,   201,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    56,    57,    58,    59,
      60,    -1,    -1,    -1,    -1,    -1,    -1,   290,    68,    69,
      70,    -1,    -1,    73,    74,    75,    76,    77,    -1,    79,
      80,    -1,    -1,    -1,   238,   239,   240,    -1,    -1,    -1,
      -1,    -1,   315,    -1,    -1,    -1,    -1,    -1,    -1,   322,
     323,    -1,    -1,    -1,    11,    -1,    -1,    -1,   262,    -1,
      -1,    18,    -1,    -1,    -1,    -1,    -1,    24,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    32,    -1,    -1,    -1,    -1,
      -1,    -1,    39,   287,    -1,    -1,    -1,    -1,    -1,    46,
     294,   295,   296,    -1,    -1,    -1,    -1,    -1,   302,    56,
      57,    58,    59,    60,    -1,    -1,    -1,    24,    -1,    -1,
      67,    68,    69,    70,    -1,    -1,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    11,    -1,    -1,    -1,    46,
      -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,    24,    56,
      57,    58,    59,    60,    -1,    -1,    32,    -1,    -1,    -1,
      67,    -1,    -1,    39,    -1,    -1,    -1,    -1,    75,    -1,
      46,    78,    79,    80,    81,    -1,    -1,    -1,    -1,    -1,
      56,    57,    58,    59,    60,    -1,    -1,    -1,    -1,    -1,
      -1,    67,    68,    69,    70,    -1,    -1,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    11,    -1,    -1,    -1,
      -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,    24,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    -1,    64,    65,    -1,    -1,    -1,    -1,    -1,
      71,    56,    57,    58,    59,    60,    -1,    78,    79,    80,
      81,    -1,    67,    68,    69,    70,    -1,    -1,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    11,    -1,    -1,
      -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,
      24,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    56,    57,    58,    59,    60,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    68,    69,    70,    -1,    -1,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    11,    -1,
      -1,    -1,    -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,
      -1,    24,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,
      -1,    -1,    -1,    36,    -1,    -1,    39,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    56,    57,    58,    59,    60,    -1,    -1,
      -1,    -1,    11,    -1,    -1,    68,    69,    70,    -1,    18,
      73,    74,    75,    76,    77,    24,    79,    80,    -1,    -1,
      -1,    -1,    -1,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      39,    -1,    -1,    -1,    11,    -1,    -1,    46,    -1,    -1,
      -1,    18,    -1,    -1,    -1,    -1,    -1,    56,    57,    58,
      59,    60,    -1,    -1,    -1,    32,    -1,    -1,    -1,    68,
      69,    70,    -1,    -1,    73,    74,    75,    76,    77,    46,
      79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    56,
      57,    58,    59,    60,    -1,    -1,    -1,    -1,    65,    66,
      -1,    68,    69,    70,    11,    -1,    73,    74,    75,    76,
      77,    18,    79,    80,    -1,    -1,    -1,    24,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    32,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    11,    -1,    -1,    46,
      -1,    -1,    -1,    18,    -1,    -1,    -1,    -1,    -1,    56,
      57,    58,    59,    60,    -1,    -1,    -1,    32,    -1,    -1,
      -1,    68,    69,    70,    -1,    -1,    73,    74,    75,    76,
      77,    46,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    56,    57,    58,    59,    60,    -1,    -1,    -1,    11,
      -1,    -1,    -1,    68,    69,    70,    18,    -1,    73,    74,
      75,    76,    77,    -1,    79,    80,    -1,    -1,    -1,    -1,
      32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    11,    -1,    -1,    46,    -1,    -1,    -1,    18,    -1,
      -1,    -1,    -1,    -1,    56,    57,    58,    59,    60,    -1,
      -1,    -1,    32,    -1,    -1,    -1,    68,    69,    70,    -1,
      -1,    73,    74,    75,    76,    77,    46,    79,    80,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    56,    57,    58,    59,
      60,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    69,
      70,    -1,    32,    73,    74,    75,    76,    77,    -1,    49,
      50,    51,    52,    53,    54,    55,    46,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    64,    65,    56,    57,    58,    59,
      60,    71,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      80,    81,    -1,    73,    74,    75,    76,    77,    -1,    79,
      80,     4,     5,    -1,    -1,    -1,    -1,    10,    11,    12,
      13,    14,    15,    16,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    25,    26,    27,    -1,    -1,    -1,    -1,    -1,
      -1,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      43,    -1,    45,    -1,    -1,    48
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     5,    10,    11,    12,    13,    14,    15,    16,
      17,    25,    26,    27,    34,    35,    43,    45,    48,    84,
      85,    86,    87,    88,    89,    90,    91,    93,    94,    95,
      96,    99,   101,   106,   109,   110,   112,   116,   132,   133,
     134,     6,     7,    20,    97,   100,     6,     9,    11,    18,
      32,    46,    56,    57,    58,    59,    60,    68,    69,    70,
      73,    74,    75,    76,    77,    79,    80,    92,   108,   112,
     117,   118,   120,   121,   122,    24,   117,   118,   119,    92,
     122,     8,    37,    39,    92,    92,    46,    85,     0,     3,
     135,     9,    92,    92,    92,   118,   118,    18,    18,    18,
      73,    74,   118,    36,    24,    67,    78,    79,    80,    81,
      92,    18,   118,    24,    67,    79,    80,    92,   119,    39,
      92,    92,    43,    50,    47,    92,    18,    67,    44,    67,
      92,    19,    79,   108,   120,   108,   120,   108,   120,    80,
      92,   117,    92,   118,   118,   118,   118,    24,   120,    67,
      92,   119,   118,    92,   118,    24,   119,    92,   111,    38,
      40,   126,    92,   108,    77,    44,    92,   103,   112,    92,
      92,    73,    74,    19,    19,    19,    24,    19,    24,    24,
      24,    24,   117,    19,    92,   119,    67,    92,    24,   119,
      67,    92,    24,   123,    18,    18,    65,    66,   118,   129,
     130,    50,    37,    92,    28,    29,    30,    31,    33,   105,
      24,   102,   108,   108,    77,    77,   117,   119,    92,    92,
     111,    61,   124,   108,   118,   129,    66,    18,    49,    50,
      51,    52,    53,    54,    55,    64,    65,    71,   131,    41,
      42,   118,     6,    18,    18,    32,    65,   103,    19,    19,
      19,    19,    19,   123,    62,   126,    24,   107,    19,    18,
     112,    65,    18,    64,    71,   118,   129,   129,    24,   125,
      92,    92,    98,    73,   104,    32,   102,    67,   112,   111,
      63,   128,   108,    19,   112,    19,   117,    18,    92,   126,
      24,    19,    19,   112,    44,    22,    72,   127,   107,    19,
      19,   117,    50,    98,    32,    65,   129,   117,   129,    21,
     113,    19,   118,    32,   124,    22,   125,    92,   114,   115,
      12,    23,    36,    24,    92,   114,    12,    23
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    83,    84,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    85,    85,    85,    85,
      85,    85,    85,    86,    87,    88,    89,    90,    91,    92,
      92,    92,    93,    94,    95,    96,    97,    97,    98,    98,
      99,   100,   100,   101,   101,   101,   101,   102,   102,   103,
     103,   103,   103,   103,   103,   103,   104,   105,   105,   105,
     105,   105,   106,   107,   107,   108,   108,   108,   108,   108,
     108,   108,   109,   110,   111,   111,   111,   112,   113,   113,
     114,   114,   115,   115,   115,   115,   115,   115,   116,   116,
     117,   117,   117,   117,   117,   117,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   119,   119,   119,   119,   119,   119,
     119,   120,   120,   120,   120,   121,   122,   122,   122,   122,
     122,   123,   123,   124,   124,   125,   125,   126,   126,   127,
     127,   128,   128,   129,   129,   129,   129,   129,   130,   130,
     130,   130,   130,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   131,   132,   133,   134,   135,   135
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     2,     2,     9,     0,     1,     1,     3,
       5,     1,     1,     5,     9,     8,     7,     0,     3,     5,
       6,     7,     1,     2,     3,     4,     1,     1,     1,     1,
       1,     1,     8,     0,     3,     1,     1,     2,     2,     1,
       1,     1,     4,     8,     1,     3,     2,    10,     0,     3,
       1,     3,     1,     2,     2,     3,     4,     4,     2,     2,
       1,     2,     3,     3,     4,     5,     3,     3,     3,     3,
       3,     2,     1,     1,     4,     4,     4,     4,     6,     6,
       6,     6,     1,     1,     0,     2,     3,     4,     3,     4,
       5,     1,     1,     3,     3,     4,     1,     1,     1,     1,
       1,     0,     3,     0,     6,     0,     5,     0,     2,     0,
       2,     0,     3,     0,     3,     1,     3,     3,     3,     4,
       5,     5,     6,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     2,     7,     2,     4,     0,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (&yylloc, sql_string, sql_result, scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, sql_string, sql_result, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (sql_string);
  YY_USE (sql_result);
  YY_USE (scanner);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, sql_string, sql_result, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), sql_string, sql_result, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, sql_string, sql_result, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (sql_string);
  YY_USE (sql_result);
  YY_USE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* commands: command_wrapper opt_semicolon  */
#line 231 "yacc_sql.y"
  {
    std::unique_ptr<ParsedSqlNode> sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[-1].sql_node));
    sql_result->add_sql_node(std::move(sql_node));
  }
#line 2040 "yacc_sql.cpp"
    break;

  case 23: /* exit_stmt: EXIT  */
#line 261 "yacc_sql.y"
         {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXIT);
    }
#line 2049 "yacc_sql.cpp"
    break;

  case 24: /* help_stmt: HELP  */
#line 267 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_HELP);
    }
#line 2057 "yacc_sql.cpp"
    break;

  case 25: /* sync_stmt: SYNC  */
#line 272 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SYNC);
    }
#line 2065 "yacc_sql.cpp"
    break;

  case 26: /* begin_stmt: TRX_BEGIN  */
#line 278 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_BEGIN);
    }
#line 2073 "yacc_sql.cpp"
    break;

  case 27: /* commit_stmt: TRX_COMMIT  */
#line 284 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_COMMIT);
    }
#line 2081 "yacc_sql.cpp"
    break;

  case 28: /* rollback_stmt: TRX_ROLLBACK  */
#line 290 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_ROLLBACK);
    }
#line 2089 "yacc_sql.cpp"
    break;

  case 29: /* ID: _id  */
#line 296 "yacc_sql.y"
        {
      (yyval.string) = (yyvsp[0].string);
    }
#line 2097 "yacc_sql.cpp"
    break;

  case 30: /* ID: DATA  */
#line 299 "yacc_sql.y"
           {
      (yyval.string) = strdup(token_name(sql_string, &(yyloc)).c_str());
    }
#line 2105 "yacc_sql.cpp"
    break;

  case 31: /* ID: aggr_func  */
#line 302 "yacc_sql.y"
                {
      (yyval.string) = strdup(token_name(sql_string, &(yyloc)).c_str());
    }
#line 2113 "yacc_sql.cpp"
    break;

  case 32: /* drop_table_stmt: DROP TABLE ID  */
#line 308 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_TABLE);
      (yyval.sql_node)->drop_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 2123 "yacc_sql.cpp"
    break;

  case 33: /* show_tables_stmt: SHOW TABLES  */
#line 315 "yacc_sql.y"
                {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
#line 2131 "yacc_sql.cpp"
    break;

  case 34: /* desc_table_stmt: DESC ID  */
#line 321 "yacc_sql.y"
             {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DESC_TABLE);
      (yyval.sql_node)->desc_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 2141 "yacc_sql.cpp"
    break;

  case 35: /* create_index_stmt: CREATE unique_flag INDEX ID ON ID LBRACE index_list RBRACE  */
#line 330 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      if((yyvsp[-7].unique_flag_type) == true){
        create_index.is_unique = true ;
      }
      create_index.index_name = (yyvsp[-5].string);
      create_index.relation_name = (yyvsp[-3].string);
      create_index.attribute_name.swap(*(yyvsp[-1].index_list_attr));
      free((yyvsp[-5].string));
      free((yyvsp[-3].string));
      free((yyvsp[-1].index_list_attr));
    }
#line 2159 "yacc_sql.cpp"
    break;

  case 36: /* unique_flag: %empty  */
#line 347 "yacc_sql.y"
    {
      (yyval.unique_flag_type) = false;
    }
#line 2167 "yacc_sql.cpp"
    break;

  case 37: /* unique_flag: UNIQUE  */
#line 351 "yacc_sql.y"
    {
      (yyval.unique_flag_type) = true ;
    }
#line 2175 "yacc_sql.cpp"
    break;

  case 38: /* index_list: ID  */
#line 358 "yacc_sql.y"
    {
      (yyval.index_list_attr) = new std::vector<std::string> ;
      (yyval.index_list_attr)->emplace_back((yyvsp[0].string)) ;
    }
#line 2184 "yacc_sql.cpp"
    break;

  case 39: /* index_list: ID COMMA index_list  */
#line 363 "yacc_sql.y"
    {
      if((yyvsp[0].index_list_attr) != nullptr){
        (yyval.index_list_attr) = (yyvsp[0].index_list_attr);
      }else{
        (yyval.index_list_attr) = new std::vector<std::string> ;
      }
      (yyval.index_list_attr)->emplace_back((yyvsp[-2].string));
    }
#line 2197 "yacc_sql.cpp"
    break;

  case 40: /* drop_index_stmt: DROP INDEX ID ON ID  */
#line 375 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_INDEX);
      (yyval.sql_node)->drop_index.index_name = (yyvsp[-2].string);
      (yyval.sql_node)->drop_index.relation_name = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 2209 "yacc_sql.cpp"
    break;

  case 41: /* table_type: VIEW  */
#line 385 "yacc_sql.y"
         {
        (yyval.table_type) = VIEW_T;
    }
#line 2217 "yacc_sql.cpp"
    break;

  case 42: /* table_type: TABLE  */
#line 388 "yacc_sql.y"
            {
        (yyval.table_type) = TABLE_T;
    }
#line 2225 "yacc_sql.cpp"
    break;

  case 43: /* create_table_stmt: CREATE table_type ID AS select_stmt  */
#line 395 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE);
      (yyval.sql_node)->create_table.relation_name = (yyvsp[-2].string);
      (yyval.sql_node)->create_table.select = std::make_unique<SqlExpr>((yyvsp[0].sql_node));
      (yyval.sql_node)->create_table.is_from_select = true;
      (yyval.sql_node)->create_table.type = (yyvsp[-3].table_type);
      free((yyvsp[-2].string));
    }
#line 2238 "yacc_sql.cpp"
    break;

  case 44: /* create_table_stmt: CREATE table_type ID LBRACE attr_def attr_def_list RBRACE AS select_stmt  */
#line 403 "yacc_sql.y"
                                                                               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = (yyval.sql_node)->create_table;
      create_table.relation_name = (yyvsp[-6].string);
      free((yyvsp[-6].string));

      std::vector<AttrInfoSqlNode> *src_attrs = (yyvsp[-3].attr_infos);

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*(yyvsp[-4].attr_info));
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete (yyvsp[-4].attr_info);

      (yyval.sql_node)->create_table.select = std::make_unique<SqlExpr>((yyvsp[0].sql_node));
      (yyval.sql_node)->create_table.is_from_select = true;
      (yyval.sql_node)->create_table.type = (yyvsp[-7].table_type);
    }
#line 2262 "yacc_sql.cpp"
    break;

  case 45: /* create_table_stmt: CREATE table_type ID LBRACE attr_def attr_def_list RBRACE select_stmt  */
#line 422 "yacc_sql.y"
                                                                            {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = (yyval.sql_node)->create_table;
      create_table.relation_name = (yyvsp[-5].string);
      free((yyvsp[-5].string));

      std::vector<AttrInfoSqlNode> *src_attrs = (yyvsp[-2].attr_infos);

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*(yyvsp[-3].attr_info));
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete (yyvsp[-3].attr_info);

      (yyval.sql_node)->create_table.select = std::make_unique<SqlExpr>((yyvsp[0].sql_node));
      (yyval.sql_node)->create_table.is_from_select = true;
      (yyval.sql_node)->create_table.type = (yyvsp[-6].table_type);
    }
#line 2286 "yacc_sql.cpp"
    break;

  case 46: /* create_table_stmt: CREATE table_type ID LBRACE attr_def attr_def_list RBRACE  */
#line 442 "yacc_sql.y"
    {
      if ((yyvsp[-5].table_type) != TABLE_T) {
        yyerror(&(yyloc), sql_string, sql_result, scanner, "only table can be created");
      }
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = (yyval.sql_node)->create_table;
      create_table.relation_name = (yyvsp[-4].string);
      free((yyvsp[-4].string));

      std::vector<AttrInfoSqlNode> *src_attrs = (yyvsp[-1].attr_infos);

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*(yyvsp[-2].attr_info));
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete (yyvsp[-2].attr_info);
      (yyval.sql_node)->create_table.type = (yyvsp[-5].table_type);
    }
#line 2310 "yacc_sql.cpp"
    break;

  case 47: /* attr_def_list: %empty  */
#line 464 "yacc_sql.y"
    {
      (yyval.attr_infos) = nullptr;
    }
#line 2318 "yacc_sql.cpp"
    break;

  case 48: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 468 "yacc_sql.y"
    {
      if ((yyvsp[0].attr_infos) != nullptr) {
        (yyval.attr_infos) = (yyvsp[0].attr_infos);
      } else {
        (yyval.attr_infos) = new std::vector<AttrInfoSqlNode>;
      }
      (yyval.attr_infos)->emplace_back(*(yyvsp[-1].attr_info));
      delete (yyvsp[-1].attr_info);
    }
#line 2332 "yacc_sql.cpp"
    break;

  case 49: /* attr_def: ID type LBRACE number RBRACE  */
#line 481 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-3].number);
      (yyval.attr_info)->name = (yyvsp[-4].string);
      (yyval.attr_info)->length = (yyvsp[-1].number);
      (yyval.attr_info)->is_allow_null = true;
      free((yyvsp[-4].string));
    }
#line 2345 "yacc_sql.cpp"
    break;

  case 50: /* attr_def: ID type LBRACE number RBRACE NULL_T  */
#line 490 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-4].number);
      (yyval.attr_info)->name = (yyvsp[-5].string);
      (yyval.attr_info)->length = (yyvsp[-2].number);
      (yyval.attr_info)->is_allow_null = true;
      free((yyvsp[-5].string));
    }
#line 2358 "yacc_sql.cpp"
    break;

  case 51: /* attr_def: ID type LBRACE number RBRACE NOT NULL_T  */
#line 499 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-5].number);
      (yyval.attr_info)->name = (yyvsp[-6].string);
       (yyval.attr_info)->length = (yyvsp[-3].number);
       (yyval.attr_info)->is_allow_null = false;
       free((yyvsp[-6].string));
    }
#line 2371 "yacc_sql.cpp"
    break;

  case 52: /* attr_def: ID  */
#line 507 "yacc_sql.y"
         {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->name = (yyvsp[0].string);
      (yyval.attr_info)->length = 4;
      (yyval.attr_info)->is_allow_null = true;
      free((yyvsp[0].string));
    }
#line 2383 "yacc_sql.cpp"
    break;

  case 53: /* attr_def: ID type  */
#line 515 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[0].number);
      (yyval.attr_info)->name = (yyvsp[-1].string);
      (yyval.attr_info)->length = 4;
      if ((yyval.attr_info)->type == TEXTS) {
        (yyval.attr_info)->length = 36;
      }
      (yyval.attr_info)->is_allow_null = true;
      free((yyvsp[-1].string));
    }
#line 2399 "yacc_sql.cpp"
    break;

  case 54: /* attr_def: ID type NULL_T  */
#line 527 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-1].number);
      (yyval.attr_info)->name = (yyvsp[-2].string);
      (yyval.attr_info)->length = 4;
      if ((yyval.attr_info)->type == TEXTS) {
        (yyval.attr_info)->length = 36;
      }
      (yyval.attr_info)->is_allow_null = true;
      free((yyvsp[-2].string));
    }
#line 2415 "yacc_sql.cpp"
    break;

  case 55: /* attr_def: ID type NOT NULL_T  */
#line 539 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-2].number);
      (yyval.attr_info)->name = (yyvsp[-3].string);
      (yyval.attr_info)->length = 4;
      if ((yyval.attr_info)->type == TEXTS) {
        (yyval.attr_info)->length = 36;
      }
      (yyval.attr_info)->is_allow_null = false;
      free((yyvsp[-3].string));
    }
#line 2431 "yacc_sql.cpp"
    break;

  case 56: /* number: NUMBER  */
#line 552 "yacc_sql.y"
           {(yyval.number) = (yyvsp[0].number);}
#line 2437 "yacc_sql.cpp"
    break;

  case 57: /* type: INT_T  */
#line 555 "yacc_sql.y"
               { (yyval.number)=INTS; }
#line 2443 "yacc_sql.cpp"
    break;

  case 58: /* type: STRING_T  */
#line 556 "yacc_sql.y"
               { (yyval.number)=CHARS; }
#line 2449 "yacc_sql.cpp"
    break;

  case 59: /* type: FLOAT_T  */
#line 557 "yacc_sql.y"
               { (yyval.number)=FLOATS; }
#line 2455 "yacc_sql.cpp"
    break;

  case 60: /* type: DATE_T  */
#line 558 "yacc_sql.y"
               { (yyval.number)=DATES;  }
#line 2461 "yacc_sql.cpp"
    break;

  case 61: /* type: TEXT_T  */
#line 559 "yacc_sql.y"
               { (yyval.number)=TEXTS;}
#line 2467 "yacc_sql.cpp"
    break;

  case 62: /* insert_stmt: INSERT INTO ID VALUES LBRACE value value_list RBRACE  */
#line 563 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_INSERT);
      (yyval.sql_node)->insertion.relation_name = (yyvsp[-5].string);
      if ((yyvsp[-1].value_list) != nullptr) {
        (yyval.sql_node)->insertion.values.swap(*(yyvsp[-1].value_list));
      }
      (yyval.sql_node)->insertion.values.emplace_back(*(yyvsp[-2].value));
      std::reverse((yyval.sql_node)->insertion.values.begin(), (yyval.sql_node)->insertion.values.end());
      delete (yyvsp[-2].value);
      free((yyvsp[-5].string));
    }
#line 2483 "yacc_sql.cpp"
    break;

  case 63: /* value_list: %empty  */
#line 578 "yacc_sql.y"
    {
      (yyval.value_list) = nullptr;
    }
#line 2491 "yacc_sql.cpp"
    break;

  case 64: /* value_list: COMMA value value_list  */
#line 581 "yacc_sql.y"
                              { 
      if ((yyvsp[0].value_list) != nullptr) {
        (yyval.value_list) = (yyvsp[0].value_list);
      } else {
        (yyval.value_list) = new std::vector<Value>;
      }
      (yyval.value_list)->emplace_back(*(yyvsp[-1].value));
      delete (yyvsp[-1].value);
    }
#line 2505 "yacc_sql.cpp"
    break;

  case 65: /* value: NUMBER  */
#line 592 "yacc_sql.y"
           {
      (yyval.value) = new Value((int)(yyvsp[0].number));
      (yyval.value)->set_name(token_name(sql_string, &(yyloc)));
      (yyloc) = (yylsp[0]);
    }
#line 2515 "yacc_sql.cpp"
    break;

  case 66: /* value: FLOAT  */
#line 597 "yacc_sql.y"
            {
      (yyval.value) = new Value((float)(yyvsp[0].floats));
      (yyval.value)->set_name(token_name(sql_string, &(yyloc)));
      (yyloc) = (yylsp[0]);
    }
#line 2525 "yacc_sql.cpp"
    break;

  case 67: /* value: '-' NUMBER  */
#line 602 "yacc_sql.y"
                 {
      (yyval.value) = new Value((int)-(yyvsp[0].number));
      (yyval.value)->set_name(token_name(sql_string, &(yyloc)));
      (yyloc) = (yylsp[-1]);
    }
#line 2535 "yacc_sql.cpp"
    break;

  case 68: /* value: '-' FLOAT  */
#line 607 "yacc_sql.y"
                {
      (yyval.value) = new Value((float)-(yyvsp[0].floats));
      (yyval.value)->set_name(token_name(sql_string, &(yyloc)));
      (yyloc) = (yylsp[-1]);
    }
#line 2545 "yacc_sql.cpp"
    break;

  case 69: /* value: DATE_STR  */
#line 612 "yacc_sql.y"
              {
      char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
      (yyval.value) = value_init_date(tmp);
      (yyval.value)->set_name(token_name(sql_string, &(yyloc)));
      free(tmp);
      if (!(yyval.value)->get_date().isValid()) {
        return -1;
      }
    }
#line 2559 "yacc_sql.cpp"
    break;

  case 70: /* value: NULL_T  */
#line 621 "yacc_sql.y"
            {
      (yyval.value) = new Value("");
      (yyval.value)->set_name(token_name(sql_string, &(yyloc)));
      (yyval.value)->set_null();
    }
#line 2569 "yacc_sql.cpp"
    break;

  case 71: /* value: SSS  */
#line 626 "yacc_sql.y"
         {
      char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
      (yyval.value)  = new Value(tmp);
      free(tmp);
    }
#line 2579 "yacc_sql.cpp"
    break;

  case 72: /* delete_stmt: DELETE FROM ID where  */
#line 636 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DELETE);
      (yyval.sql_node)->deletion.relation_name = (yyvsp[-1].string);
      if ((yyvsp[0].condition_tree) != nullptr) {
        (yyval.sql_node)->deletion.conditions.reset((yyvsp[0].condition_tree));
      }
      free((yyvsp[-1].string));
    }
#line 2592 "yacc_sql.cpp"
    break;

  case 73: /* update_stmt: UPDATE ID SET ID EQ expression update_list where  */
#line 647 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_UPDATE);
      (yyval.sql_node)->update.relation_name = (yyvsp[-6].string);
      if ((yyvsp[-1].update_list) != nullptr) {
         (yyval.sql_node)->update.update_list.swap(*(yyvsp[-1].update_list));
         delete (yyvsp[-1].update_list);
      }
      (yyval.sql_node)->update.update_list.insert(std::make_pair((yyvsp[-4].string), std::unique_ptr<Expression>((yyvsp[-2].expression))));
      if ((yyvsp[0].condition_tree) != nullptr) {
        (yyval.sql_node)->update.conditions.reset((yyvsp[0].condition_tree));
      }
      free((yyvsp[-6].string));
      free((yyvsp[-4].string));
    }
#line 2611 "yacc_sql.cpp"
    break;

  case 74: /* table_name: ID  */
#line 663 "yacc_sql.y"
       {
      (yyval.name_with_alias) = new NameWithAlias((yyvsp[0].string), "");
      free((yyvsp[0].string));
    }
#line 2620 "yacc_sql.cpp"
    break;

  case 75: /* table_name: ID AS ID  */
#line 667 "yacc_sql.y"
               {
      (yyval.name_with_alias) = new NameWithAlias((yyvsp[-2].string), (yyvsp[0].string));
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 2630 "yacc_sql.cpp"
    break;

  case 76: /* table_name: ID ID  */
#line 672 "yacc_sql.y"
            {
      (yyval.name_with_alias) = new NameWithAlias((yyvsp[-1].string), (yyvsp[0].string));
      free((yyvsp[-1].string));
      free((yyvsp[0].string));
    }
#line 2640 "yacc_sql.cpp"
    break;

  case 77: /* select_stmt: SELECT select_list FROM table_name rel_list inner_join_list where group_by having order_by_list_all  */
#line 680 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[-8].select_list) != nullptr) {
        //std::reverse($2->nodes.begin(), $2->nodes.end());
        (yyval.sql_node)->selection.select_exprs.swap((yyvsp[-8].select_list)->exprs);
        delete (yyvsp[-8].select_list);
      }
      if ((yyvsp[-5].relation_list) != nullptr) {
        (yyval.sql_node)->selection.relations.swap(*(yyvsp[-5].relation_list));
        delete (yyvsp[-5].relation_list);
      }
      (yyval.sql_node)->selection.relations.push_back(*(yyvsp[-6].name_with_alias));
      std::reverse((yyval.sql_node)->selection.relations.begin(), (yyval.sql_node)->selection.relations.end());

      // check relations' validity
      std::unordered_set<std::string> aliases;
        for (auto &rel : (yyval.sql_node)->selection.relations) {
            const auto& alias = rel.second;
            if (alias.empty()) continue;
            if (aliases.find(alias) != aliases.end()) {
            yyerror(&(yyloc), sql_string, sql_result, scanner, "duplicate alias");
            }
            aliases.insert(alias);
        }

      if ((yyvsp[-4].join_list) != nullptr) {
        (yyval.sql_node)->selection.join_list = (*(yyvsp[-4].join_list));
        (yyval.sql_node)->selection.join_list.relations.push_back(*(yyvsp[-6].name_with_alias));
        std::reverse((yyval.sql_node)->selection.join_list.relations.begin(),(yyval.sql_node)->selection.join_list.relations.end());
        std::reverse((yyval.sql_node)->selection.join_list.conditions.begin(),(yyval.sql_node)->selection.join_list.conditions.end());
        (yyval.sql_node)->selection.relations=(yyval.sql_node)->selection.join_list.relations;
      }

      if ((yyvsp[-3].condition_tree) != nullptr) {
        (yyval.sql_node)->selection.conditions.reset((yyvsp[-3].condition_tree));
      }

      if ((yyvsp[-2].group_by) != nullptr) {
        (yyval.sql_node)->selection.group_by = std::move(*(yyvsp[-2].group_by));
        delete (yyvsp[-2].group_by);
      }
      if ((yyvsp[-1].condition_tree) != nullptr) {
        (yyval.sql_node)->selection.having.reset((yyvsp[-1].condition_tree));
      }
      if ((yyvsp[0].rel_attr_list) != nullptr) {
        (yyval.sql_node)->selection.order_lists.swap(*(yyvsp[0].rel_attr_list));
        delete (yyvsp[0].rel_attr_list);
      }

      delete (yyvsp[-6].name_with_alias);
    }
#line 2696 "yacc_sql.cpp"
    break;

  case 78: /* order_by_list_all: %empty  */
#line 735 "yacc_sql.y"
    {
      (yyval.rel_attr_list) = nullptr;
    }
#line 2704 "yacc_sql.cpp"
    break;

  case 79: /* order_by_list_all: ORDER BY order_by_list  */
#line 739 "yacc_sql.y"
    {
      (yyval.rel_attr_list) = (yyvsp[0].rel_attr_list);
    }
#line 2712 "yacc_sql.cpp"
    break;

  case 80: /* order_by_list: order_by_  */
#line 746 "yacc_sql.y"
    {
      (yyval.rel_attr_list) = new std::vector<RelAttrSqlNode>;
      (yyval.rel_attr_list)->emplace_back(*(yyvsp[0].rel_attr));
      delete (yyvsp[0].rel_attr);
    }
#line 2722 "yacc_sql.cpp"
    break;

  case 81: /* order_by_list: order_by_ COMMA order_by_list  */
#line 752 "yacc_sql.y"
    {
      if ((yyvsp[0].rel_attr_list) != nullptr) {
        (yyval.rel_attr_list) = (yyvsp[0].rel_attr_list);
      } else {
        (yyval.rel_attr_list) = new std::vector<RelAttrSqlNode>;
      }
      (yyval.rel_attr_list)->emplace_back(*(yyvsp[-2].rel_attr));
      delete (yyvsp[-2].rel_attr);
    }
#line 2736 "yacc_sql.cpp"
    break;

  case 82: /* order_by_: ID  */
#line 765 "yacc_sql.y"
    {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name  = (yyvsp[0].string);
      (yyval.rel_attr)->sort_way = "ASC";
      free((yyvsp[0].string));
    }
#line 2747 "yacc_sql.cpp"
    break;

  case 83: /* order_by_: ID ASC  */
#line 772 "yacc_sql.y"
    {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name  = (yyvsp[-1].string);
      (yyval.rel_attr)->sort_way = "ASC";
      free((yyvsp[-1].string));
    }
#line 2758 "yacc_sql.cpp"
    break;

  case 84: /* order_by_: ID DESC  */
#line 779 "yacc_sql.y"
    {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name  = (yyvsp[-1].string);
      (yyval.rel_attr)->sort_way = "DESC";
      free((yyvsp[-1].string));
    }
#line 2769 "yacc_sql.cpp"
    break;

  case 85: /* order_by_: ID DOT ID  */
#line 786 "yacc_sql.y"
    {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-2].string);
      (yyval.rel_attr)->attribute_name = (yyvsp[0].string);
      (yyval.rel_attr)->sort_way = "ASC";
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 2782 "yacc_sql.cpp"
    break;

  case 86: /* order_by_: ID DOT ID ASC  */
#line 795 "yacc_sql.y"
    {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-3].string);
      (yyval.rel_attr)->attribute_name = (yyvsp[-1].string);
      (yyval.rel_attr)->sort_way = "ASC";
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 2795 "yacc_sql.cpp"
    break;

  case 87: /* order_by_: ID DOT ID DESC  */
#line 804 "yacc_sql.y"
    {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_name  = (yyvsp[-3].string);
      (yyval.rel_attr)->attribute_name = (yyvsp[-1].string);
      (yyval.rel_attr)->sort_way = "DESC";
      free((yyvsp[-3].string));
      free((yyvsp[-1].string));
    }
#line 2808 "yacc_sql.cpp"
    break;

  case 88: /* calc_stmt: CALC expression_list  */
#line 815 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CALC);
      std::reverse((yyvsp[0].expression_list)->begin(), (yyvsp[0].expression_list)->end());
      (yyval.sql_node)->calc.expressions.swap(*(yyvsp[0].expression_list));
      delete (yyvsp[0].expression_list);
    }
#line 2819 "yacc_sql.cpp"
    break;

  case 89: /* calc_stmt: SELECT expression_list  */
#line 822 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CALC);
      std::reverse((yyvsp[0].expression_list)->begin(), (yyvsp[0].expression_list)->end());
      (yyval.sql_node)->calc.expressions.swap(*(yyvsp[0].expression_list));
      delete (yyvsp[0].expression_list);
    }
#line 2830 "yacc_sql.cpp"
    break;

  case 90: /* expression_list: expression  */
#line 832 "yacc_sql.y"
    {
      (yyval.expression_list) = new std::vector<Expression*>;
      (yyval.expression_list)->emplace_back((yyvsp[0].expression));
    }
#line 2839 "yacc_sql.cpp"
    break;

  case 91: /* expression_list: expression ID  */
#line 837 "yacc_sql.y"
    {
      (yyval.expression_list) = new std::vector<Expression*>;
      (yyvsp[-1].expression)->set_name((yyvsp[0].string)); free((yyvsp[0].string));
      (yyval.expression_list)->emplace_back((yyvsp[-1].expression));
    }
#line 2849 "yacc_sql.cpp"
    break;

  case 92: /* expression_list: expression AS ID  */
#line 843 "yacc_sql.y"
    {
      (yyval.expression_list) = new std::vector<Expression*>;
      (yyvsp[-2].expression)->set_name((yyvsp[0].string)); free((yyvsp[0].string));
      (yyval.expression_list)->emplace_back((yyvsp[-2].expression));
    }
#line 2859 "yacc_sql.cpp"
    break;

  case 93: /* expression_list: expression COMMA expression_list  */
#line 849 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }
      (yyval.expression_list)->emplace_back((yyvsp[-2].expression));
    }
#line 2872 "yacc_sql.cpp"
    break;

  case 94: /* expression_list: expression ID COMMA expression_list  */
#line 858 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }
      (yyvsp[-3].expression)->set_name((yyvsp[-2].string)); free((yyvsp[-2].string));
      (yyval.expression_list)->emplace_back((yyvsp[-3].expression));
    }
#line 2886 "yacc_sql.cpp"
    break;

  case 95: /* expression_list: expression AS ID COMMA expression_list  */
#line 868 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }
      (yyvsp[-4].expression)->set_name((yyvsp[-2].string)); free((yyvsp[-2].string));
      (yyval.expression_list)->emplace_back((yyvsp[-4].expression));
    }
#line 2900 "yacc_sql.cpp"
    break;

  case 96: /* expression: expression '+' expression  */
#line 879 "yacc_sql.y"
                              {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::ADD, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2908 "yacc_sql.cpp"
    break;

  case 97: /* expression: expression '-' expression  */
#line 882 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::SUB, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2916 "yacc_sql.cpp"
    break;

  case 98: /* expression: expression '*' expression  */
#line 885 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::MUL, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2924 "yacc_sql.cpp"
    break;

  case 99: /* expression: expression '/' expression  */
#line 888 "yacc_sql.y"
                                {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::DIV, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc));
    }
#line 2932 "yacc_sql.cpp"
    break;

  case 100: /* expression: LBRACE expression RBRACE  */
#line 891 "yacc_sql.y"
                               {
      (yyval.expression) = (yyvsp[-1].expression);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2941 "yacc_sql.cpp"
    break;

  case 101: /* expression: '-' expression  */
#line 895 "yacc_sql.y"
                                  {
      (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, (yyvsp[0].expression), nullptr, sql_string, &(yyloc));
    }
#line 2949 "yacc_sql.cpp"
    break;

  case 102: /* expression: value  */
#line 898 "yacc_sql.y"
            {
      (yyval.expression) = new ValueExpr(*(yyvsp[0].value));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].value);
    }
#line 2959 "yacc_sql.cpp"
    break;

  case 103: /* expression: rel_attr  */
#line 903 "yacc_sql.y"
               {
      (yyval.expression) = new FieldExpr(*(yyvsp[0].rel_attr));
      // $$->set_name(token_name(sql_string, &@$));
      delete (yyvsp[0].rel_attr);
    }
#line 2969 "yacc_sql.cpp"
    break;

  case 104: /* expression: LENGTH LBRACE value RBRACE  */
#line 908 "yacc_sql.y"
                                 {
      if (RC::SUCCESS != (yyvsp[-1].value)->apply_length_func()) {
        yyerror(&(yyloc), sql_string, sql_result, scanner, "length function's parameter must be string");
      }
      (yyval.expression) = new ValueExpr(*(yyvsp[-1].value)); delete (yyvsp[-1].value);
    }
#line 2980 "yacc_sql.cpp"
    break;

  case 105: /* expression: LENGTH LBRACE rel_attr RBRACE  */
#line 914 "yacc_sql.y"
                                    {
      let f = new FieldExpr(*(yyvsp[-1].rel_attr));
      (yyval.expression) = f;
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      f->set_func(F_LENGTH);
      delete (yyvsp[-1].rel_attr);
    }
#line 2992 "yacc_sql.cpp"
    break;

  case 106: /* expression: ROUND LBRACE value RBRACE  */
#line 921 "yacc_sql.y"
                                {
      if (RC::SUCCESS != (yyvsp[-1].value)->apply_round_func()) {
          yyerror(&(yyloc), sql_string, sql_result, scanner, "round function's parameter must be float");
      }
      (yyval.expression) = new ValueExpr(*(yyvsp[-1].value)); delete (yyvsp[-1].value);
    }
#line 3003 "yacc_sql.cpp"
    break;

  case 107: /* expression: ROUND LBRACE rel_attr RBRACE  */
#line 927 "yacc_sql.y"
                                   {
      let f = new FieldExpr(*(yyvsp[-1].rel_attr));
      (yyval.expression) = f;
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      f->set_func(F_ROUND);
      delete (yyvsp[-1].rel_attr);
    }
#line 3015 "yacc_sql.cpp"
    break;

  case 108: /* expression: ROUND LBRACE value COMMA value RBRACE  */
#line 934 "yacc_sql.y"
                                            {
      if ((yyvsp[-1].value)->attr_type() != AttrType::INTS) {
        yyerror(&(yyloc), sql_string, sql_result, scanner, "round function's second parameter must be int");
      }
      if (RC::SUCCESS != (yyvsp[-3].value)->apply_round_func((yyvsp[-1].value)->get_int())) {
        yyerror(&(yyloc), sql_string, sql_result, scanner, "round function's first parameter must be float");
      }
      delete (yyvsp[-1].value);
      (yyval.expression) = new ValueExpr(*(yyvsp[-3].value)); delete (yyvsp[-3].value);
    }
#line 3030 "yacc_sql.cpp"
    break;

  case 109: /* expression: ROUND LBRACE rel_attr COMMA value RBRACE  */
#line 944 "yacc_sql.y"
                                               {
      let f = new FieldExpr(*(yyvsp[-3].rel_attr));
      (yyval.expression) = f;
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      f->set_func(F_ROUND);
      if ((yyvsp[-1].value)->attr_type() != AttrType::INTS) {
        yyerror(&(yyloc), sql_string, sql_result, scanner, "round function's second parameter must be int");
      }
      f->decimals = (yyvsp[-1].value)->get_int();
      delete (yyvsp[-1].value);
      delete (yyvsp[-3].rel_attr);
    }
#line 3047 "yacc_sql.cpp"
    break;

  case 110: /* expression: DATE_FORMAT LBRACE value COMMA SSS RBRACE  */
#line 956 "yacc_sql.y"
                                                {
      if ((yyvsp[-3].value)->attr_type() != AttrType::DATES) {
        yyerror(&(yyloc), sql_string, sql_result, scanner, "date_format function's first parameter must be date");
      }
      char *tmp = common::substr((yyvsp[-1].string),1,strlen((yyvsp[-1].string))-2);
      if (RC::SUCCESS != (yyvsp[-3].value)->apply_date_format_func(tmp)) {
        yyerror(&(yyloc), sql_string, sql_result, scanner, "date_format function's second parameter must be string");
      }
      (yyval.expression) = new ValueExpr(*(yyvsp[-3].value)); delete (yyvsp[-3].value);
      free(tmp);
      free((yyvsp[-1].string));
    }
#line 3064 "yacc_sql.cpp"
    break;

  case 111: /* expression: DATE_FORMAT LBRACE rel_attr COMMA SSS RBRACE  */
#line 968 "yacc_sql.y"
                                                   {
      let f = new FieldExpr(*(yyvsp[-3].rel_attr));
      (yyval.expression) = f;
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      f->set_func(F_DATE_FORMAT);

      char *tmp = common::substr((yyvsp[-1].string),1,strlen((yyvsp[-1].string))-2);
      f->date_format = tmp;
      free(tmp);
      free((yyvsp[-1].string));

      delete (yyvsp[-3].rel_attr);
    }
#line 3082 "yacc_sql.cpp"
    break;

  case 112: /* expression: aggr  */
#line 981 "yacc_sql.y"
           {
      (yyval.expression) = new AggregateExpr(*(yyvsp[0].aggregate_node), nullptr);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
      delete (yyvsp[0].aggregate_node);
    }
#line 3092 "yacc_sql.cpp"
    break;

  case 113: /* expression: select_stmt  */
#line 986 "yacc_sql.y"
                  {
      (yyval.expression) = new SqlExpr((yyvsp[0].sql_node));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 3101 "yacc_sql.cpp"
    break;

  case 114: /* select_list: %empty  */
#line 994 "yacc_sql.y"
    {
      (yyval.select_list) = nullptr;
    }
#line 3109 "yacc_sql.cpp"
    break;

  case 115: /* select_list: expression select_list  */
#line 997 "yacc_sql.y"
                             {
      if ((yyvsp[0].select_list) != nullptr) {
        (yyval.select_list) = (yyvsp[0].select_list);
      } else {
        (yyval.select_list) = new ExprList;
      }
      (yyval.select_list)->exprs.emplace_back((yyvsp[-1].expression));
    }
#line 3122 "yacc_sql.cpp"
    break;

  case 116: /* select_list: expression ID select_list  */
#line 1005 "yacc_sql.y"
                                {
        if (auto* t = dynamic_cast<FieldExpr*>((yyvsp[-2].expression))) {
            if (t->node().attribute_name == "*") {
                yyerror(&(yyloc), sql_string, sql_result, scanner, "can not use * AS alias in select list");
            }
        }

      if ((yyvsp[0].select_list) != nullptr) {
        (yyval.select_list) = (yyvsp[0].select_list);
      } else {
        (yyval.select_list) = new ExprList;
      }
      (yyvsp[-2].expression)->set_name((yyvsp[-1].string));
      free((yyvsp[-1].string));
      (yyval.select_list)->exprs.emplace_back((yyvsp[-2].expression));
    }
#line 3143 "yacc_sql.cpp"
    break;

  case 117: /* select_list: expression AS ID select_list  */
#line 1021 "yacc_sql.y"
                                   {
        if (auto* t = dynamic_cast<FieldExpr*>((yyvsp[-3].expression))) {
            if (t->node().attribute_name == "*") {
                yyerror(&(yyloc), sql_string, sql_result, scanner, "can not use * AS alias in select list");
            }
        }
      if ((yyvsp[0].select_list) != nullptr) {
        (yyval.select_list) = (yyvsp[0].select_list);
      } else {
        (yyval.select_list) = new ExprList;
      }
      (yyvsp[-3].expression)->set_name((yyvsp[-1].string));
      free((yyvsp[-1].string));
      (yyval.select_list)->exprs.emplace_back((yyvsp[-3].expression));
    }
#line 3163 "yacc_sql.cpp"
    break;

  case 118: /* select_list: COMMA expression select_list  */
#line 1036 "yacc_sql.y"
                                   {
      if ((yyvsp[0].select_list) != nullptr) {
        (yyval.select_list) = (yyvsp[0].select_list);
      } else {
        (yyval.select_list) = new ExprList;
      }
      (yyval.select_list)->exprs.emplace_back((yyvsp[-1].expression));
    }
#line 3176 "yacc_sql.cpp"
    break;

  case 119: /* select_list: COMMA expression ID select_list  */
#line 1044 "yacc_sql.y"
                                      {
        if (auto* t = dynamic_cast<FieldExpr*>((yyvsp[-2].expression))) {
            if (t->node().attribute_name == "*") {
                yyerror(&(yyloc), sql_string, sql_result, scanner, "can not use * AS alias in select list");
            }
        }
      if ((yyvsp[0].select_list) != nullptr) {
        (yyval.select_list) = (yyvsp[0].select_list);
      } else {
        (yyval.select_list) = new ExprList;
      }
      (yyvsp[-2].expression)->set_name((yyvsp[-1].string));
      free((yyvsp[-1].string));
      (yyval.select_list)->exprs.emplace_back((yyvsp[-2].expression));
    }
#line 3196 "yacc_sql.cpp"
    break;

  case 120: /* select_list: COMMA expression AS ID select_list  */
#line 1059 "yacc_sql.y"
                                         {
        if (auto* t = dynamic_cast<FieldExpr*>((yyvsp[-3].expression))) {
            if (t->node().attribute_name == "*") {
                yyerror(&(yyloc), sql_string, sql_result, scanner, "can not use * AS alias in select list");
            }
        }
      if ((yyvsp[0].select_list) != nullptr) {
        (yyval.select_list) = (yyvsp[0].select_list);
      } else {
        (yyval.select_list) = new ExprList;
      }
      (yyvsp[-3].expression)->set_name((yyvsp[-1].string));
      free((yyvsp[-1].string));
      (yyval.select_list)->exprs.emplace_back((yyvsp[-3].expression));
    }
#line 3216 "yacc_sql.cpp"
    break;

  case 121: /* rel_attr: '*'  */
#line 1077 "yacc_sql.y"
        {
      (yyval.rel_attr) = make_single_field("*");
    }
#line 3224 "yacc_sql.cpp"
    break;

  case 122: /* rel_attr: ID  */
#line 1080 "yacc_sql.y"
         {
      (yyval.rel_attr) = make_single_field((yyvsp[0].string));
      free((yyvsp[0].string));
    }
#line 3233 "yacc_sql.cpp"
    break;

  case 123: /* rel_attr: ID DOT ID  */
#line 1084 "yacc_sql.y"
                {
      (yyval.rel_attr) = make_single_table_field((yyvsp[-2].string), (yyvsp[0].string));
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 3243 "yacc_sql.cpp"
    break;

  case 124: /* rel_attr: ID DOT '*'  */
#line 1089 "yacc_sql.y"
                 {
      (yyval.rel_attr) = make_single_table_field((yyvsp[-2].string), "*");
      free((yyvsp[-2].string));
    }
#line 3252 "yacc_sql.cpp"
    break;

  case 125: /* aggr: aggr_func LBRACE rel_attr RBRACE  */
#line 1096 "yacc_sql.y"
                                     {
        if (get_display_name(*(yyvsp[-1].rel_attr)) == "*" && (yyvsp[-3].aggregate_func) == AggregateFunc::Count) {
            (yyval.aggregate_node) = make_aggr(AggregateFunc::CountStar);
        } else {
            (yyval.aggregate_node) = make_aggr((yyvsp[-3].aggregate_func));
            (yyval.aggregate_node)->attr = *(yyvsp[-1].rel_attr);
        }
        delete (yyvsp[-1].rel_attr);
    }
#line 3266 "yacc_sql.cpp"
    break;

  case 126: /* aggr_func: AVG  */
#line 1108 "yacc_sql.y"
        { (yyval.aggregate_func) = AggregateFunc::Avg; }
#line 3272 "yacc_sql.cpp"
    break;

  case 127: /* aggr_func: SUM  */
#line 1109 "yacc_sql.y"
          { (yyval.aggregate_func) = AggregateFunc::Sum; }
#line 3278 "yacc_sql.cpp"
    break;

  case 128: /* aggr_func: COUNT  */
#line 1110 "yacc_sql.y"
            { (yyval.aggregate_func) = AggregateFunc::Count; }
#line 3284 "yacc_sql.cpp"
    break;

  case 129: /* aggr_func: MAX  */
#line 1111 "yacc_sql.y"
          { (yyval.aggregate_func) = AggregateFunc::Max; }
#line 3290 "yacc_sql.cpp"
    break;

  case 130: /* aggr_func: MIN  */
#line 1112 "yacc_sql.y"
          { (yyval.aggregate_func) = AggregateFunc::Min; }
#line 3296 "yacc_sql.cpp"
    break;

  case 131: /* rel_list: %empty  */
#line 1117 "yacc_sql.y"
    {
      (yyval.relation_list) = nullptr;
    }
#line 3304 "yacc_sql.cpp"
    break;

  case 132: /* rel_list: COMMA table_name rel_list  */
#line 1120 "yacc_sql.y"
                                {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<NameWithAlias>;
      }

      (yyval.relation_list)->push_back(std::move(*(yyvsp[-1].name_with_alias)));
      delete (yyvsp[-1].name_with_alias);
    }
#line 3319 "yacc_sql.cpp"
    break;

  case 133: /* inner_join_list: %empty  */
#line 1133 "yacc_sql.y"
    {
      (yyval.join_list) = nullptr;
    }
#line 3327 "yacc_sql.cpp"
    break;

  case 134: /* inner_join_list: INNER JOIN table_name ON condition_tree inner_join_list  */
#line 1136 "yacc_sql.y"
                                                              {
      if ((yyvsp[0].join_list) != nullptr) {
        (yyval.join_list) = (yyvsp[0].join_list);
      } else {
        (yyval.join_list) = new JoinList;
      }
      (yyval.join_list)->conditions.push_back((yyvsp[-1].condition_tree));
      (yyval.join_list)->relations.push_back(std::move(*(yyvsp[-3].name_with_alias)));
      delete (yyvsp[-3].name_with_alias);
    }
#line 3342 "yacc_sql.cpp"
    break;

  case 135: /* update_list: %empty  */
#line 1148 "yacc_sql.y"
    {
    (yyval.update_list) = nullptr;
    }
#line 3350 "yacc_sql.cpp"
    break;

  case 136: /* update_list: COMMA ID EQ expression update_list  */
#line 1151 "yacc_sql.y"
                                         {
      if ((yyvsp[0].update_list) != nullptr) {
        (yyval.update_list) = (yyvsp[0].update_list);
      } else {
        (yyval.update_list) = new std::unordered_map<std::string, std::unique_ptr<Expression>>;

      }
      (yyval.update_list)->insert(std::make_pair((yyvsp[-3].string), std::unique_ptr<Expression>((yyvsp[-1].expression))));
      free((yyvsp[-3].string));
    }
#line 3365 "yacc_sql.cpp"
    break;

  case 137: /* where: %empty  */
#line 1164 "yacc_sql.y"
    {
      (yyval.condition_tree) = nullptr;
    }
#line 3373 "yacc_sql.cpp"
    break;

  case 138: /* where: WHERE condition_tree  */
#line 1167 "yacc_sql.y"
                           {
      (yyval.condition_tree) = (yyvsp[0].condition_tree);  
    }
#line 3381 "yacc_sql.cpp"
    break;

  case 139: /* having: %empty  */
#line 1173 "yacc_sql.y"
    {
      (yyval.condition_tree) = nullptr;
    }
#line 3389 "yacc_sql.cpp"
    break;

  case 140: /* having: HAVING condition_tree  */
#line 1176 "yacc_sql.y"
                            {
      (yyval.condition_tree) = (yyvsp[0].condition_tree);
    }
#line 3397 "yacc_sql.cpp"
    break;

  case 141: /* group_by: %empty  */
#line 1182 "yacc_sql.y"
    {
      (yyval.group_by) = nullptr;
    }
#line 3405 "yacc_sql.cpp"
    break;

  case 142: /* group_by: GROUP BY expression_list  */
#line 1185 "yacc_sql.y"
                               {
      (yyval.group_by) = new GroupBy;
      if ((yyvsp[0].expression_list) != nullptr) {
          for (auto e : *(yyvsp[0].expression_list)) {
              (yyval.group_by)->exprs.emplace_back(e);
          }
          delete (yyvsp[0].expression_list);
      }
    }
#line 3419 "yacc_sql.cpp"
    break;

  case 143: /* condition_tree: %empty  */
#line 1197 "yacc_sql.y"
    {
      (yyval.condition_tree) = nullptr;
    }
#line 3427 "yacc_sql.cpp"
    break;

  case 144: /* condition_tree: LBRACE condition_tree RBRACE  */
#line 1200 "yacc_sql.y"
                                   {
      (yyval.condition_tree) = (yyvsp[-1].condition_tree);
    }
#line 3435 "yacc_sql.cpp"
    break;

  case 145: /* condition_tree: condition  */
#line 1203 "yacc_sql.y"
                {
      (yyval.condition_tree) = new ConditionTree;
      (yyval.condition_tree)->node.reset((yyvsp[0].condition));
    }
#line 3444 "yacc_sql.cpp"
    break;

  case 146: /* condition_tree: condition_tree AND condition_tree  */
#line 1207 "yacc_sql.y"
                                        {
      (yyval.condition_tree) = new ConditionTree;
      (yyval.condition_tree)->op = ConditionTree::AND_OP;
      (yyval.condition_tree)->left.reset((yyvsp[-2].condition_tree));
      (yyval.condition_tree)->right.reset((yyvsp[0].condition_tree));
    }
#line 3455 "yacc_sql.cpp"
    break;

  case 147: /* condition_tree: condition_tree OR condition_tree  */
#line 1213 "yacc_sql.y"
                                       {
      (yyval.condition_tree) = new ConditionTree;
      (yyval.condition_tree)->op = ConditionTree::OR_OP;
      (yyval.condition_tree)->left.reset((yyvsp[-2].condition_tree));
      (yyval.condition_tree)->right.reset((yyvsp[0].condition_tree));
    }
#line 3466 "yacc_sql.cpp"
    break;

  case 148: /* condition: expression comp_op expression  */
#line 1221 "yacc_sql.y"
                                  {
        (yyval.condition) = new ConditionSqlNode;
        (yyval.condition)->left = std::unique_ptr<Expression>((yyvsp[-2].expression));
        (yyval.condition)->right = std::unique_ptr<Expression>((yyvsp[0].expression));
        (yyval.condition)->comp = (yyvsp[-1].comp);
    }
#line 3477 "yacc_sql.cpp"
    break;

  case 149: /* condition: EXISTS LBRACE select_stmt RBRACE  */
#line 1227 "yacc_sql.y"
                                       {
        (yyval.condition) = new ConditionSqlNode;
        (yyval.condition)->left.reset(new SqlExpr((yyvsp[-1].sql_node)));
        (yyval.condition)->comp = EXISTS_OP;
    }
#line 3487 "yacc_sql.cpp"
    break;

  case 150: /* condition: NOT EXISTS LBRACE select_stmt RBRACE  */
#line 1232 "yacc_sql.y"
                                           {
        (yyval.condition) = new ConditionSqlNode;
        (yyval.condition)->left.reset(new SqlExpr((yyvsp[-1].sql_node)));
        (yyval.condition)->comp = NOT_EXISTS_OP;
    }
#line 3497 "yacc_sql.cpp"
    break;

  case 151: /* condition: expression IN LBRACE expression_list RBRACE  */
#line 1237 "yacc_sql.y"
                                                  {
        (yyval.condition) = new ConditionSqlNode;
        (yyval.condition)->left = std::unique_ptr<Expression>((yyvsp[-4].expression));
        (yyval.condition)->right = std::make_unique<ExpressionsExpr>(std::move(*(yyvsp[-1].expression_list)));
        delete (yyvsp[-1].expression_list);
        (yyval.condition)->comp = IN_OP;
    }
#line 3509 "yacc_sql.cpp"
    break;

  case 152: /* condition: expression NOT IN LBRACE expression_list RBRACE  */
#line 1244 "yacc_sql.y"
                                                      {
        (yyval.condition) = new ConditionSqlNode;
        (yyval.condition)->left = std::unique_ptr<Expression>((yyvsp[-5].expression));
        (yyval.condition)->right = std::make_unique<ExpressionsExpr>(std::move(*(yyvsp[-1].expression_list)));
        delete (yyvsp[-1].expression_list);
        (yyval.condition)->comp = NOT_IN_OP;
    }
#line 3521 "yacc_sql.cpp"
    break;

  case 153: /* comp_op: EQ  */
#line 1254 "yacc_sql.y"
         { (yyval.comp) = EQUAL_TO; }
#line 3527 "yacc_sql.cpp"
    break;

  case 154: /* comp_op: LT  */
#line 1255 "yacc_sql.y"
         { (yyval.comp) = LESS_THAN; }
#line 3533 "yacc_sql.cpp"
    break;

  case 155: /* comp_op: GT  */
#line 1256 "yacc_sql.y"
         { (yyval.comp) = GREAT_THAN; }
#line 3539 "yacc_sql.cpp"
    break;

  case 156: /* comp_op: LE  */
#line 1257 "yacc_sql.y"
         { (yyval.comp) = LESS_EQUAL; }
#line 3545 "yacc_sql.cpp"
    break;

  case 157: /* comp_op: GE  */
#line 1258 "yacc_sql.y"
         { (yyval.comp) = GREAT_EQUAL; }
#line 3551 "yacc_sql.cpp"
    break;

  case 158: /* comp_op: NE  */
#line 1259 "yacc_sql.y"
         { (yyval.comp) = NOT_EQUAL; }
#line 3557 "yacc_sql.cpp"
    break;

  case 159: /* comp_op: IS  */
#line 1260 "yacc_sql.y"
         { (yyval.comp) = IS_OP;}
#line 3563 "yacc_sql.cpp"
    break;

  case 160: /* comp_op: IS NOT  */
#line 1261 "yacc_sql.y"
            { (yyval.comp) = IS_NOT_OP;}
#line 3569 "yacc_sql.cpp"
    break;

  case 161: /* comp_op: LIKE  */
#line 1262 "yacc_sql.y"
           { (yyval.comp) = LIKE_OP; }
#line 3575 "yacc_sql.cpp"
    break;

  case 162: /* comp_op: NOT LIKE  */
#line 1263 "yacc_sql.y"
               { (yyval.comp) = NOT_LIKE_OP; }
#line 3581 "yacc_sql.cpp"
    break;

  case 163: /* load_data_stmt: LOAD DATA INFILE SSS INTO TABLE ID  */
#line 1268 "yacc_sql.y"
    {
      char *tmp_file_name = common::substr((yyvsp[-3].string), 1, strlen((yyvsp[-3].string)) - 2);
      
      (yyval.sql_node) = new ParsedSqlNode(SCF_LOAD_DATA);
      (yyval.sql_node)->load_data.relation_name = (yyvsp[0].string);
      (yyval.sql_node)->load_data.file_name = tmp_file_name;
      free((yyvsp[0].string));
      free(tmp_file_name);
    }
#line 3595 "yacc_sql.cpp"
    break;

  case 164: /* explain_stmt: EXPLAIN command_wrapper  */
#line 1281 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXPLAIN);
      (yyval.sql_node)->explain.sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[0].sql_node));
    }
#line 3604 "yacc_sql.cpp"
    break;

  case 165: /* set_variable_stmt: SET ID EQ value  */
#line 1289 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SET_VARIABLE);
      (yyval.sql_node)->set_variable.name  = (yyvsp[-2].string);
      (yyval.sql_node)->set_variable.value = *(yyvsp[0].value);
      free((yyvsp[-2].string));
      delete (yyvsp[0].value);
    }
#line 3616 "yacc_sql.cpp"
    break;


#line 3620 "yacc_sql.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, sql_string, sql_result, scanner, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, sql_string, sql_result, scanner);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, sql_string, sql_result, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, sql_string, sql_result, scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, sql_string, sql_result, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, sql_string, sql_result, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 1301 "yacc_sql.y"

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
