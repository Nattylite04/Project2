#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

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

pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

void log_action(const char *type, int id, const char *other_type, int other_id, const char *message) {

    pthread_mutex_lock(&print_lock);

    if (other_type != NULL) {
        printf("%s %d [%s %d]: %s\n", type, id, other_type, other_id, message);
    } else {
        printf("%s %d []: %s\n", type, id, message);
    }

    fflush(stdout);

    pthread_mutex_unlock(&print_lock);
}

void sleep_ms(int milliseconds) {
    struct timespec ts;

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000L;

    nanosleep(&ts, NULL);
}



void *teller(void *arg) {
    Teller *t = (Teller *)arg;

    log_action("Teller", t->id, NULL, 0, "ready to serve");
    sem_post(&bank_ready);

    sem_wait(&t->customer_arrived);

    log_action("Teller", t->id, "Customer", t->customer_id, "asks for transaction");

    sem_post(&t->ask_transaction);

    sem_wait(&t->transaction_given);

    if (t->transaction == 0) {
        log_action("Teller", t->id, "Customer", t->customer_id, "receives deposit request");
    } else {
        log_action("Teller", t->id, "Customer", t->customer_id, "receives withdrawal request");
    }

    sem_post(&t->request_received);

    sem_wait(&t->customer_left);

    log_action("Teller", t->id, "Customer", t->customer_id, "customer left teller");

    return NULL;
}

void *customer(void *arg) {
    Customer *c = (Customer *)arg;
    int id = c->id;
    Teller *t = c->assigned_teller;

    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)(id * 2654435761U);

    c -> transaction = (int)(rand_r(&seed) % 2);
    
    if (c->transaction == 0) {
        log_action("Customer", id, NULL, 0, "selects deposit transaction");
    } else {
        log_action("Customer", id, NULL, 0, "selects withdrawal transaction");
    }
    
    int arrival_delay = (int)(rand_r(&seed) % 101);
    
    char message[100];

    snprintf(message, sizeof(message), "waiting %d ms before entering bank", arrival_delay);
    
    log_action("Customer", id, NULL, 0, message);
    
    sleep_ms(arrival_delay);
    
    log_action("\nCustomer", id, NULL, 0, "finished waiting to enter bank");

    t->customer_id = id;

    log_action("Customer", id, "Teller", t->id, "introduces itself");

    sem_post(&t->customer_arrived);

    sem_wait(&t->ask_transaction);

    t->transaction = id % 2;

    if (t->transaction == 0) {
        log_action("Customer", id, "Teller", t->id, "requests deposit");
    } else {
        log_action("Customer", id, "Teller", t->id, "requests withdrawal");
    }

    sem_post(&t->transaction_given);

    sem_wait(&t->request_received);

    log_action("Customer", id, "Teller", t->id, "ends test interaction");

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

        pthread_create(&teller_threads[i], NULL, teller, &tellers[i]);
    }

    for (int i = 0; i < NUM_TELLERS; i++) {
        sem_wait(&bank_ready);
    }

    printf("Bank: all tellers are ready\n\n");
    
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
