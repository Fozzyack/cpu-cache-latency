CC = gcc
CFLAGS = -std=gnu17 -O2 -Wall -Wextra -Werror -Wpedantic

TARGET = mem_lantency.out

.PHONY: clean run

default : $(TARGET)

clean:
	rm -rf $(TARGET)

run: $(TARGET)
	./$(TARGET)

$(TARGET): main.c Makefile
	$(CC) $(CFLAGS) -o $(TARGET) main.c
