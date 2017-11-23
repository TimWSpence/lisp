#ifndef _BUILTIN
  #define _BUILTIN
  #include "builtin.h"
#endif

#ifndef _CORE
  #define _CORE
  #include "core.h"
#endif

lval* builtin_head(lenv* e, lval* a) {
  LASSERT(a, a->count == 1, "Function 'head' passed wrong number of arguments - was '%d' instead of '1'", a->count);
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'head' passed '%s' instead of LVAL_QEXPR", ltype_name(a->cell[0]->type));
  LASSERT(a, a->cell[0]->count != 0, "Function 'head' passed {}");

  lval* v = lval_take(a, 0);
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lenv* e, lval* a) {
  LASSERT(a, a->count == 1, "Function 'tail' passed wrong number of arguments - was '%d' instead of '1'", a->count);
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'tail' passed '%s' instead of LVAL_QEXPR", ltype_name(a->cell[0]->type));
  LASSERT(a, a->cell[0]->count != 0, "Function 'tail' passed {}");

  lval* v = lval_take(a, 0);
  lval_del(lval_pop(v, 0));
  return v;
}

lval* builtin_list(lenv* e, lval* a) {
  a->type = LVAL_QEXPR;
  return a;
}

lval* builtin_var(lenv* e, lval* a, char* func) {
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'def' passed '%s' instead of LVAL_QEXPR", ltype_name(a->cell[0]->type));

  lval* syms = a->cell[0];

  for(int i=0; i<syms->count; i++) {
    LASSERT(a, syms->cell[i]->type == LVAL_SYM, "Function 'def' can only define LVAL_SYM, not '%s'", ltype_name(syms->cell[i]->type));
  }

  LASSERT(a, syms->count == a->count-1, "Function 'def' cannot define incorrect number of values to symbols");

  for(int i=0; i<syms->count; i++) {
    if (strcmp(func, "def") == 0) {
      lenv_def(e, syms->cell[i], a->cell[i+1]);
    }

    if (strcmp(func, "=")   == 0) {
      lenv_put(e, syms->cell[i], a->cell[i+1]);
    }
  }

  lval_del(a);
  return lval_sexpr();
}

lval* builtin_def(lenv* e, lval* a) {
  return builtin_var(e, a, "def");
}

lval* builtin_put(lenv* e, lval* a) {
  return builtin_var(e, a, "=");
}

lval* builtin_lambda(lenv* e, lval* a) {
  LASSERT_NUM("\\", a, 2);
  LASSERT_TYPE("\\", a, 0, LVAL_QEXPR);
  LASSERT_TYPE("\\", a, 1, LVAL_QEXPR);

  for(int i=0; i<a->cell[0]->count; i++) {
    LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM), "Cannot define non-symbol. Got %s, Expected %s.", ltype_name(a->cell[0]->cell[i]->type),ltype_name(LVAL_SYM));
  }

  lval* formals = lval_pop(a, 0);
  lval* body = lval_pop(a, 0);
  lval_del(a);

  return lval_lambda(formals, body);
}

lval* builtin_op(lenv* e, lval* a, char* op) {
  for (int i = 0; i < a->count; i++) {
    if (a->cell[i]->type != LVAL_NUM) {
      lval_del(a);
      return lval_err("Op can only operate on LVAL_NUM, not '%s'", ltype_name(a->cell[i]->type));
    }
  }

  lval* x = lval_pop(a, 0);

  if ((strcmp(op, "-") == 0) && a->count == 0) {
    x->num = -x->num;
  }

  while(a->count > 0) {
    lval* y = lval_pop(a, 0);
    if (strcmp(op, "+") == 0) { x->num += y->num; }
    if (strcmp(op, "-") == 0) { x->num -= y->num; }
    if (strcmp(op, "*") == 0) { x->num *= y->num; }
    if (strcmp(op, "/") == 0) {
      if (y->num == 0) {
        lval_del(x); lval_del(y);
        x = lval_err("Division By Zero!"); break;
      }
      x->num /= y->num;
    }
    lval_del(y);
  }
  lval_del(a);
  return x;
}

lval* builtin_add(lenv* e, lval* a) {
  return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
  return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
  return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
  return builtin_op(e, a, "/");
}

lval* builtin_ord(lenv* e, lval * a, char* op) {
  LASSERT_NUM(op, a, 2);
  LASSERT_TYPE(op, a, 0, LVAL_NUM);
  LASSERT_TYPE(op, a, 1, LVAL_NUM);

  int r;
  if (strcmp(op, ">")  == 0) {
    r = (a->cell[0]->num >  a->cell[1]->num);
  }
  if (strcmp(op, "<")  == 0) {
    r = (a->cell[0]->num <  a->cell[1]->num);
  }
  if (strcmp(op, ">=") == 0) {
    r = (a->cell[0]->num >= a->cell[1]->num);
  }
  if (strcmp(op, "<=") == 0) {
    r = (a->cell[0]->num <= a->cell[1]->num);
  }
  lval_del(a);
  return lval_num(r);
}

lval* builtin_cmp(lenv* e, lval* a, char* op) {
  LASSERT_NUM(op, a, 2);
  int r;
  if (strcmp(op, "==") == 0) {
    r = lval_eq(a->cell[0], a->cell[1]);
  }
  if (strcmp(op, "!=") == 0) {
    r = !lval_eq(a->cell[0], a->cell[1]);
  }
  lval_del(a);
  return lval_num(r);
}

lval* builtin_eq(lenv* e, lval* a) {
  return builtin_cmp(e, a, "==");
}

lval* builtin_ne(lenv* e, lval* a) {
  return builtin_cmp(e, a, "!=");
}

