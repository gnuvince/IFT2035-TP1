/*
 * tp1.c
 *
 *  Created on: Sep 27, 2010
 *      Author: eric
 */

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
	Token value;
	List *next;
};

struct Expr {
	enum { num, expr } type;
	union {
		Number number;
		struct BinaryOperator {
			Operator operator;
			struct Expr left, right;
		} expression;
	} value;
};
