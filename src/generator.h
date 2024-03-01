#ifndef ALEC_PARSER_H
#define ALEC_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylineno;
void yyerror(char *s, ...);

typedef struct node node_t;
typedef struct list list_t;

struct node {
    char *data;
    node_t *next;
};

struct list {
    node_t *head;
    node_t *tail;
};

struct record {
    char *name;
    list_t *args;
    list_t *rules;
    list_t *recipe;
};

typedef void (*free_f)(void *);
typedef int (*cmp_f)(const void *, const void *);

int scmp(const void *a, const void *b);

node_t *node_new(char *data);

list_t *list_new(char *data);
void list_free(list_t *l, free_f free_data);
void list_append(list_t *l, node_t *n);

int list_find(list_t *l, void *data, cmp_f cmp);

struct record *record_new(char *name, list_t *args, list_t *rules, list_t *recipe);
void record_free(void *rp);

void record_dupes(list_t *d, list_t *l);

void record_print_template(const struct record *r, int dup);
void record_print_function(const struct record *r);
void record_print_dupes(const list_t *l);

#endif
