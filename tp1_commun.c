#include <stdio.h>
#include <stdlib.h>

/* Enumeration for our four arithmetic operations: +, -, * and /. */
enum Operator {
	op_add, op_sub, op_mul, op_div
};


/* Enumeration of all the errors that can occur in this program. */
enum ErrorCode {
	ec_ok, ec_invalid_syntax, ec_empty_expression,
	ec_div_zero, ec_invalid_symbol, ec_eof
};

/* Just in case we decide to change from int to long. */
typedef int Number;


/*
  <expr>   ::= <number> | <expr> <expr> <op>
  <op>     ::= "+" | "-" | "*" | "/"
  <number> ::= <digit> <number>
  <digit>  ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"

  An expression represents either an integral value or a binary
  operation on two subexpressions.
 */
struct Expr {
	enum { operand, expr } type;
	union {
		Number number;
		struct BinaryOperator {
			enum Operator operator;
			struct Expr *left;
			struct Expr *right;
		} expression;
	} _;
};


/* Single element of a linked list. */
struct Node {
	struct Expr value;
	struct Node *next;
};


/* A linked list with a reference to the first node. */
struct Stack {
    struct Node *head;
};



/* Create a new stack on the heap and return a pointer to it. */
struct Stack *StackNew() {
    struct Stack *stack = malloc(sizeof(struct Stack));
    stack->head = NULL;
    return stack;
}


/* Call free on all the nodes of a Stack. */
void StackFree(struct Stack *stack) {
    /* Free every node in the stack. */
    while (stack->head != NULL) {
        struct Node *next = stack->head->next;
        free(stack->head);
        stack->head = next;
    }

    /* Free the actual stack pointer. */
    free(stack);
}

/* Returns 1 if the stack is empty, or 0 if the stack has at least on element. */
int StackIsEmpty(struct Stack *stack) {
    return stack->head == NULL;
}



/* Add an expression to the top of the stack. */
void StackPush(struct Stack *stack, struct Expr *e) {
    struct Node *new = malloc(sizeof(struct Node));
    new->value = *e;
    new->next = stack->head;
    stack->head = new;
}


/* Remove the top element of the stack and put it in out and return 1.
   If the stack is empty, return 0 and set out to NULL.
 */
int StackPop(struct Stack *stack, struct Expr *out) {
    if (StackIsEmpty(stack)) {
        out = NULL;
        return 0;
    }
    else {
        *out = stack->head->value;
        stack->head = stack->head->next;
        return 1;
    }
}



/* Test whether a character is a space, tab or newline. */
int isspace(int c) {
    return c == ' ' || c == '\t' || c == '\n';
}

/* Test whether a character is a digit. (Mimics isspace(c) from ctypes.h)  */
int isdigit(int c) {
    return c >= '0' && c <= '9';
}

/* Test whether a character is a +, -, * or / symbol. */
int isoperator(int c) {
    return c == '+'
        || c == '-'
        || c == '*'
        || c == '/';
}


/* Test whether a character is a valid symbol in our program. */
int isvalid(int c) {
    return isspace(c) || isdigit(c) || isoperator(c);
}


/* Return the integer value of a character. */
int digit_to_int(int c) {
    return c - '0';
}


/* Translate an enum Operator into its symbolic character (e.g. op_add -> +) */
char OperatorToChar(enum Operator op) {
    return "+-*/"[op];
}


/* Translate an enum Operator into its PostScript functions (e.g. op_add -> add) */
char * OperatorToPS(enum Operator op) {
    char *ps_operators[4] = {"add", "sub", "mul", "div"};
    return ps_operators[op];
}


/* Translate a character to its associated enum Operator member. */
enum Operator CharToOperator(char c) {
    switch (c) {
    case '+': return op_add;
    case '-': return op_sub;
    case '*': return op_mul;
    case '/': return op_div;
    }
    return '_'; /* Should be unreachable. */
}

/* Enumeration of the different states that the generator is in:
   - st_normal: At the beginning of the generation and when reading spaces
   - st_number: When reading digits
   - st_operator: When reading an operator character
 */
enum GeneratorState { st_normal, st_number, st_operator };



/* Read a string of characters from stdin and construct an AST.
   Store the final expression tree in out.
 */
enum ErrorCode GenerateAST(struct Stack *stack, struct Expr *out) {
    enum GeneratorState state = st_normal;
    int c; /* Character that was just read. */
    int number; /* The integer that is currently being read. */

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
                /* VINCERIC: Trouver où faire le free() */
                struct Expr *right = malloc(sizeof(struct Expr));
                struct Expr *left = malloc(sizeof(struct Expr));
                struct Expr *expression = malloc(sizeof(struct Expr));

                /* Error if there weren't (at least) two expressions on the stack. */
                if (!StackPop(stack, right) || !StackPop(stack, left))
                    return ec_invalid_syntax;

                expression->type = expr;
                expression->_.expression.operator = CharToOperator(c);
                expression->_.expression.left = left;
                expression->_.expression.right = right;

