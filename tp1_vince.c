/*
 * tp1.c
 *
 *  Created on: Sep 27, 2010
 *      Author: eric & vince
 */

#include <stdio.h>
#include <stdlib.h>

enum Operator {
	op_add, op_sub, op_mul, op_div
};

enum ErrorCode {
	ec_ok, ec_invalid_syntax, ec_div_zero
};

typedef int Number;

struct Token {
	enum { op, num } type;
	union {
		enum Operator operator;
		Number number;
	} value;
};


struct Node {
	struct Token value;
	struct Node *next;
};

struct List {
    struct Node *head;
};


struct Expr {
	enum { operand, expr } type;
	union {
		Number number;
		struct BinaryOperator {
			enum Operator operator;
			struct Expr *left;
			struct Expr *right;
		} expression;
	} value;
};



enum TokenizerState { tok_normal, tok_number, tok_operator };



/***
 * List manipulation functions.
 ***/
struct List *ListNew() {
    struct List *list = malloc(sizeof(struct List));
    list->head = NULL;
    return list;
}

void ListFree(struct List *list) {
    /* Free every node in the list. */
    while (list->head != NULL) {
        struct Node *next = list->head->next;
        free(list->head);
        list->head = next;
    }

    /* Free the actual list pointer. */
    free(list);
}

void ListAdd(struct List *list, struct Token *t) {
    struct Node *new = malloc(sizeof(struct Node));
    new->value = *t;
    new->next = list->head;
    list->head = new;
}


int isspace(int c) {
    return c == ' ' || c == '\t' || c == '\n';
}

int isdigit(int c) {
    return c >= '0' && c <= '9';
}

int isoperator(int c) {
    return c == '+'
        || c == '-'
        || c == '*'
        || c == '/';
}

int digit_to_int(int c) {
    return c - '0';
}

int main(void) {
    struct List *l = ListNew();
    struct Node *n;
    int i;

    struct Token t;
    for (i = 5; i > 0; --i) {
        t.type = num;
        t.value.number = i;
        ListAdd(l, &t);
    }

    t.type = op;
    t.value.operator = op_mul;
    ListAdd(l, &t);


    for (n = l->head; n != NULL; n = n->next) {
        if ((*n).value.type == num)
            printf("[NUM] %d\n", (*n).value.value.number);
        else
            printf("[OP ] %d\n", (*n).value.value.operator);
    }

    ListFree(l);

    return 0;
}
