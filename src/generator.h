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

struct list *list_new(char *data, struct list *list);
void list_free(struct list *l, void (*free_data)(void *));

struct record *record_new(char *name, struct list *args, struct list *rules, struct list *recipe);
void record_free(void *rp);

void record_print_function(const struct record *r);
void record_print_template(const struct record *r);

#endif
