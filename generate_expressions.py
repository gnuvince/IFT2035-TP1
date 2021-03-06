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


def invalid_symbol():
    s = expression()
    i = random.randint(0, len(s) - 1) 
    c = chr(random.randint(ord('0'), ord('z')))
    return s[:i] + c + s[i:]

def invalid_syntax():
    expr = expression().split()
    random.shuffle(expr)
    return ' '.join(expr)

for i in xrange(number_of_expressions):
    print invalid_syntax()
