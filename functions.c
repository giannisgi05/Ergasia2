#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "function.h"
// Καθολικός κατάλογος προϊόντων
Product catalog[NUM_PRODUCTS];
// Πελάτες που δεν εξυπηρετήθηκαν
bool customers_not_served[NUM_CUSTOMERS];
void initializeCatalog() {
for (int i = 0; i < NUM_PRODUCTS; i++) {
sprintf(catalog[i].description, "Product %d", i + 1);
catalog[i].price = (rand() % 100) + ((float)rand() / RAND_MAX);
catalog[i].item_count = 2;
catalog[i].requests = 0;
catalog[i].total_requests = 0;
}
for (int i = 0; i < NUM_CUSTOMERS; i++) {
customers_not_served[i] = false;
}
}
void processOrder(int customer_id, int product_index, int* success, float* total_cost) {
catalog[product_index].requests++;if (catalog[product_index].item_count > 0) {
*success = 1;
catalog[product_index].item_count--;
catalog[product_index].total_requests++;
*total_cost = catalog[product_index].price;
printf("Customer %d: Order successful - %s, Cost: %.2f\n", customer_id,
catalog[product_index].description, *total_cost);
} else {
*success = 0;
*total_cost = 0.0;
catalog[product_index].total_requests++;
printf("Customer %d: Order failed - %s out of stock\n", customer_id,
catalog[product_index].description);
customers_not_served[customer_id - 1] = true;
}
}
