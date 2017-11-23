#ifndef _CORE
  #define _CORE
  #include "core.h"
#endif

/* List functions */
lval* builtin_head(lenv* e, lval* a);
lval* builtin_tail(lenv* e, lval* a);
lval* builtin_list(lenv* e, lval* a);
lval* builtin_join(lenv* e, lval* a);

/* Definition functions */
lval* builtin_def(lenv* e, lval* a);
lval* builtin_put(lenv* e, lval* a);
lval* builtin_lambda(lenv* e, lval* a);

/* Math functions */
lval* builtin_add(lenv* e, lval* a);
lval* builtin_sub(lenv* e, lval* a);
lval* builtin_mul(lenv* e, lval* a);
lval* builtin_div(lenv* e, lval* a);

/* Comparison functions */
lval* builtin_eq(lenv* e, lval* a);
lval* builtin_ne(lenv* e, lval* a);
lval* builtin_gt(lenv* e, lval* a);
lval* builtin_lt(lenv* e, lval* a);
lval* builtin_ge(lenv* e, lval* a);
lval* builtin_le(lenv* e, lval* a);

/* Conditional functions */
lval* builtin_if(lenv* e, lval* a);

/* I/O functions */
//TODO should slurp/spit be using binary mode?
lval* builtin_slurp(lenv* e, lval* a);
lval* builtin_spit(lenv* e, lval* a);

/* String functions */
lval* builtin_seq(lenv* e, lval* a);
lval* builtin_str(lenv* e, lval* a);
lval* builtin_char(lenv* e, lval* a);
lval* builtin_int(lenv* e, lval* a);
lval* builtin_str_includes(lenv* e, lval* a);

/* Miscellaneous functions */
lval* builtin_load(lenv* e, lval* a);
lval* builtin_error(lenv* e, lval* a);
lval* builtin_print(lenv* e, lval* a);
lval* builtin_eval(lenv* e, lval* a);
