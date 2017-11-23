#ifndef _CORE
  #define _CORE
  #include "core.h"
#endif

#ifndef _BUILTIN
  #define _BUILTIN
  #include "builtin.h"
#endif

#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#endif

void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
  lval* k = lval_sym(name);
  lval* v = lval_fun(func);
  lenv_put(e, k, v);
  lval_del(k);
  lval_del(v);
}

void lenv_add_builtins(lenv* e) {
  /* List Functions */
  lenv_add_builtin(e, "list", builtin_list);
  lenv_add_builtin(e, "head", builtin_head);
  lenv_add_builtin(e, "tail", builtin_tail);
  lenv_add_builtin(e, "eval", builtin_eval);
  lenv_add_builtin(e, "join", builtin_join);

  /* Mathematical Functions */
  lenv_add_builtin(e, "+", builtin_add);
  lenv_add_builtin(e, "-", builtin_sub);
  lenv_add_builtin(e, "*", builtin_mul);
  lenv_add_builtin(e, "/", builtin_div);

  /* Definitions */
  lenv_add_builtin(e, "\\", builtin_lambda);
  lenv_add_builtin(e, "def", builtin_def);
  lenv_add_builtin(e, "=",   builtin_put);

  /* Comparison Functions */
  lenv_add_builtin(e, "if", builtin_if);
  lenv_add_builtin(e, "==", builtin_eq);
  lenv_add_builtin(e, "!=", builtin_ne);
  lenv_add_builtin(e, ">",  builtin_gt);
  lenv_add_builtin(e, "<",  builtin_lt);
  lenv_add_builtin(e, ">=", builtin_ge);
  lenv_add_builtin(e, "<=", builtin_le);

  /* String Functions */
  lenv_add_builtin(e, "load",  builtin_load);
  lenv_add_builtin(e, "error", builtin_error);
  lenv_add_builtin(e, "print", builtin_print);
  lenv_add_builtin(e, "slurp", builtin_slurp);
  lenv_add_builtin(e, "spit", builtin_spit);
  lenv_add_builtin(e, "seq", builtin_seq);
  lenv_add_builtin(e, "str", builtin_str);
  lenv_add_builtin(e, "char", builtin_char);
  lenv_add_builtin(e, "int", builtin_int);
}


int main(int argc, char** argv) {
  Number  = mpc_new("number");
  Symbol  = mpc_new("symbol");
  String  = mpc_new("string");
  Char    = mpc_new("char");
  Comment = mpc_new("comment");
  Sexpr   = mpc_new("sexpr");
  Qexpr   = mpc_new("qexpr");
  Expr    = mpc_new("expr");
  Lispy   = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
  "                                                     \
    number : /-?[0-9]+/ ;                               \
    symbol : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;         \
    string : /\"(\\\\.|[^\"])*\"/ ;                     \
    char   : /'.'/ ;                                  \
    comment: /;[^\\r\\n]*/ ;                            \
    sexpr  : '(' <expr>* ')' ;                          \
    qexpr  : '{' <expr>* '}' ;                          \
    expr   : <number> | <symbol> | <string> | <char>    \
           | <comment> | <sexpr> | <qexpr> ;            \
    lispy  : /^/ <expr>* /$/ ;                          \
  ",
  Number, Symbol, String, Char, Comment, Sexpr, Qexpr, Expr, Lispy);

  lenv* e = lenv_new();
  lenv_add_builtins(e);

  lval* core = lval_add(lval_sexpr(), lval_str("core.lspy"));
  lval* x = builtin_load(e, core);
  if (x->type == LVAL_ERR) { lval_println(x); }

  if(argc == 1) {

    puts("Lispy Version 0.0.0.0.2");
    puts("Press Ctrl+c to Exit\n");

    while (1) {

      char* input = readline("lispy> ");
      add_history(input);

      /* Attempt to parse the user input */
      mpc_result_t r;
      if (mpc_parse("<stdin>", input, Lispy, &r)) {
        /* On success print and delete the AST */
        mpc_ast_print(r.output);
        lval* x = lval_eval(e, lval_read(r.output));
        lval_println(x);
        lval_del(x);
        mpc_ast_delete(r.output);
      } else {
        /* Otherwise print and delete the Error */
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
      }

      free(input);
    }

    /* Undefine and delete our parsers */
  }

  if (argc >= 2) {

    for (int i = 1; i < argc; i++) {

      /* Argument list with a single argument, the filename */
      lval* args = lval_add(lval_sexpr(), lval_str(argv[i]));

      /* Pass to builtin load and get the result */
      lval* x = builtin_load(e, args);

      /* If the result is an error be sure to print it */
      if (x->type == LVAL_ERR) { lval_println(x); }

      lval_del(x);
    }
  }

  lenv_del(e);
  mpc_cleanup(9, Number, Symbol, String, Char, Comment, Sexpr, Qexpr, Expr, Lispy);

  return 0;
}
