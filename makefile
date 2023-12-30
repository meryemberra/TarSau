CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11

SRC = tarsau.c
OBJ = $(SRC:.c=.o)
TARGET = tarsau

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)

