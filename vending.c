#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PRODUCTS 100
#define LOAD_ERROR -1
#define BUY_ERROR -1
#define SUCCESS 0

typedef struct {
    int product_id;
    char name[50];
    double price;
    int quantity_in_hand;
} Product;

Product *inventory[MAX_PRODUCTS];
int product_count = 0;

int findProduct(int product_id) {
    for (int i = 0; i < product_count; i++) {
        if (inventory[i] != NULL && inventory[i]->product_id == product_id) {
            return i;
        }
    }
    return -1;
}

int loadProduct(int id, const char *name, double price, int qty) {
    if (product_count >= MAX_PRODUCTS) {
        return LOAD_ERROR;
    }
    if (id <= 0 || price < 0 || qty < 0) {
        return LOAD_ERROR;
    }
    if (findProduct(id) != -1) {
        return LOAD_ERROR;
    }
    
    Product *newProduct = (Product *)malloc(sizeof(Product));
    if (newProduct == NULL) {
        return LOAD_ERROR;
    }
    
    newProduct->product_id = id;
    strncpy(newProduct->name, name, sizeof(newProduct->name) - 1);
    newProduct->name[sizeof(newProduct->name) - 1] = '\0';
    newProduct->price = price;
    newProduct->quantity_in_hand = qty;
    
    inventory[product_count] = newProduct;
    product_count++;
    
    return SUCCESS;
}

int buyProduct(int id, int qty) {
    int idx = findProduct(id);
    
    if (idx == -1) {
        printf("\n[ERROR] Product ID %d not found.\n", id);
        return BUY_ERROR;
    }
    if (qty <= 0) {
        printf("\n[ERROR] Invalid quantity requested: %d.\n", qty);
        return BUY_ERROR;
    }
    if (qty > inventory[idx]->quantity_in_hand) {
        printf("\n[ERROR] Insufficient stock! Available: %d, Requested: %d.\n", 
               inventory[idx]->quantity_in_hand, qty);
        return BUY_ERROR;
    }
    
    inventory[idx]->quantity_in_hand -= qty;
    double total = qty * inventory[idx]->price;
    printf("\n[SUCCESS] Purchased %d x %s. Total Cost: $%.2f\n", qty, inventory[idx]->name, total);
    
    return SUCCESS;
}

void inventoryReport(void) {
    printf("\n========================================\n");
    printf("            INVENTORY REPORT            \n");
    printf("========================================\n");
    printf("%-10s %-20s %-10s\n", "ID", "NAME", "QTY");
    printf("----------------------------------------\n");
    for (int i = 0; i < product_count; i++) {
        if (inventory[i] != NULL) {
            printf("%-10d %-20s %-10d\n", 
                   inventory[i]->product_id, 
                   inventory[i]->name, 
                   inventory[i]->quantity_in_hand);
        }
    }
    printf("========================================\n\n");
}

int main(void) {
    char line[256], name[50];
    int id, qty, choice;
    double price;

    printf("========================================\n");
    printf("    VENDING MACHINE INVENTORY SYSTEM    \n");
    printf("========================================\n");
    printf("Loading initial inventory from products.dat...\n");

    FILE *file = fopen("products.dat", "r");
    if (file == NULL) {
        printf("[ERROR] Could not open products.dat file. Creating a new one.\n");
    } else {
        while (fgets(line, sizeof(line), file) != NULL) {
            if (strlen(line) <= 1) continue;
            
            if (sscanf(line, "%d %49s %lf %d", &id, name, &price, &qty) == 4) {
                if (loadProduct(id, name, price, qty) != SUCCESS) {
                    printf("[LOG] Failed to load product ID: %d\n", id);
                }
            }
        }
        fclose(file);
    }

    printf("Inventory loaded successfully (%d products loaded).\n\n", product_count);

    do {
        fprintf(stderr, "========================================\n");
        fprintf(stderr, "             MAIN MENU                  \n");
        fprintf(stderr, "========================================\n");
        fprintf(stderr, " 1. Buy Product\n");
        fprintf(stderr, " 2. View Inventory Report\n");
        fprintf(stderr, " 3. Add New Product\n");
        fprintf(stderr, " 4. Exit\n");
        fprintf(stderr, "----------------------------------------\n");
        fprintf(stderr, " Enter your choice [1-4]: ");
        
        if (scanf("%d", &choice) != 1) {
            choice = 4;
        }

        switch (choice) {
            case 1:
                printf("\n--- BUY PRODUCT ---\n");
                printf("Enter Product ID and Quantity: ");
                if (scanf("%d %d", &id, &qty) == 2) {
                    buyProduct(id, qty);
                } else {
                    printf("[ERROR] Invalid input format.\n");
                }
                break;
                
            case 2:
                inventoryReport();
                break;

            case 3:
                printf("\n--- ADD NEW PRODUCT ---\n");
                printf("Enter ID, Name, Price, and Quantity (e.g., 204 Chips 1.25 10): ");
                if (scanf("%d %49s %lf %d", &id, name, &price, &qty) == 4) {
                    if (loadProduct(id, name, price, qty) == SUCCESS) {
                        FILE *outfile = fopen("products.dat", "a");
                        if (outfile != NULL) {
                            fprintf(outfile, "%d %s %.2f %d\n", id, name, price, qty);
                            fclose(outfile);
                            printf("[SUCCESS] Product added and saved to local storage!\n");
                        } else {
                            printf("[WARNING] Product loaded in memory, but failed to save to disk.\n");
                        }
                    } else {
                        printf("[ERROR] Could not load product (Check if ID already exists or limit reached).\n");
                    }
                } else {
                    printf("[ERROR] Invalid input format.\n");
                }
                break;
                
            case 4:
                printf("\nThank you for using the system. Goodbye!\n");
                break;
                
            default:
                printf("\n[ERROR] Invalid choice. Please select between 1 and 4.\n\n");
                break;
        }
    } while (choice != 4);

    return 0;
}