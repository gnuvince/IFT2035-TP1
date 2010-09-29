CC=gcc

all: vince eric

vince: tp1_vince.c
	${CC} -Wall -Wextra -pedantic -std=c89 -o vince tp1_vince.c

eric: tp1_eric.c
	${CC} -Wall -Wextra -pedantic -std=c89 -o eric tp1_eric.c
