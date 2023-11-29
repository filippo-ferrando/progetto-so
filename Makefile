CC = gcc
CFLAGS = -Wvla -Wextra -Werror

.PHONY: clean

clean:
	rm -f *.out