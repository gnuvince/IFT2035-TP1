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

/* Add an expression to the top of the stack. */
void StackPush(struct Stack *stack, struct Expr *e) {
    struct Node *new = malloc(sizeof(struct Node));
    new->value = *e;
    new->next = stack->head;
    stack->head = new;
}


/* Remove the top element of the stack and return it. */
struct Expr StackPop(struct Stack *stack) {
    struct Expr e = stack->head->value;
    stack->head = stack->head->next;
    return e;
}


/* Return the number of expressions on the stack. */
int StackLength(struct Stack *list) {
	int len = 0;
	struct Node *node = list->head;
	while (node != NULL) {
		len += 1;
		node = node->next;
	}
	return len;
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


/* Read a string of characters from stdin and construct an AST. */
enum ErrorCode GenerateAST(struct List *tokens) {
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





int main(void) {
    return 0;
}


