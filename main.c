#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "function.h"
int main() {
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
close(customer_pipes[i][0]);close(random_product_pipes[i][1]);
for (int j = 0; j < NUM_ORDERS_PER_CUSTOMER; j++) {
int product_index;
read(random_product_pipes[i][0], &product_index, sizeof(int));
sleep(PROCESSING_TIME);
write(customer_pipes[i][1], &product_index, sizeof(int));
}
close(customer_pipes[i][1]);
close(random_product_pipes[i][0]);
exit(0);
} else {
close(customer_pipes[i][1]);
close(random_product_pipes[i][0]);
for (int j = 0; j < NUM_ORDERS_PER_CUSTOMER; j++) {
int product_index = rand() % NUM_PRODUCTS;
write(random_product_pipes[i][1], &product_index, sizeof(int));
int success;
float total_cost;
processOrder(i + 1, product_index, &success, &total_cost);
write(customer_pipes[i][0], &success, sizeof(int));
write(customer_pipes[i][0], &total_cost, sizeof(float));
}close(customer_pipes[i][0]);
close(random_product_pipes[i][1]);
wait(NULL);
}
}
printf("Final Report\n");
// Εκτύπωςη αναφοράσ
return 0;
}
