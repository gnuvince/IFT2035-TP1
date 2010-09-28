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

struct List {
	struct Token value;
	struct List *next;
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

void printOp(int operator) {

	switch(operator) {
	case (op_add):
			printf("+"); break;
	case (op_sub):
			printf("-"); break;
	case (op_mul):
			printf("*"); break;
	case (op_div):
			printf("/"); break;
	}
}

void printList(struct List *L) {
	struct List *elt;

	elt = L;

	printf("\n[(");
	while (elt != NULL) {
		printf(" ");
		if (elt->value.type == op) {
			printOp(elt->value.value.operator);
		} else {
			printf("%d", elt->value.value.number);
		}
		elt = elt->next;
		if (elt != NULL)
			printf(",");
	}
	printf(" )]\n");
}

struct List * getList() {
	// On construit une liste a mano representant l'expression:
	// 31 4 - 8 5 * + 67 /
	// ==> [(/, 67, +, *, 5, 8, -, 4, 31)]

	struct List *L, *last;

	struct Token *token = malloc(sizeof(struct Token));
	token->type = op;
	token->value.operator = op_div;

	struct List *node = malloc(sizeof(struct List));
	node->value = *token;
	node->next = NULL;
	L = node;
	last = node;

	token->type = num;
	token->value.number = 67;
	node = malloc(sizeof(struct List));
	node->value = *token;
	node->next = NULL;
	last->next = node;
	last = node;

	token->type = op;
	token->value.number = op_add;
	node = malloc(sizeof(struct List));
	node->value = *token;
	node->next = NULL;
	last->next = node;
	last = node;

	token->type = op;
	token->value.number = op_mul;
	node = malloc(sizeof(struct List));
	node->value = *token;
	node->next = NULL;
	last->next = node;
	last = node;

	token->type = num;
	token->value.number = 5;
	node = malloc(sizeof(struct List));
	node->value = *token;
	node->next = NULL;
	last->next = node;
	last = node;

	token->type = num;
	token->value.number = 8;
	node = malloc(sizeof(struct List));
	node->value = *token;
	node->next = NULL;
	last->next = node;
	last = node;

	token->type = op;
	token->value.number = op_sub;
	node = malloc(sizeof(struct List));
	node->value = *token;
	node->next = NULL;
	last->next = node;
	last = node;

	token->type = num;
	token->value.number = 4;
	node = malloc(sizeof(struct List));
	node->value = *token;
	node->next = NULL;
	last->next = node;
	last = node;

	token->type = num;
	token->value.number = 31;
	node = malloc(sizeof(struct List));
	node->value = *token;
	node->next = NULL;
	last->next = node;
	last = node;

	free(token);
	return L;
}

void freeList(struct List *L) {
	struct List *elt, *next;

	elt = L;

	while (elt != NULL) {
		next = elt->next;
		free(elt);
		elt = next;
	}
}

int main() {
	struct List *L;

	L = getList();
	printList(L);
	freeList(L);

	return 0;
}
