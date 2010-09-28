CC=gcc

all: vince eric

vince:
	${CC} -Wall -Wextra -pedantic -std=c89 -o vince tp1_vince.c

eric:
	${CC} -Wall -Wextra -pedantic -std=c89 -o vince tp1_eric.c
