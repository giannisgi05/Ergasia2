#ifndef FUNCTION_H
#define FUNCTION_H

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

// Δηλώσεις συναρτήσεων
void initializeCatalog();
void processOrder(int customer_id, int product_index, int* success, float* total_cost);

#endif // FUNCTION_H
