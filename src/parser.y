%code requires {
    #include "generator.h"
    #include <stdarg.h>
}

%define api.value.type union
%token <char *> n LITERAL COMMENT PROLOGUE EPILOGUE

%type <record_t *> record
%type <list_t *> list items
%type <char *> name

%token EOL COMMA SWITCH EMPTY

%code provides {
	int yylex(void);
	int yyparse(void);
	void yyrestart(FILE *);
	int yylex_destroy(void);

	extern list_t records;
	extern list_t epilogue;
	extern list_t prologue;
}

%destructor { free($$); } <char *>

%start document

%%

document: prologue grammar epilogue

prologue: %empty
      | prologue PROLOGUE { list_append(&prologue, node_new($2)); }
      ;

epilogue: SWITCH
     | epilogue EPILOGUE { list_append(&epilogue, node_new($2)); }
     ;

grammar: SWITCH
   | grammar EOL
   | grammar record { list_append(&records, node_new((char *)$2)); }
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

