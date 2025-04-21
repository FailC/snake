CC = gcc
CFLAGS = -std=c2x -Wall -Wextra -pedantic
LDFLAGS = -Lraylib-5.5_linux_amd64/lib -lraylib -Wl,-rpath=raylib-5.5_linux_amd64/lib -lm
SRC = main.c
TARGET = main

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)
