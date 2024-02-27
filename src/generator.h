#ifndef ALEC_PARSER_H
#define ALEC_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylineno;
void yyerror(char *s, ...);

struct list {
    char *data;
    struct list *next;
};

struct record {
    char *name;
    struct list *args;
    struct list *rules;
    struct list *recipe;
};

typedef void(*free_f)(void *);
typedef int(*cmp_f)(const void *, const void *);

int scmp(const void *a, const void *b);

struct list *list_new(char *data, struct list *list);
void list_free(struct list *l, free_f free_data);

int list_find(struct list *l, void *data, cmp_f cmp);

struct record *record_new(char *name, struct list *args, struct list *rules, struct list *recipe);
void record_free(void *rp);

struct list *record_dupes(struct list *l);

void record_print_template(const struct record *r, int dup);
void record_print_function(const struct record *r);
void record_print_dupes(const struct list *l);

#endif
