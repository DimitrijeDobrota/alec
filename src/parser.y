%{
    #include "generator.h"
    #include <stdarg.h>

	int yylex(void);
	int yyparse(void);
	void yyrestart(FILE *);
	int yylex_destroy(void);

	struct list *records = NULL;
	struct list *before = NULL;
	struct list *after = NULL;
%}

%union {
    struct record *r;
    struct list *l;
    char *n;
}

%token <n> LITERAL COMMENT BEFORE AFTER
%token EOL COMMA SWITCH

%type <r> record
%type <l> list
%type <n> name

%start document

%%

document: before SWITCH mid SWITCH after

before: %empty
      | BEFORE before { before = list_new($1, before); }

after: %empty
     | AFTER after { after = list_new($1, after); }

mid: %empty
   | EOL mid 
   | record mid { records = list_new((char *)$1, records); }
   | COMMENT mid {
       records = list_new((char *)record_new(
           $1, NULL, NULL, NULL
       ), records);
   }
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


    // print before section
	for(struct list *p = before; p; p = p->next) {
		printf("%s", p->data);
	}

    struct list *dupes = record_dupes(records);

    printf("\n/* Template compile-time variables */\n\n");
    record_print_dupes(dupes);
	for(struct list *p = records; p; p = p->next) {
        const struct record * r = (const struct record *)p->data;
		record_print_template(r, list_find(dupes, r->name, scmp));
	}

    printf("\n/* Run-time functions */\n\n");
	for(struct list *p = records; p; p = p->next) {
        const struct record * r = (const struct record *)p->data;
		record_print_function(r);
	}

    // print after section
	for(struct list *p = after; p; p = p->next) {
		printf("%s", p->data);
	}

	list_free(dupes, 0);

	list_free(before, free);
	list_free(records, record_free);
	list_free(after, free);

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

int list_find(struct list *l, void *data, cmp_f cmp) {
    while(l) {
        if(!cmp(l->data, data)) return 1;
        l = l->next;
    }
    return 0;
}

void record_print_dupes(const struct list *l) {
    while(l) {
        printf("template <auto... val> static const char *%s_v;\n", l->data);
        l = l->next;
    }
    printf("\n");
}

void record_print_function(const struct record *r) {
	struct list dummy = { 0 }, *c = &dummy;

    if(!r->recipe) { // comment
        printf("%s\n\n", r->name);
        return;
    }

	printf("static constexpr auto %s(", r->name);
	for(struct list *p = r->args; p; p = p->next) {
		c = c->next = list_new(strchr(p->data, ' ') + 1, NULL);
		printf("%s", p->data);
		if(p->next) printf(", ");
	}
	printf(") {\n");

	if(r->rules) {
		for(struct list *p = dummy.next; p; p = p->next) {
			printf("\tassert(");
			for(struct list *q = r->rules; q; q = q->next) {
				printf("%s(%s)", q->data, p->data);
				if(q->next) printf(" && ");
			}
			printf(");\n");
		}
	}

    if(r->args) {
        printf("\treturn details::helper::make(");
        for(struct list *p = r->recipe; p; p = p->next) {
            printf("%s", p->data);
            if(p->next) printf(", ");
        }
        printf(")");
    } else {
        printf("\treturn %s_v", r->name);
    }
    printf(";\n}\n\n");

	list_free(dummy.next, NULL);
}

void record_print_template(const struct record *r, int dup) {
	struct list dummy = { 0 }, *c = &dummy;

    if(!r->recipe) { // comment
        printf("%s\n\n", r->name);
        return;
    }

    if(r->args) {
        printf("template <");
        for(struct list *p = r->args; p; p = p->next) {
            c = c->next = list_new(strchr(p->data, ' ') + 1, NULL);
            printf("%s", p->data);
            if(p->next) printf(", ");
        }
        printf(">\n");
    }

	if(r->rules) {
		printf("\trequires ");
		for(struct list *p = dummy.next; p; p = p->next) {
			for(struct list *q = r->rules; q; q = q->next) {
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
        for(struct list *p = dummy.next; p; p = p->next) {
            printf("%s", p->data);
            if(p->next) printf(", ");
        }
        printf(">");
    }

    if(!r->recipe->next && r->recipe->data[0] == '"') {
        // string literal
        printf("\n\t = details::escape_literal<%s>;", r->recipe->data);
    } else {
        printf("\n\t = details::escape<");
        for(struct list *p = r->recipe; p; p = p->next) {
            printf("%s", p->data);
            if(p->next) printf(", ");
        }
        printf(">;");
    }
    printf("\n\n");

	list_free(dummy.next, NULL);
}

int scmp(const void *a, const void *b) {
    return strcmp((const char *)a, (const char *)b);
}

struct list *record_dupes(struct list *l) {
	struct list *s = NULL, *d = NULL;

	for(struct list *p = records; p; p = p->next) {
		const struct record * r = (const struct record *)p->data;
        if(!list_find(s, r->name, scmp)) s = list_new(r->name, s);
        else if(!list_find(d, r->name, scmp)) d = list_new(r->name, d);
	}

	list_free(s, NULL);
    return d;
}

void yyerror(char *s, ...) {
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

