CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -pthread
TARGET = project2
SRC = main.c

.PHONY: all run clean test

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	@echo "Running bank simulation (10-second timeout)..."
	timeout 10 ./$(TARGET) > output.txt
	@echo "Customers who exited:"
	@grep -c "finished leaving through door" output.txt || true
	@echo "Output saved to output.txt"

clean:
	rm -f $(TARGET) output.txt
