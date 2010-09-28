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


struct List *ListNew() {
    struct List *list = malloc(sizeof(struct List));
    list->head = NULL;
    return list;
}

void ListFree(struct List *list) {
    while (list->head != NULL) {
        struct Node *next = list->head->next;
        free(list->head);
        list->head = next;
    }
    free(list);
}

void ListAdd(struct Token *t, struct List *list) {
    struct Node *new = malloc(sizeof(struct Node));
    new->value = *t;
    new->next = list->head;
    list->head = new;
}

/*
enum ErrorCode Tokenize(struct List *tokens) {
    int c;
    int currentNumber;
    enum TokenizerState state = tok_normal;

    while (1) {
        c = getchar();

        switch (state) {
        case tok_normal:
            if (isdigit(c)) {
                currentNumber = digit_to_int(c);
                state = tok_number;
            }
            else if (isoperator(c))
                state = tok_operator;
            break;

        case tok_number:
            if (isdigit(c)) {
                currentNumber = currentNumber * 10 + digit_to_int(c);
            }
            else {
                printf("%d\n", currentNumber);
                state = tok_normal;
            }
            break;

        case tok_operator:
            if (isspace(c)) {
                state = tok_normal;
            }
            break;

        default:
            break;
        }
    }
}
*/

int main(void) {
    struct List *l = ListNew();
    struct Node *n;
    int i;

    for (i = 10; i > 0; --i) {
        struct Token t;
        t.type = num;
        t.value.number = i;
        ListAdd(&t, l);
    }


    for (n = l->head; n != NULL; n = n->next) {
        printf("%d\n", (*n).value.value.number);
    }

    ListFree(l);

    return 0;
}
