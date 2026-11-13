#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define NUM_TELLERS 3
#define NUM_CUSTOMERS 50

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

Teller tellers[NUM_TELLERS];

typedef struct {
    int id;
    int transaction;
    Teller *assigned_teller;
} Customer;

sem_t bank_ready;
sem_t manager;
sem_t safe;
sem_t door;

int available_tellers[NUM_TELLERS];

int queue_front = 0;
int queue_rear = 0;

pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t closing_lock = PTHREAD_MUTEX_INITIALIZER;
sem_t teller_available;

int bank_closing = 0;



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

void add_available_teller(int id) {
    pthread_mutex_lock(&queue_lock);
    
    available_tellers[queue_rear] = id;
    queue_rear = (queue_rear + 1) % NUM_TELLERS;
    
    pthread_mutex_unlock(&queue_lock);
    
    sem_post(&teller_available);
}

int get_available_teller(void) {
    sem_wait(&teller_available);
    
    pthread_mutex_lock(&queue_lock);
    
    int id = available_tellers[queue_front];
    queue_front = (queue_front + 1) % NUM_TELLERS;
    
    pthread_mutex_unlock(&queue_lock);
    
    return id;
}

int is_bank_closing(void) {
    pthread_mutex_lock(&closing_lock);
    
    int closing = bank_closing;
    
    pthread_mutex_unlock(&closing_lock);
    
    return closing;
}

void *teller(void *arg) {
    Teller *t = (Teller *)arg;

    unsigned int seed = (unsigned int)time(NULL) ^ ((unsigned int)(t->id + 1) * 2654435761U);
        
    log_action("Teller", t->id, NULL, 0, "ready to serve");
    
    sem_post(&bank_ready);

    while(1) {
        log_action("Teller", t->id, NULL, 0, "waiting for a customer");
        
        add_available_teller(t->id);
    
        sem_wait(&t->customer_arrived);
        
        if(is_bank_closing()) {
            break;
        }
        
        int customer_id = t->customer_id;
        log_action("Teller", t->id, "Customer", t->customer_id, "serving a customer");
        
        log_action("Teller", t->id, "Customer", t->customer_id, "asks for transaction");
    
        sem_post(&t->ask_transaction);
    
        sem_wait(&t->transaction_given);
    
        if (t->transaction == 0) {
            log_action("Teller", t->id, "Customer", t->customer_id, "handling deposit transaction");
        } else {
            log_action("Teller", t->id, "Customer", t->customer_id, "handling withdrawal transaction");
        }
        
        if (t->transaction == 1) {
    
            log_action("Teller", t->id, NULL, 0, "going to the manager");
        
            sem_wait(&manager);
        
            log_action("Teller", t->id, NULL, 0, "getting manager's permission");
        
            int delay = 5 + (int)(rand_r(&seed) % 26);
        
            sleep_ms(delay);
        
            log_action("Teller", t->id, NULL, 0, "got manager's permission");
        
            sem_post(&manager);
        }
        
        log_action("Teller", t->id, "Customer", t->customer_id, "going to safe");
    
        sem_wait(&safe);
        
        log_action("Teller", t->id, NULL, 0, "enter safe");
        
        int safe_delay = 10 + (int)(rand_r(&seed) % 41);
        
        sleep_ms(safe_delay);
        
        log_action("Teller", t->id, NULL, 0, "leaving safe");
        
        sem_post(&safe);
        
        if(t->transaction == 0) {
            log_action("Teller", t->id, "Customer", t->customer_id, "finishes deposit transaction");
        }
        else {
            log_action("Teller", t->id, "Customer", t->customer_id, "finishes withdrawal transaction");
        }
        
        log_action("Teller", t->id, "Customer", customer_id, "wait for customer to leave");
        sem_post(&t->request_received);
    
        sem_wait(&t->customer_left);
        
        log_action("Teller", t->id, NULL, 0, "ready to serve");
    } 
    
    log_action("Teller", t->id, NULL, 0, "leaving for the day");
    return NULL;
}

