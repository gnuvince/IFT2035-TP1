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
	ec_ok, ec_invalid_syntax, ec_div_zero, ec_invalid_symbol
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



enum TokenizerState { st_normal, st_number, st_operator };



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

int isvalid(int c) {
    return isspace(c) || isdigit(c) || isoperator(c);
}


int digit_to_int(int c) {
    return c - '0';
}


char OperatorToChar(enum Operator op) {
    return "+-*/"[op];
}


enum Operator CharToOperator(char c) {
    switch (c) {
    case '+': return op_add;
    case '-': return op_sub;
    case '*': return op_mul;
    case '/': return op_div;
    }
    return '_'; /* Should be unreachable. */
}


enum ErrorCode Tokenize(struct List *tokens) {
    enum TokenizerState state = st_normal;
    int c;
    int number;

    while ((c = getchar()) != '\n') {
        if (c == EOF) exit(0);
        if (!isvalid(c)) return ec_invalid_symbol;

        switch (state) {
        case st_normal:
            if (isdigit(c)) {
                number = digit_to_int(c);
                state = st_number;
            }
            else if (isoperator(c)) {
                struct Token token;
                token.type = op;
                token.value.operator = CharToOperator(c);
                ListAdd(tokens, &token);
            }
            else if (isspace(c)) {
                /* Stay in the same state. */
            }
            break;

        case st_number:
            if (isdigit(c)) {
                number = 10*number + digit_to_int(c);
            }
            else if (isoperator(c)) {
                return ec_invalid_syntax;
            }
            else if (isspace(c)) {
                struct Token token;
                token.type = num;
                token.value.number = number;
                ListAdd(tokens, &token);

                state = st_normal;
            }
            break;

        case st_operator:
            if (isdigit(c) || isoperator(c)) {
                return ec_invalid_syntax;
            }
            else if (isspace(c)) {
                state = st_normal;
            }
            break;

        default:
            break;
        }
    }

    return ec_ok;
}



int main(void) {
    struct List *tokens = ListNew();
    struct Node *n;
    enum ErrorCode error;

    error = Tokenize(tokens);

    switch (error) {
    case ec_ok:
        for (n = tokens->head; n != NULL; n = n->next) {
            if ((*n).value.type == op)
                printf("%c ", OperatorToChar((*n).value.value.operator));
            else
                printf("%d ", (*n).value.value.number);
        }
        printf("\n");
        break;

    case ec_invalid_syntax:
        printf("Invalid syntax.\n");
        break;

    case ec_invalid_symbol:
        printf("Invalid symbol.\n");
        break;

    case ec_div_zero:
        printf("Division by zero.\n");
        break;
    }


    ListFree(tokens);

    return 0;
}
