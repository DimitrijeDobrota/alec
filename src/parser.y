%code requires {
    #include "generator.h"
    #include <stdarg.h>
}

%union {
    struct record *r;
    list_t *l;
    char *n;
}

%code provides {
	int yylex(void);
	int yyparse(void);
	void yyrestart(FILE *);
	int yylex_destroy(void);

	extern list_t records;
	extern list_t after;
	extern list_t before;
}

%token <n> LITERAL COMMENT BEFORE AFTER
%token EOL COMMA SWITCH EMPTY

%type <r> record
%type <l> list items
%type <n> name

%start document

%%

document: before mid after

before: %empty
      | before BEFORE { list_append(&before, node_new($2)); }
      ;

after: SWITCH
     | after AFTER { list_append(&after, node_new($2)); }
     ;

mid: SWITCH
   | mid EOL
   | mid record { list_append(&records, node_new((char *)$2)); }
   ;

record: name list list list { $$ = record_new($1, $2, $3, $4); }
      | COMMENT             { $$ = record_new($1, NULL, NULL, NULL); }
      ;

name: LITERAL EOL   { $$ = $1; }
    ;

list: EMPTY       { $$ = NULL; }
    | items EOL   { $$ = $1; }
    ;

items: LITERAL             { $$ = list_new($1); }
     | items COMMA LITERAL { list_append($1, node_new($3)); $$ = $1; }
     ;

%%

void yyerror(char *s, ...) {
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

