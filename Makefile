CC = gcc
CFLAGS = -Wall -O2
LIBS = -lreadline -lncurses

# Set these variables appropriately if your readline or ncurses libraries
# are installed in non-standard locations.
#
# CPPFLAGS = -I/usr/local/include
# LDFLAGS = -L/usr/local/lib -R/usr/local/lib

edln: edln.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o edln edln.c $(LIBS)

clean:
	-rm -f edln *~
