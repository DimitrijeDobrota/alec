%{
    #include "generator.h"
    #include <stdarg.h>

    #define MALLOC(x) do {                          \
        x = malloc(sizeof(*x));                     \
        if(!x) yyerror("out of space"), exit(1);    \
    } while(0);

	int yylex(void);
	int yyparse(void);
	void yyrestart(FILE *);
	int yylex_destroy(void);

	list_t records = { 0 };
	list_t after = { 0 };
	list_t before = { 0 };
%}

%union {
    struct record *r;
    list_t *l;
    char *n;
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


    // print before section
	for(node_t *p = before.head; p; p = p->next) {
		printf("%s", p->data);
	}

    list_t dupes = { 0 };
    record_dupes(&dupes, &records);

    printf("\n/* Template compile-time variables */\n\n");
    record_print_dupes(&dupes);
	for(node_t *p = records.head; p; p = p->next) {
        const struct record * r = (const struct record *)p->data;
		record_print_template(r, list_find(&dupes, r->name, scmp));
	}

    printf("\n/* Run-time functions */\n\n");
	for(node_t *p = records.head; p; p = p->next) {
        const struct record * r = (const struct record *)p->data;
		record_print_function(r);
	}

    // print after section
	for(node_t *p = after.head; p; p = p->next) {
		printf("%s", p->data);
	}

	list_free(&dupes, 0);

	list_free(&before, free);
	list_free(&records, record_free);
	list_free(&after, free);

	yylex_destroy();
}

node_t *node_new(char *data) {
    node_t *n;

    MALLOC(n);
    *n = (node_t) {
        .data = data,
        .next = NULL,
    };

    return n;
}

list_t *list_new(char *data) {
    list_t *l;

    MALLOC(l);
    *l = (list_t) { 0 };
    if(data) list_append(l, node_new(data));

    return l;
}

void list_free(list_t *l, void (*free_data)(void *)) {
    if(!l) return;

	node_t *c = l->head, *t;
	while(c) {
		t = c;
		c = c->next;
		if(free_data) free_data((void *)t->data);
		free(t);
	}
}

void list_append(list_t *l, node_t *n) {
    if(!l->head) l->head = l->tail = n;
    else l->tail = l->tail->next = n;
}

struct record *record_new(char *name, list_t *args, list_t *rules, list_t *recipe){
	struct record* rec;

    MALLOC(rec);
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
	if(r->args) list_free(r->args, free), free(r->args);
	if(r->rules) list_free(r->rules, free), free(r->rules);
	if(r->recipe) list_free(r->recipe, free), free(r->recipe);
	free(r->name);
	free(r);
}

int list_find(list_t *l, void *data, cmp_f cmp) {
    node_t *c = l->head;
    while(c) {
        if(!cmp(c->data, data)) return 1;
        c = c->next;
    }
    return 0;
}

void record_print_dupes(const list_t *l) {
    node_t *c = l->head;
    while(c) {
        printf("template <auto... val> static const char *%s_v;\n", c->data);
        c = c->next;
    }
    printf("\n");
}

void record_print_function(const struct record *r) {
	list_t dummy = { 0 };

    if(!r->recipe) { // comment
        printf("%s\n\n", r->name);
        return;
    }

	printf("static constexpr auto %s(", r->name);
    if(r->args) {
        for(node_t *p = r->args->head; p; p = p->next) {
            list_append(&dummy, node_new(strchr(p->data, ' ') + 1));
            printf("%s", p->data);
            if(p->next) printf(", ");
        }
    }
	printf(") {\n");

	if(r->rules) {
		for(node_t *p = dummy.head; p; p = p->next) {
			printf("\tassert(");
			for(node_t *q = r->rules->head; q; q = q->next) {
				printf("%s(%s)", q->data, p->data);
				if(q->next) printf(" && ");
			}
			printf(");\n");
		}
	}

    if(r->args) {
        printf("\treturn details::helper::make(");
        for(node_t *p = r->recipe->head; p; p = p->next) {
            printf("%s", p->data);
            if(p->next) printf(", ");
        }
        printf(")");
    } else {
        printf("\treturn %s_v", r->name);
    }
    printf(";\n}\n\n");

	list_free(&dummy, NULL);
}

void record_print_template(const struct record *r, int dup) {
	list_t dummy = { 0 };

    if(!r->recipe) { // comment
        printf("%s\n\n", r->name);
        return;
    }

    if(r->args) {
        printf("template <");
        for(node_t *p = r->args->head; p; p = p->next) {
            list_append(&dummy, node_new(strchr(p->data, ' ') + 1));
            printf("%s", p->data);
            if(p->next) printf(", ");
        }
        printf(">\n");
    }

	if(r->rules) {
		printf("\trequires ");
		for(node_t *p = dummy.head; p; p = p->next) {
			for(node_t *q = r->rules->head; q; q = q->next) {
				printf("%s_v<%s>", q->data, p->data);
				if(q->next) printf(" && ");
			}
			if(p->next) printf(" && ");
		}
		printf("\n");
	}

	printf("static constexpr auto %s_v", r->name);
    if(dup) {
        printf("<");
        for(node_t *p = dummy.head; p; p = p->next) {
            printf("%s", p->data);
            if(p->next) printf(", ");
        }
        printf(">");
    }

    if(r->recipe->head && r->recipe->head->data[0] == '"') {
        printf("\n\t = details::escape_literal<%s>;", r->recipe->head->data);
    } else {
        printf("\n\t = details::escape<");
        for(node_t *p = r->recipe->head; p; p = p->next) {
            printf("%s", p->data);
            if(p->next) printf(", ");
        }
        printf(">;");
    }
    printf("\n\n");

	list_free(&dummy, NULL);
}

int scmp(const void *a, const void *b) {
    return strcmp((const char *)a, (const char *)b);
}

void record_dupes(list_t *d, list_t *l) {
	list_t s = { 0 };

	for(node_t *p = records.head; p; p = p->next) {
		const struct record * r = (const struct record *)p->data;
        if(!list_find(&s, r->name, scmp)) list_append(&s, node_new(r->name));
        else if(!list_find(d, r->name, scmp)) list_append(d, node_new(r->name));
	}

	list_free(&s, NULL);
}

void yyerror(char *s, ...) {
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

