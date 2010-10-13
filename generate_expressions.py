import sys
import random

OPERATORS = "+-*/"

valid_expressions = sys.argv[1] == "1"
number_of_expressions = int(sys.argv[2])
max_expression_length = int(sys.argv[3])


def expression():
    expr = []
    expr.append(str(random.randint(0, 1000)))
    expr.append(str(random.randint(0, 1000)))
    expr.append(random.choice(OPERATORS))

    for i in xrange(random.randint(0, max_expression_length)):
        expr.append(str(random.randint(0, 1000)))
        expr.append(random.choice(OPERATORS))

    return ' '.join(expr)


for i in xrange(number_of_expressions):
    print expression()
