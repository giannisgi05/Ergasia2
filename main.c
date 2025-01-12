#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define NUM_PRODUCTS 20
#define NUM_CUSTOMERS 5
#define NUM_ORDERS_PER_CUSTOMER 10
#define PROCESSING_TIME 1 // in seconds

typedef struct {
    char description[50];
    float price;
    int item_count;
    int requests;
    int total_requests;
} Product;

// Global catalog array
Product catalog[NUM_PRODUCTS];

// Array to track customers who had failed orders
bool customers_not_served[NUM_CUSTOMERS];

// Function to initialize the catalog with products
void initializeCatalog() {
    // Initialize the catalog with some dummy products
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        sprintf(catalog[i].description, "Product %d", i + 1);
        catalog[i].price = (rand() % 100) + ((float)rand() / RAND_MAX);
        catalog[i].item_count = 2; // Starting with 2 items for each product
        catalog[i].requests = 0;   // Initialize the requests counter
        catalog[i].total_requests = 0; // Initialize the total requests counter
    }

    // Initialize the customers_not_served array
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        customers_not_served[i] = false; // Assume all customers are served at first
    }
}

// Function to process an order and update the catalog
void processOrder(int customer_id, int product_index, int* success, float* total_cost) {
    catalog[product_index].requests++; // Increment the requests counter
    if (catalog[product_index].item_count > 0) {
        *success = 1;
        catalog[product_index].item_count--;
        catalog[product_index].total_requests++; // Increment the total requests counter
        *total_cost = catalog[product_index].price;
        printf("Customer %d: Order successful - %s, Cost: %.2f\n", customer_id, catalog[product_index].description, *total_cost);
    } else {
        *success = 0;
        *total_cost = 0.0;
        catalog[product_index].total_requests++; // Increment the total requests counter even if out of stock
        printf("Customer %d: Order failed - %s out of stock\n", customer_id, catalog[product_index].description);

        // Mark this customer as not served
        customers_not_served[customer_id - 1] = true;
    }
}

int main() {
    // Initialize catalog and pipes
    initializeCatalog();
    int customer_pipes[NUM_CUSTOMERS][2];           // Pipes for communication with customers
    int random_product_pipes[NUM_CUSTOMERS][2];     // Pipes for sending random product index

    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        // Create pipes for communication with each customer
        if (pipe(customer_pipes[i]) == -1 || pipe(random_product_pipes[i]) == -1) {
            perror("Pipe creation failed");
            exit(EXIT_FAILURE);
        }

        // Fork for each customer
        pid_t pid = fork();

        if (pid == -1) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Child process (customer)
            close(customer_pipes[i][0]);             // Close read end of the order pipe
            close(random_product_pipes[i][1]);       // Close write end of the random product pipe

            for (int j = 0; j < NUM_ORDERS_PER_CUSTOMER; j++) {
                int product_index;
                read(random_product_pipes[i][0], &product_index, sizeof(int)); // Receive random product index

                // Sleep for PROCESSING_TIME seconds to simulate processing time
                sleep(PROCESSING_TIME);

                write(customer_pipes[i][1], &product_index, sizeof(int));      // Send order to e-shop

                float total_cost;
                int success;

                read(customer_pipes[i][1], &success, sizeof(int));         // Receive order result
                read(customer_pipes[i][1], &total_cost, sizeof(float));    // Receive total cost

                // Sleep for 1 second between orders
                sleep(1);
            }

            close(customer_pipes[i][1]);           // Close write end of the order pipe
            close(random_product_pipes[i][0]);     // Close read end of the random product pipe
            exit(EXIT_SUCCESS);
        } else {
            // Parent process (e-shop)
            close(customer_pipes[i][1]);               // Close write end of the order pipe
            close(random_product_pipes[i][0]);         // Close read end of the random product pipe

            for (int j = 0; j < NUM_ORDERS_PER_CUSTOMER; j++) {
                int product_index = rand() % NUM_PRODUCTS; // Randomly choose a product
                write(random_product_pipes[i][1], &product_index, sizeof(int)); // Send random product index

                // Sleep for PROCESSING_TIME seconds to simulate processing time
                sleep(PROCESSING_TIME);

                float total_cost;
                int success;

                // Process the order
                processOrder(i + 1, product_index, &success, &total_cost);

                // Send order result back to the customer
                write(customer_pipes[i][0], &success, sizeof(int));
                write(customer_pipes[i][0], &total_cost, sizeof(float));
            }

            close(customer_pipes[i][0]);           // Close read end of the order pipe
            close(random_product_pipes[i][1]);     // Close write end of the random product pipe
            wait(NULL); // Wait for the child process to finish
        }
    }

    // Print the summary report
    printf("\nSummary Report:\n");
    int total_successful_orders = 0;
    int total_failed_orders = 0;
    int total_requests = 0;
    float total_cost = 0.0;
    for (int i = 0; i < NUM_PRODUCTS; i++) {
        printf("Product %d: Requests: %d, Sold: %d\n", i + 1, catalog[i].total_requests, 2 - catalog[i].item_count);
        total_successful_orders += 2 - catalog[i].item_count;
        total_failed_orders += catalog[i].total_requests - (2 - catalog[i].item_count);
        total_requests += catalog[i].total_requests;
        total_cost += (2 - catalog[i].item_count) * catalog[i].price;
    }

    printf("\nOverall Statistics:\n");
    printf("Total Requests: %d\n", total_requests);
    printf("Total Successful Orders: %d\n", total_successful_orders);
    printf("Total Failed Orders: %d\n", total_failed_orders);
    printf("Total Cost of All Orders: %.2f\n", total_cost);

    // Print the list of customers not served
    printf("\nCustomers Not Served:\n");
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        if (customers_not_served[i]) {
            printf("Customer %d\n", i + 1);
        }
    }

    return 0;
}
