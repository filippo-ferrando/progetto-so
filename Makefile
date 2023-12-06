CC = gcc
CFLAGS = -Wvla -Wextra -Werror

.PHONY: clean run

Master.out: Master.c Alimentatore.c Attivatore.c Atomo.c Inibitore.c
	$(CC) $(CFLAGS) -o Atomo.out Atomo.c
	$(CC) $(CFLAGS) -o Inibitore.out Inibitore.c
	$(CC) $(CFLAGS) -o Attivatore.out Attivatore.c
	$(CC) $(CFLAGS) -o Alimentatore.out Alimentatore.c
	$(CC) $(CFLAGS) -o Master.out Master.c

clean:
	rm -f *.out

run:
	./Master.out