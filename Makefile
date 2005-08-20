# Note: You'll have to edit this makefile if you want to use it,
#       but it shouldn't be hard if you know how to compile programs.

CC = gcc
CFLAGS = -Wall -O2 -I/opt/local/gnu/include
LDFLAGS = -L/opt/local/gnu/lib -R/opt/local/gnu/lib
LIBS = -lreadline -lncurses

edln: edln.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) -o edln edln.c

clean:
	-rm -f edln *~
