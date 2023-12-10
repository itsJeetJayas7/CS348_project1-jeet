CC = gcc
CFLAGS = -Wall `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0` -lsqlite3

SRC = testingC.c
OUT = event_RSVP

all:
	@$(CC) -o $(OUT) $(SRC) $(CFLAGS) $(LIBS) > /dev/null 2>&1

clean:
	@rm -f $(OUT) > /dev/null 2>&1
