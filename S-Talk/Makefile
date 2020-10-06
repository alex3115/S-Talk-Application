CFLAGS = -Wall -g -std=c11 -D _POSIX_C_SOURCE=200809L -Werror

all: build

build:
	gcc $(CFLAGS) main.c instructorList.o inputThread.c sendThread.c shutdownManager.c receiver.c output.c -lpthread -o s-talk


run: build
	./main

valgrind: build
	valgrind --leak-check=full ./main

clean:
	rm -f s-talk