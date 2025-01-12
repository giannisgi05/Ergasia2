// File: main.c
#include "catalog.h"
#include "orders.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define NUM_PRODUCTS 20
#define NUM_CUSTOMERS 5
#define NUM_ORDERS_PER_CUSTOMER 10
#define PROCESSING_TIME 1

int main() {
// Initialize catalog and pipes
initializeCatalog();
int customer_pipes[NUM_CUSTOMERS][2];
int random_product_pipes[NUM_CUSTOMERS][2];

for (int i = 0; i < NUM_CUSTOMERS; i++) {
if (pipe(customer_pipes[i]) == -1 || pipe(random_product_pipes[i]) == -1) {
perror("Pipe creation failed");
exit(EXIT_FAILURE);
}

pid_t pid = fork();
if (pid == -1) {

perror("Fork failed");
exit(EXIT_FAILURE);
}

if (pid == 0) {
// Child process (customer)
handleCustomer(i, customer_pipes[i], random_product_pipes[i]);
exit(EXIT_SUCCESS);
} else {
// Parent process (e-shop)
handleShop(i, customer_pipes[i], random_product_pipes[i]);
wait(NULL); // Wait for child process
}
}

// Print summary report
printSummary();

return 0;
}
