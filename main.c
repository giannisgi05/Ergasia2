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
