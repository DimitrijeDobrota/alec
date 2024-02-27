%{
    #include "generator.h"
    #include <stdarg.h>

	int yylex(void);
	int yyparse(void);
	void yyrestart(FILE *);
	int yylex_destroy(void);

	struct list *records;
%}

%union {
    struct record *r;
    struct list *l;
    char *n;
}

%token <n> LITERAL
%token EOL COMMA

%type <r> record
%type <l> list
%type <n> name

%start document

%%

document: %empty
        | EOL document 
        | record document { records = list_new((char *)$1, records); }
        ;

record: name list list list { $$ = record_new($1, $2, $3, $4); }
      ;

name: LITERAL EOL   { $$ = $1; }
    ;

list: EOL                { $$ = NULL; }
    | LITERAL EOL        { $$ = list_new($1, NULL); }
    | LITERAL COMMA list { $$ = list_new($1, $3); }
    ;

%%

#ifdef YYDEBUG
	int yydebug = 1;
#endif

int main(const int argc, char *argv[]) {
    if(argc < 2) {
        yyparse();
        return 0;
    }

    for(int i = 1; i < argc; i++) {
        FILE *f = fopen(argv[i], "r");
        if(!f) {
            perror(argv[1]);
            return -1;
        }

        yyrestart(f);
        yyparse();

        fclose(f);
    }

	for(struct list *p = records; p; p = p->next) {
		record_print_function((const struct record *)p->data);
	}

	for(struct list *p = records; p; p = p->next) {
		record_print_template((const struct record *)p->data);
	}

	list_free(records, record_free);
	yylex_destroy();
}

struct list *list_new(char *data, struct list *list) {
    struct list *l = malloc(sizeof(struct list));

    if(!l) {
        yyerror("out of space");
        exit(1);
    }

    *l = (struct list) {
        .data = data,
        .next = list,
    };

    return l;
}

void list_free(struct list *l, void (*free_data)(void *)) {
	struct list *t;
	while(l) {
		t = l;
		l = l->next;
		if(free_data) free_data((void *)t->data);
		free(t);
	}
}

struct record *record_new(char *name, struct list *args, struct list *rules, struct list *recipe){
	struct record* rec = malloc(sizeof(struct record));

	if(!rec) {
		yyerror("out of space");
		exit(1);
	}

	*rec = (struct record) {
		.name = name,
		.args = args,
		.rules = rules,
		.recipe = recipe,
	};

	return rec;
}

void record_free(void *rp) {
	struct record *r = (struct record *)rp;
	list_free(r->args, free);
	list_free(r->rules, free);
	list_free(r->recipe, free);
	free(r->name);
	free(r);
}

void record_print_function(const struct record *r) {
	struct list dummy, *c = &dummy;
	printf("static constexpr auto %s(", r->name);
	for(struct list *p = r->args; p; p = p->next) {
		c = c->next = list_new(strchr(p->data, ' ') + 1, NULL);
		printf("%s", p->data);
		if(p->next) printf(", ");
	}
	printf(") {\n");

	if(r->rules) {
		for(struct list *p = dummy.next; p; p = p->next) {
			printf("\t assert(");
			for(struct list *q = r->rules; q; q = q->next) {
				printf("%s(%s)", q->data, p->data);
				if(q->next) printf(" && ");
			}
			printf(");\n");
		}
	}

	printf("\treturn details::helper::make(");
	for(struct list *p = r->recipe; p; p = p->next) {
		printf("%s", p->data);
		if(p->next) printf(", ");
	}
	printf(");\n}\n\n");

	list_free(dummy.next, NULL);
}

void record_print_template(const struct record *r) {
	struct list dummy, *c = &dummy;

	printf("template <");
	for(struct list *p = r->args; p; p = p->next) {
		c = c->next = list_new(strchr(p->data, ' ') + 1, NULL);
		printf("%s", p->data);
		if(p->next) printf(", ");
	}
	printf(">\n");

	if(r->rules) {
		printf("\trequires ");
		for(struct list *p = dummy.next; p; p = p->next) {
			for(struct list *q = r->rules; q; q = q->next) {
				printf("%s_v(%s)", q->data, p->data);
				if(q->next) printf(" && ");
			}
			if(p->next) printf(" && ");
		}
		printf("\n");
	}

	printf("static constexpr auto %s_v\n\t = details::escape<", r->name);
	for(struct list *p = r->recipe; p; p = p->next) {
		printf("%s", p->data);
		if(p->next) printf(", ");
	}
	printf(">;\n\n");

	list_free(dummy.next, NULL);
}

void yyerror(char *s, ...) {
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