lval* builtin_if(lenv* e, lval* a) {
  LASSERT_NUM("if", a, 3);
  LASSERT_TYPE("if", a, 0, LVAL_NUM);
  LASSERT_TYPE("if", a, 1, LVAL_QEXPR);
  LASSERT_TYPE("if", a, 2, LVAL_QEXPR);

  lval* x;
  a->cell[1]->type = LVAL_SEXPR;
  a->cell[2]->type = LVAL_SEXPR;

  if(a->cell[0]->num) {
    x = lval_eval(e, lval_pop(a, 1));
  } else {
    x = lval_eval(e, lval_pop(a, 2));
  }

  lval_del(a);
  return x;
}

lval* builtin_gt(lenv* e, lval* a) {
  return builtin_ord(e, a, ">");
}

lval* builtin_lt(lenv* e, lval* a) {
  return builtin_ord(e, a, "<");
}

lval* builtin_ge(lenv* e, lval* a) {
  return builtin_ord(e, a, ">=");
}

lval* builtin_le(lenv* e, lval* a) {
  return builtin_ord(e, a, "<=");
}

lval* builtin_load(lenv* e, lval* a) {
  LASSERT_NUM("load", a, 1);
  LASSERT_TYPE("load", a, 0, LVAL_STR);

  mpc_result_t r;
  if (mpc_parse_contents(a->cell[0]->str, Lispy, &r)) {
    lval* expr = lval_read(r.output);
    mpc_ast_delete(r.output);

    while(expr->count) {
      lval* x = lval_eval(e, lval_pop(expr, 0));
      if (x->type == LVAL_ERR) { lval_println(x); }
      lval_del(x);
    }

    lval_del(expr);
    lval_del(a);

    return lval_sexpr();
  } else {
    char* err_msg = mpc_err_string(r.error);
    mpc_err_delete(r.error);

    /* Create new error message using it */
    lval* err = lval_err("Could not load Library %s", err_msg);
    free(err_msg);
    lval_del(a);

    return err;
  }
}

//TODO should slurp/spit be using binary mode?
lval* builtin_slurp(lenv* e, lval* a) {
  LASSERT_NUM("slurp", a, 1);
  LASSERT_TYPE("slurp", a, 0, LVAL_STR);

  char *file_contents;
  long input_file_size;
  FILE *input_file = fopen(a->cell[0]->str, "rb");
  if (input_file == NULL) {
    return lval_err(strerror(errno));
  }
  fseek(input_file, 0, SEEK_END);
  input_file_size = ftell(input_file);
  rewind(input_file);
  file_contents = malloc(input_file_size * (sizeof(char)));
  fread(file_contents, sizeof(char), input_file_size, input_file);
  fclose(input_file);

  lval* x = malloc(sizeof(lval));
  x->type = LVAL_STR;
  x->str = malloc(input_file_size + 1);
  strcpy(x->str, file_contents);

  free(file_contents);
  lval_del(a);
  return x;
}

lval* builtin_spit(lenv* e, lval* a) {
  LASSERT_NUM("spit", a, 2);
  LASSERT_TYPE("spit", a, 0, LVAL_STR);
  LASSERT_TYPE("spit", a, 1, LVAL_STR);

  FILE *output_file = fopen(a->cell[0]->str, "wb");
  if (output_file == NULL) {
    return lval_err(strerror(errno));
  }
  fwrite(a->cell[1]->str, sizeof(char), strlen(a->cell[1]->str), output_file);
  fclose(output_file);
  lval_del(a);

  return lval_sexpr();
}

lval* builtin_print(lenv* e, lval* a) {
  for (int i = 0; i < a->count; i++) {
    lval_print(a->cell[i]); putchar(' ');
  }

  putchar('\n');
  lval_del(a);

  return lval_sexpr();
}

lval* builtin_error(lenv* e, lval* a) {
  LASSERT_NUM("error", a, 1);
  LASSERT_TYPE("error", a, 0, LVAL_STR);

  /* Construct Error from first argument */
  lval* err = lval_err(a->cell[0]->str);

  /* Delete arguments and return */
  lval_del(a);
  return err;
}

lval* builtin_eval(lenv* e, lval* a) {
  LASSERT(a, a->count == 1, "Function 'eval' passed the wrong number of arguments - passed %d rather than 1", a->count);
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "Function 'eval' passed '%s' rather than LVAL_QEXPR", ltype_name(a->cell[0]->type));

  lval* x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {
  for (int i = 0; i < a->count; i++) {
    LASSERT(a, a->cell[i]->type == LVAL_QEXPR, "Function 'join' passed '%s' rather than LVAL_QEXPR", ltype_name(a->cell[i]->type));
  }

  lval* x = lval_pop(a, 0);

  while(a->count) {
    x = lval_join(x, lval_pop(a, 0));
  }
  lval_del(a);
  return x;
}

lval* builtin_seq(lenv* e, lval* a) {
  LASSERT_NUM("seq", a, 1);
  LASSERT_TYPE("seq", a, 0, LVAL_STR);

  char* str = a->cell[0]->str;
  lval* seq = lval_qexpr();
  for(int i=0; i<strlen(str); i++) {
    lval_add(seq, lval_char(str[i]));
  }
  free(a);
  return seq;
}

lval* builtin_str(lenv* e, lval* a) {
  LASSERT_NUM("str", a, 1);
  LASSERT_TYPE("str", a, 0, LVAL_QEXPR);
  for(int i=0; i<a->cell[0]->count; i++) {
    LASSERT_TYPE("str", a->cell[0], i, LVAL_CHAR);
  }

  lval* seq = a->cell[0];
  int count = seq->count;
  char* str = malloc(sizeof(char) * (count+1));
  for(int i=0; i<count; i++) {
    str[i] = seq->cell[i]->character;
  }
  str[count] = '\0';

  lval_del(a);
  return lval_str(str);
}
