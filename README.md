# Project2
## Description
Bank simulation using multithreading and synchronization. Bank has 3 tellers and serves 50 customers. Each customer randomly chooses to make a deposit or withdrawal. The program uses POSIX threads, semaphores, and mutexes to coordinate interactions between customers and tellers while preventing conflicts over shared resources.

## Files
simulation.c - Main source code containing bank simulation
Makefile - Compiles and runs command. Different command options -> make (compiles code), make run (compiles and runs simulation.c), make test (runs simulation with 10s timeout and writes output into a output.txt file)

## Compilation
gcc -Wall -Wextra -pthread simulation.c -o simulation

## Execution
./simulation

## Program Overview and Transaction Process
Tellers
- 3 teller threads created
- Each teller announces when ready
- Teller waits for customer and request transactions
- Withdrawal transaction require manager
- All transactions require safe access
- Teller wait for customers to leave before serving another customer

Customers
- 50 customer threads created
- Each customer randomly selects deposit or withdrawal
- Customer waits between 0 and 100ms before arriving
- Customer enter through door and wait for available teller
- Customers provide transaction and wait for completion
- Customer leaves bank after transaction completes

Transaction Process
1. All 3 tellers become ready, Bank opens
2. Customers arrive after random delays
3. Each customer waits for available teller
4. Customer introduces itself and provides transaction
5. Withdrawals, teller need manager permission(5-30ms)
6. Teller enters safe to complete transaction(10-50ms)
7. Teller informs customer of completed transaction
8. Customer leaves bank
9. tellers terminate after all 50 customers leave, then bank closes

   
