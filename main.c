int main() {
    // Αρχικοποίηση καταλόγου και σωλήνων
    initializeCatalog();
    int customer_pipes[NUM_CUSTOMERS][2];           // Σωλήνες για επικοινωνία με πελάτες
    int random_product_pipes[NUM_CUSTOMERS][2];     // Σωλήνες για αποστολή τυχαίου δείκτη προϊόντος

    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        // Δημιουργία σωλήνων για επικοινωνία με κάθε πελάτη
        if (pipe(customer_pipes[i]) == -1 || pipe(random_product_pipes[i]) == -1) {
            perror("Pipe creation failed");
            exit(EXIT_FAILURE);
        }

        // Δημιουργία διεργασίας για κάθε πελάτη
        pid_t pid = fork();

        if (pid == -1) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Διεργασία παιδιού (πελάτης)
            close(customer_pipes[i][0]);             // Κλείσιμο άκρης ανάγνωσης του σωλήνα παραγγελιών
            close(random_product_pipes[i][1]);       // Κλείσιμο άκρης εγγραφής του σωλήνα τυχαίων προϊόντων

            for (int j = 0; j < NUM_ORDERS_PER_CUSTOMER; j++) {
                int product_index;
                read(random_product_pipes[i][0], &product_index, sizeof(int)); // Λήψη τυχαίου δείκτη προϊόντος

                // Αναμονή για PROCESSING_TIME δευτερόλεπτα για προσομοίωση χρόνου επεξεργασίας
                sleep(PROCESSING_TIME);

                write(customer_pipes[i][1], &product_index, sizeof(int));      // Αποστολή παραγγελίας στο e-shop

                float total_cost;
                int success;

                read(customer_pipes[i][1], &success, sizeof(int));         // Λήψη αποτελέσματος παραγγελίας
                read(customer_pipes[i][1], &total_cost, sizeof(float));    // Λήψη συνολικού κόστους

                // Αναμονή για 1 δευτερόλεπτο μεταξύ των παραγγελιών
                sleep(1);
            }

            close(customer_pipes[i][1]);           // Κλείσιμο άκρης εγγραφής του σωλήνα παραγγελιών
            close(random_product_pipes[i][0]);     // Κλείσιμο άκρης ανάγνωσης του σωλήνα τυχαίων προϊόντων
            exit(EXIT_SUCCESS);
        } else {
            // Διεργασία γονέα (e-shop)
            close(customer_pipes[i][1]);               // Κλείσιμο άκρης εγγραφής του σωλήνα παραγγελιών
            close(random_product_pipes[i][0]);         // Κλείσιμο άκρης ανάγνωσης του σωλήνα τυχαίων προϊόντων

            for (int j = 0; j < NUM_ORDERS_PER_CUSTOMER; j++) {
                int product_index = rand() % NUM_PRODUCTS; // Επιλογή τυχαίου προϊόντος
                write(random_product_pipes[i][1], &product_index, sizeof(int)); // Αποστολή τυχαίου δείκτη προϊόντος

                // Αναμονή για PROCESSING_TIME δευτερόλεπτα για προσομοίωση χρόνου επεξεργασίας
                sleep(PROCESSING_TIME);

                float total_cost;
                int success;

                // Επεξεργασία παραγγελίας
                processOrder(i + 1, product_index, &success, &total_cost);

                // Αποστολή αποτελέσματος παραγγελίας πίσω στον πελάτη
                write(customer_pipes[i][0], &success, sizeof(int));
                write(customer_pipes[i][0], &total_cost, sizeof(float));
            }

            close(customer_pipes[i][0]);           // Κλείσιμο άκρης ανάγνωσης του σωλήνα παραγγελιών
            close(random_product_pipes[i][1]);     // Κλείσιμο άκρης εγγραφής του σωλήνα τυχαίων προϊόντων
            wait(NULL); // Αναμονή για την ολοκλήρωση της διεργασίας παιδιού
        }
    }

    // Εκτύπωση αναφοράς συνοπτικών στοιχείων
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

    // Εκτύπωση λίστας πελατών που δεν εξυπηρετήθηκαν
    printf("\nΠελάτες που δεν εξυπηρετήθηκαν:\n");
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        if (customers_not_served[i]) {
            printf("Customer %d\n", i + 1);
        }
    }

    return 0;
}
