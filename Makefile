CC = gcc
CFLAGS = -std=c2x -Wall -Wextra -pedantic
LDFLAGS = -Lraylib-5.5_linux_amd64/lib -lraylib -Wl,-rpath=raylib-5.5_linux_amd64/lib -lm
SRC = main.c
TARGET = main

STATIC_LIBS = ./raylib-5.5_linux_amd64/lib/libraylib.a -lm -ldl -lpthread -lGL -lrt -lX11
STATIC_TARGET = snake

# Default: dynamic build
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Static build
static: $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(STATIC_LIBS) -o $(STATIC_TARGET)

# Clean
clean:
	rm -f $(TARGET) $(STATIC_TARGET)
