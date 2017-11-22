#include <stdio.h>
#include <string.h>
#include "mpc.h"

struct lval;
struct lenv;

typedef struct lval lval;
typedef struct lenv lenv;
typedef lval* (*lbuiltin)(lenv*, lval*);

enum { LVAL_ERR, LVAL_NUM,   LVAL_SYM, LVAL_STR, LVAL_FUN, LVAL_SEXPR, LVAL_QEXPR };

struct lval {
  int type;

  /* Basic */
  long num;
  char* err;
  char* sym;
  char* str;

  /* Function */
  lbuiltin builtin;
  lenv* env;
  lval* formals;
  lval* body;

  /* Expression */
  int count;
  lval** cell;
};

struct lenv {
  lenv* par;
  int count;
  char** syms;
  lval** vals;
};

//Possibly these should be defined somewhere else. Maybe if we split out the parsing logic it could go in the corresponding header file
mpc_parser_t* Number;
mpc_parser_t* Symbol;
mpc_parser_t* String;
mpc_parser_t* Comment;
mpc_parser_t* Sexpr;
mpc_parser_t* Qexpr;
mpc_parser_t* Expr;
mpc_parser_t* Lispy;

#define LASSERT(args, cond, fmt, ...)           \
  if (!(cond)) {                                \
    lval* err = lval_err(fmt, ##__VA_ARGS__);   \
    lval_del(args);                             \
    return err;                                 \
  }

#define LASSERT_TYPE(func, args, index, expect)                         \
  LASSERT(args, args->cell[index]->type == expect,                      \
          "Function '%s' passed incorrect type for argument %i. "       \
          "Got %s, Expected %s.",                                       \
          func, index, ltype_name(args->cell[index]->type), ltype_name(expect))

#define LASSERT_NUM(func, args, num)                              \
  LASSERT(args, args->count == num,                               \
          "Function '%s' passed incorrect number of arguments. "  \
          "Got %i, Expected %i.",                                 \
          func, args->count, num)

#define LASSERT_NOT_EMPTY(func, args, index)                        \
  LASSERT(args, args->cell[index]->count != 0,                      \
          "Function '%s' passed {} for argument %i.", func, index);

//Convert lval type -> string
char* ltype_name(int t);

//Functions to operate on lvals
lval* lval_num(long x);
lval* lval_err(char* fmt, ...);
lval* lval_sym(char* s);
lval* lval_str(char* s);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
lval* lval_fun(lbuiltin func);
lval* lval_lambda(lval* formals, lval* body);
void lval_del(lval* v);
lval* lval_copy(lval* v);
lval* lval_eval(lenv* e, lval* v);
lval* lval_add(lval* v, lval* x);
lval* lval_pop(lval* v, int i);
lval* lval_take(lval* v, int i);
void lval_print(lval* v);
void lval_println(lval* v);
void lval_expr_print(lval* v, char open, char close);
void lval_print_str(lval* v);
int lval_eq(lval* x, lval* y);
lval* lval_join(lval* x, lval* y);
lval* lval_call(lenv* e, lval* f, lval* a);
lval* lval_eval_sexpr(lenv* e, lval* v);
lval* lval_eval(lenv* e, lval* v);

//Functions to operate on lenvs
lenv* lenv_new(void);
void lenv_del(lenv* e);
lenv* lenv_copy(lenv* e);
void lenv_put(lenv* e, lval* k, lval* v);
void lenv_def(lenv* e, lval* k, lval* v);
lval* lenv_get(lenv* e, lval* k);

/* Functions to read lval from mpc ast */
lval* lval_read_num(mpc_ast_t* t);
lval* lval_read_str(mpc_ast_t* t);
lval* lval_read(mpc_ast_t* t);
