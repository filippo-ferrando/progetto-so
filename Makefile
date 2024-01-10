CC = gcc
CFLAGS = -Wvla -Wextra -Werror

# -Wvla: warns if variable-length array is used in C90, and rejects it in C99
# -Wextra: enables some extra warning flags that are not enabled by -Wall
# -Werror: make all warnings into errors

.PHONY: clean run

# PHONY is used to tell make that clean and run are not files

Master.out: Master.c Alimentatore.c Attivatore.c Atomo.c Inibitore.c
	$(CC) $(CFLAGS) -o Atomo.out Atomo.c
	$(CC) $(CFLAGS) -o Inibitore.out Inibitore.c
	$(CC) $(CFLAGS) -o Attivatore.out Attivatore.c
	$(CC) $(CFLAGS) -o Alimentatore.out Alimentatore.c
	$(CC) $(CFLAGS) -o Master.out Master.c

clean:
	rm -f *.out

setup:
	./env-setter.sh

run:
	./Master.out