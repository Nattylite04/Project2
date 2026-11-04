#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_TELLERS 3

typedef struct {
    int id;
    int customer_id;
    int transaction;

    sem_t customer_arrived;
    sem_t ask_transaction;
    sem_t transaction_given;
    sem_t request_received;
    sem_t customer_left;
} Teller;

typedef struct {
    int id;
    int transaction;
    Teller *assigned_teller;
} Customer;

sem_t bank_ready;

void *teller(void *arg) {
    Teller *t = (Teller *)arg;

    printf("Teller %d []: ready to serve\n", t->id);
    sem_post(&bank_ready);

    sem_wait(&t->customer_arrived);

    printf("Teller %d [Customer %d]: asks for transaction\n",
           t->id, t->customer_id);

    sem_post(&t->ask_transaction);

    sem_wait(&t->transaction_given);

    printf("Teller %d [Customer %d]: receives %s request\n",
           t->id, t->customer_id,
           t->transaction == 0 ? "deposit" : "withdrawal");

    sem_post(&t->request_received);

    sem_wait(&t->customer_left);

    printf("Teller %d: customer left teller\n", t->id);

    return NULL;
}

void *customer(void *arg) {
    Customer *c = (Customer *)arg;
    int id = c->id;
    Teller *t = c->assigned_teller;

    t->customer_id = id;

    printf("Customer %d [Teller %d]: introduces itself\n",
           id, t->id);

    sem_post(&t->customer_arrived);

    sem_wait(&t->ask_transaction);

    t->transaction = id % 2;

    printf("Customer %d [Teller %d]: requests %s\n",
           id, t->id,
           t->transaction == 0 ? "deposit" : "withdrawal");

    sem_post(&t->transaction_given);

    sem_wait(&t->request_received);

    printf("Customer %d [Teller %d]: ends test interaction\n",
           id, t->id);

    sem_post(&t->customer_left);

    return NULL;
}

int main(void) {
    pthread_t teller_threads[NUM_TELLERS];
    pthread_t customer_threads[NUM_TELLERS];

    Teller tellers[NUM_TELLERS];
    Customer customers[NUM_TELLERS];
    sem_init(&bank_ready, 0, 0);

    for (int i = 0; i < NUM_TELLERS; i++) {
        tellers[i].id = i;
        tellers[i].customer_id = -1;
        tellers[i].transaction = -1;

        sem_init(&tellers[i].customer_arrived, 0, 0);
        sem_init(&tellers[i].ask_transaction, 0, 0);
        sem_init(&tellers[i].transaction_given, 0, 0);
        sem_init(&tellers[i].request_received, 0, 0);
        sem_init(&tellers[i].customer_left, 0, 0);

        pthread_create(&teller_threads[i], NULL,
                       teller, &tellers[i]);
    }

    for (int i = 0; i < NUM_TELLERS; i++) {
        sem_wait(&bank_ready);
    }

    printf("Bank: all tellers are ready\n");
    
    for (int i = 0; i < NUM_TELLERS; i++) {
        customers[i].id = i;
        customers[i].transaction = -1;
        customers[i].assigned_teller = &tellers[i];
        
        pthread_create(&customer_threads[i], NULL, customer, &customers[i]);
    }

    for (int i = 0; i < NUM_TELLERS; i++) {
        pthread_join(customer_threads[i], NULL);
        pthread_join(teller_threads[i], NULL);
    }

    for (int i = 0; i < NUM_TELLERS; i++) {
        sem_destroy(&tellers[i].customer_arrived);
        sem_destroy(&tellers[i].ask_transaction);
        sem_destroy(&tellers[i].transaction_given);
        sem_destroy(&tellers[i].request_received);
        sem_destroy(&tellers[i].customer_left);
    }

    sem_destroy(&bank_ready);

    printf("Bank: test completed\n");

    return 0;
}
