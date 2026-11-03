#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

// Semaphores for communication
sem_t teller_ready;
sem_t customer_arrived;
sem_t transaction_done;

// Teller thread
void *teller(void *arg) {
    printf("Teller 0: ready to serve\n");
    sem_post(&teller_ready);

    // Wait for customer introduction
    sem_wait(&customer_arrived);

    printf("Teller 0 [Customer 0]: receives introduction\n");

    // Let customer know interaction is finished
    sem_post(&transaction_done);

    return NULL;
}

// Customer thread
void *customer(void *arg) {
    // Wait until the teller is ready
    sem_wait(&teller_ready);

    printf("Customer 0 [Teller 0]: approaches teller\n");
    printf("Customer 0 [Teller 0]: introduces itself\n");

    sem_post(&customer_arrived);

    // Wait for teller to finish
    sem_wait(&transaction_done);

    printf("Customer 0 [Teller 0]: leaves teller\n");

    return NULL;
}

int main() {
    pthread_t teller_thread;
    pthread_t customer_thread;

    sem_init(&teller_ready, 0, 0);
    sem_init(&customer_arrived, 0, 0);
    sem_init(&transaction_done, 0, 0);

    pthread_create(&teller_thread, NULL, teller, NULL);
    pthread_create(&customer_thread, NULL, customer, NULL);

    pthread_join(teller_thread, NULL);
    pthread_join(customer_thread, NULL);

    sem_destroy(&teller_ready);
    sem_destroy(&customer_arrived);
    sem_destroy(&transaction_done);

    printf("Bank simulation finished\n");

    return 0;
}
