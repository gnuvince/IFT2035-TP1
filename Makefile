CC=gcc
COPTS=-Wall -Wextra -pedantic -std=c89 -g

all: vince eric commun

vince: tp1_vince.c
	${CC} ${COPTS}  -o vince tp1_vince.c

eric: tp1_eric.c
	${CC} ${COPTS} -o eric tp1_eric.c

commun: tp1_commun.c
	${CC} ${COPTS} -o commun tp1_commun.c