void *customer(void *arg) {
    Customer *c = (Customer *)arg;
    int id = c->id;

    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)(id * 2654435761U);

    c -> transaction = (int)(rand_r(&seed) % 2);
    
    if (c->transaction == 0) {
        log_action("Customer", id, NULL, 0, "wants to perform a deposit transaction");
    } else {
        log_action("Customer", id, NULL, 0, "wants to perform a withdrawal transaction");
    }
    
    int arrival_delay = (int)(rand_r(&seed) % 101);
    
    char message[100];

    snprintf(message, sizeof(message), "waiting %d ms before entering bank", arrival_delay);
    
    log_action("Customer", id, NULL, 0, message);

    sleep_ms(arrival_delay);
    
    log_action("Customer", id, NULL, 0, "finished waiting to enter bank");
    
    log_action("Customer", id, NULL, 0, "going to bank");

    sem_wait(&door);
    
    log_action("Customer", id, NULL, 0, "entering bank");
    
    sem_post(&door);
    
    log_action("Customer", id, NULL, 0, "getting in line");
    
    log_action("Customer", id, NULL, 0, "selecting a teller");
    
    int teller_id = get_available_teller();
    
    Teller *t = &tellers[teller_id];
    
    c->assigned_teller = t;
    
    t->customer_id = id;
    
    log_action("Customer", id, "Teller", t->id, "selects teller");

    log_action("Customer", id, "Teller", t->id, "introduces itself");

    sem_post(&t->customer_arrived);

    sem_wait(&t->ask_transaction);

    t->transaction = c->transaction;

    if (t->transaction == 0) {
        log_action("Customer", id, "Teller", t->id, "asks for a deposit transaction");
    } else {
        log_action("Customer", id, "Teller", t->id, "asks for withdrawal transaction");
    }

    sem_post(&t->transaction_given);

    sem_wait(&t->request_received);

    log_action("Customer", id, "Teller", t->id, "leaves teller");
    
    log_action("Customer", id, NULL, 0, "goes to door");
    
    sem_wait(&door);
    
    log_action("Customer", id, NULL, 0, "leaves the bank");
    
    sem_post(&door);
    
    sem_post(&t->customer_left);
    
    return NULL;
}

int main(void) {
    pthread_t teller_threads[NUM_TELLERS];
    pthread_t customer_threads[NUM_CUSTOMERS];

    Customer customers[NUM_CUSTOMERS];
    sem_init(&bank_ready, 0, 0);
    sem_init(&manager, 0, 1);
    sem_init(&safe, 0, 2);
    sem_init(&door, 0, 2);
    sem_init(&teller_available, 0, 0);
    

    for (int i = 0; i < NUM_TELLERS; i++) {
        tellers[i].id = i;
        tellers[i].customer_id = -1;
        tellers[i].transaction = -1;

        sem_init(&tellers[i].customer_arrived, 0, 0);
        sem_init(&tellers[i].ask_transaction, 0, 0);
        sem_init(&tellers[i].transaction_given, 0, 0);
        sem_init(&tellers[i].request_received, 0, 0);
        sem_init(&tellers[i].customer_left, 0, 0);
    }
    
    for(int i = 0; i < NUM_TELLERS; i++) {
        pthread_create(&teller_threads[i], NULL, teller, &tellers[i]);
    }

    for (int i = 0; i < NUM_TELLERS; i++) {
        sem_wait(&bank_ready);
    }

    printf("Bank: all tellers are ready\n");
    
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        customers[i].id = i;
        customers[i].transaction = -1;
        customers[i].assigned_teller = NULL;
        
        pthread_create(&customer_threads[i], NULL, customer, &customers[i]);
    }

    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customer_threads[i], NULL);
    }

    pthread_mutex_lock(&closing_lock);
    bank_closing = 1;
    pthread_mutex_unlock(&closing_lock);

    for (int i = 0; i < NUM_TELLERS; i++) {
        sem_post(&tellers[i].customer_arrived);
    }
    
    for (int i = 0; i < NUM_TELLERS; i++) {
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
    sem_destroy(&manager);
    sem_destroy(&safe);
    sem_destroy(&door);
    sem_destroy(&teller_available);
    
    pthread_mutex_destroy(&queue_lock);
    pthread_mutex_destroy(&closing_lock);
    pthread_mutex_destroy(&print_lock);

    printf("The bank closes for the day.\n");

    return 0;
}
