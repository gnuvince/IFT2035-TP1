CC=gcc

all: vince eric commun

vince: tp1_vince.c
	${CC} -Wall -Wextra -pedantic -std=c89 -o vince tp1_vince.c

eric: tp1_eric.c
	${CC} -Wall -Wextra -pedantic -std=c89 -o eric tp1_eric.c

commun: tp1_commun.c
	${CC} -Wall -Wextra -pedantic -std=c89 -o commun tp1_commun.c

