CC = gcc
CFLAGS = -std=gnu11 -Wall -Wextra -pthread
TARGET = project2
SRC = main.c

.PHONY: compile run test

compile: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: compile
	./$(TARGET)

test: compile
	@echo "Testing bank simulation..."
	timeout 10 ./$(TARGET) > output.txt
	@echo "Customers who finished leaving the bank:"
	@grep -c "finished leaving through door" output.txt
	@echo "Full output saved to output.txt"
