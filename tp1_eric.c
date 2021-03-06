/*
 * tp1.c
 *
 *  Created on: Sep 27, 2010
 *      Author: eric & vince
 */

#include <stdio.h>
#include <stdlib.h>

char *ps_operators[4] = {"add", "sub", "mul", "div"};

enum Operator {
	op_add, op_sub, op_mul, op_div
};

enum ErrorCode {
	ec_ok, ec_invalid_syntax, ec_empty_expression,
	ec_div_zero, ec_invalid_symbol, ec_eof
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

int ListLen(struct List *list) {
	int len = 0;
	struct Node *node = list->head;
	while (node != NULL) {
		len += 1;
		node = node->next;
	}
	return len;
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


char * OperatorToPS(enum Operator op) {
    return ps_operators[op];
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

    while (1) {
        c = getchar();
        if (c == EOF) return ec_eof;
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

        if (c == '\n') break;
    }

    return ec_ok;
}

struct AST {
	struct Expr *root;
};

struct AST *ASTNew() {
	struct AST *ast = malloc(sizeof(struct AST));
	ast->root = NULL;
	return ast;
}

struct Expr *ExprNew() {
	struct Expr *node = malloc(sizeof(struct Expr));
	node->value.expression.left = NULL;
	node->value.expression.right = NULL;
	return node;
}

void ExprFree(struct Expr *node) {
	if (node->type == operand)
		free(node);
	else {
		ExprFree(node->value.expression.left);
		ExprFree(node->value.expression.right);
		free(node);
	}
}

void ASTFree(struct AST *ast) {
	if (ast->root != NULL) {
		ExprFree(ast->root);
		free(ast);
	}
}

void SchemeExprPrint(struct Expr *e) {
	if (e->type == operand)
		printf("%d", e->value.number);
	else {
		printf("(");
		printf("%c ", OperatorToChar(e->value.expression.operator));
		SchemeExprPrint(e->value.expression.left);
		printf(" ");
		SchemeExprPrint(e->value.expression.right);
		printf(")");
	}
}

void SchemePrint(struct AST *ast) {
	if (ast->root->type == operand)
		printf("%d\n", ast->root->value.number);
	else {
		SchemeExprPrint(ast->root);
		printf("\n");
	}
}

void CExprPrint(struct Expr *e) {
	int par_left = 0;	/* parentheses around left sub-expression? */
	int par_right = 0;	/* parentheses around right sub-expression? */
	enum Operator op_A, op_left, op_right;

	if (e->type == operand)
			printf("%d", e->value.number);
	else {
		op_A = e->value.expression.operator;
		/* Determine where we need parentheses */
		if (op_A == op_mul || op_A == op_div) {
			if (e->value.expression.left->type == expr) {
				op_left = e->value.expression.left->value.expression.operator;
				if (op_left == op_add || op_left == op_sub)
					par_left = 1;
			}
			if (e->value.expression.right->type == expr) {
				op_right = e->value.expression.right->value.expression.operator;
				if (op_right == op_add || op_right == op_sub)
					par_right = 1;
			}
		}
		if (par_left)
			printf("(");
		CExprPrint(e->value.expression.left);
		if (par_left)
			printf(")");
		printf(" ");
		printf("%c", OperatorToChar(op_A));
		printf(" ");
		if (par_right)
			printf("(");
		CExprPrint(e->value.expression.right);
		if (par_right)
			printf(")");
	}
}

void CPrint(struct AST *ast) {
	if (ast->root->type == operand)
		printf("%d\n", ast->root->value.number);
	else {
		CExprPrint(ast->root);
		printf("\n");
	}
}

void PSExprPrint(struct Expr *e) {
	if (e->type == operand)
		printf("%d", e->value.number);
	else {
		PSExprPrint(e->value.expression.left);
		printf(" ");
		PSExprPrint(e->value.expression.right);
		printf(" ");
		printf("%s", OperatorToPS(e->value.expression.operator));
	}
}

void PSPrint(struct AST *ast) {
	PSExprPrint(ast->root);
	printf("\n");
}

Number ExprEvaluate(struct Expr *e) {
	if (e->type == operand)
		return e->value.number;
	else {
		switch (e->value.expression.operator) {
		case (op_add):
			return ExprEvaluate(e->value.expression.left) +
				   ExprEvaluate(e->value.expression.right);

		case (op_sub):
			return ExprEvaluate(e->value.expression.left) -
				   ExprEvaluate(e->value.expression.right);

		case (op_mul):
			return ExprEvaluate(e->value.expression.left) *
				   ExprEvaluate(e->value.expression.right);

		case (op_div):
			return ExprEvaluate(e->value.expression.left) /
				   ExprEvaluate(e->value.expression.right);
		}
	}
	return 0;
}

void ASTEvaluate(struct AST *ast) {
	printf("%d\n", ExprEvaluate(ast->root));
}

void Report(struct AST *ast) {
	SchemePrint(ast);
	CPrint(ast);
	PSPrint(ast);
	ASTEvaluate(ast);
	printf("\n");
}

enum ErrorCode getexpr(struct List *tokens, struct Expr *subtree) {
	struct Token token;
	enum ErrorCode error;

	/* getexpr should not be called with an empty token list */
	if (tokens->head == NULL)
		return ec_invalid_syntax;

	/* consume 1 token */
	token = tokens->head->value;
	tokens->head = tokens->head->next;

	/* process the token */
	if (token.type == num) {	/* operand => leaf*/
		subtree->type = operand;
		subtree->value.number = token.value.number;
	} else {					/* operator => internal node */
		subtree->type = expr;
		subtree->value.expression.operator = token.value.operator;

		/* process right subtree */
		subtree->value.expression.right = ExprNew();
		error = getexpr(tokens, subtree->value.expression.right);
		if (error != ec_ok) return error;

		/* process left subtree */
		subtree->value.expression.left = ExprNew();
		error = getexpr(tokens, subtree->value.expression.left);
		if (error != ec_ok) return error;
	}
	return ec_ok;
}

enum ErrorCode ASTize(struct List *tokens, struct AST *ast) {
	int length = ListLen(tokens);
	enum ErrorCode error;

	if (length == 0)
		return ec_empty_expression;
	if (length == 1 && tokens->head->value.type != num)
		return ec_invalid_syntax;
	if (tokens->head->value.type == num && length > 1)
		return ec_invalid_syntax;

	/* recursively build the AST */
	error = getexpr(tokens, ast->root);
	if (error != ec_ok)
		return error;
	if (ListLen(tokens) != 0)
		return ec_invalid_syntax;

	return ec_ok;
}

void ExprPrint (struct Expr *expr) {
	if (expr->type == operand)
		printf("%d ", expr->value.number);
	else {
		ExprPrint(expr->value.expression.left);
		ExprPrint(expr->value.expression.right);
		printf("%c ", OperatorToChar(expr->value.expression.operator));
	}
}

void ASTprint (struct AST *ast) {
	if (ast != NULL) {
		ExprPrint(ast->root);
		printf("\n");
	}
}

int main(void) {
    struct List *tokens;
    /* struct Node *n; */
    enum ErrorCode tokenize_error, astize_error;
	struct AST *ast;
	struct Expr *expr;
	int completed = 0;

	while (!completed) {
		tokens = ListNew();
		tokenize_error = Tokenize(tokens);

		switch (tokenize_error) {
		case ec_ok:
			/* ASTizing */
			ast = ASTNew();
			expr  = ExprNew();
			ast->root = expr;
			astize_error = ASTize(tokens, ast);
			Report(ast);

			switch (astize_error) {
			case ec_ok:
				break;

			case ec_invalid_syntax:
				printf("Invalid syntax.\n");
				break;

			case ec_empty_expression:
				printf("Empty expression provided.\n");
				break;

			case ec_div_zero:
			case ec_invalid_symbol:
			case ec_eof:
				break;
			}
			ASTFree(ast);
			break;

		case ec_invalid_symbol:
			printf("Invalid symbol.\n");
			break;

		case ec_div_zero:
			printf("Division by zero.\n");
			break;

		case ec_eof:
				completed = 1;
				printf("EOF\n");
				break;

		case ec_invalid_syntax:
		case ec_empty_expression:
			break;
		}

		ListFree(tokens);
	}

    return 0;
}