                StackPush(stack, expression);
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
                struct Expr *expression = malloc(sizeof(struct Expr));
                expression->type = operand;
                expression->_.number = number;
                StackPush(stack, expression);

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

    if (StackPop(stack, out)) {
        /* No more elements in the stack, expression consummed entirely. */
        if (StackIsEmpty(stack))
            return ec_ok;
        else
            return ec_invalid_syntax;
    }
    return ec_empty_expression;
}



void SchemeExprPrint(struct Expr *e) {
	if (e->type == operand)
		printf("%d", e->_.number);
	else {
		printf("(");
		printf("%c ", OperatorToChar(e->_.expression.operator));
		SchemeExprPrint(e->_.expression.left);
		printf(" ");
		SchemeExprPrint(e->_.expression.right);
		printf(")");
	}
}

void SchemePrint(struct Expr *root) {
    SchemeExprPrint(root);
    printf("\n");
}

void CExprPrint(struct Expr *e) {
	int par_left = 0;	/* parentheses around left sub-expression? */
	int par_right = 0;	/* parentheses around right sub-expression? */
	enum Operator op_top, op_left, op_right;

	if (e->type == operand)
        printf("%d", e->_.number);
	else {
		op_top = e->_.expression.operator;
		/* Determine where we need parentheses */
		if (op_top == op_mul || op_top == op_div) {
			if (e->_.expression.left->type == expr) {
				op_left = e->_.expression.left->_.expression.operator;
				if (op_left == op_add || op_left == op_sub)
					par_left = 1;
			}
			if (e->_.expression.right->type == expr) {
				op_right = e->_.expression.right->_.expression.operator;
				if (op_right == op_add || op_right == op_sub)
					par_right = 1;
			}
		}
		if (par_left)
			printf("(");
		CExprPrint(e->_.expression.left);
		if (par_left)
			printf(")");
		printf(" ");
		printf("%c", OperatorToChar(op_top));
		printf(" ");
		if (par_right)
			printf("(");
		CExprPrint(e->_.expression.right);
		if (par_right)
			printf(")");
	}
}

void CPrint(struct Expr *root) {
    CExprPrint(root);
    printf("\n");
}

void PSExprPrint(struct Expr *e) {
	if (e->type == operand)
		printf("%d", e->_.number);
	else {
		PSExprPrint(e->_.expression.left);
		printf(" ");
		PSExprPrint(e->_.expression.right);
		printf(" ");
		printf("%s", OperatorToPS(e->_.expression.operator));
	}
}

void PSPrint(struct Expr *root) {
	PSExprPrint(root);
	printf("\n");
}



Number ExprEvaluate(struct Expr *e, enum ErrorCode *ec) {
    Number left, right;

	if (e->type == operand)
		return e->_.number;
	else {
		switch (e->_.expression.operator) {
		case (op_add):
			return ExprEvaluate(e->_.expression.left, ec) +
                ExprEvaluate(e->_.expression.right, ec);

		case (op_sub):
			return ExprEvaluate(e->_.expression.left, ec) -
                ExprEvaluate(e->_.expression.right, ec);

		case (op_mul):
			return ExprEvaluate(e->_.expression.left, ec) *
                ExprEvaluate(e->_.expression.right, ec);

		case (op_div):
            left = ExprEvaluate(e->_.expression.left, ec);
            right = ExprEvaluate(e->_.expression.right, ec);

            if (right == 0) {
                *ec = ec_div_zero;
                return 1;
            }
            else
                return left / right;
		}
	}
	return 0;
}


void Report(struct Expr *root) {
	SchemePrint(root);
	CPrint(root);
	PSPrint(root);
}


int main(void) {
    struct Stack *stack = StackNew();
    struct Expr expression;
    enum ErrorCode generation_error;
    enum ErrorCode evaluate_error = ec_ok;
    Number result;


    /*
    printf("%d\n", StackIsEmpty(stack));
    expression.type = operand;
    expression._.number = 3;
    StackPush(stack, &expression);
    printf("%d\n", StackIsEmpty(stack));
    StackPop(stack, &expression);
    printf("%d\n", StackIsEmpty(stack));

    return 0;
    */
    generation_error = GenerateAST(stack, &expression);
    result = ExprEvaluate(&expression, &evaluate_error);

    switch (generation_error) {
    case ec_ok:
        if (evaluate_error == ec_div_zero)
            printf("Division par zéro.\n");
        else {
            Report(&expression);
            printf("%d\n", result);
        }
        break;

    case ec_invalid_symbol:
        printf("Symbole invalide.\n");
        break;

    case ec_div_zero:
        printf("Division par zéro.\n");
        break;

    case ec_eof:
        printf("EOF\n");
        break;

    case ec_invalid_syntax:
        printf("Erreur de syntaxe.\n");
        break;

    case ec_empty_expression:
    	break;
    }


    StackFree(stack);
    return 0;
}


