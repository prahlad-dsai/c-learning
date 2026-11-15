#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LEN 200
#define MAX_ACCOUNTS 100
#define MAX_TRANSACTIONS 500
#define ACCOUNTS_FILE "accounts.dat"
#define TRANSACTIONS_FILE "transactions.dat"

union ExtraInfo {
    int target_account_id;
    float fee_charged;
};

struct Account {
    char name[MAX_LEN];
    int acc_id;
    float balance;
};

struct Transaction {
    int transaction_id;
    int acc_id;
    char type[100];
    float amount;
    union ExtraInfo extra;
};

struct Account accounts[MAX_ACCOUNTS];
int account_count = 0;
struct Transaction transactions[MAX_TRANSACTIONS];
int transaction_count = 0;

void load_data() {
    FILE *accFile = fopen(ACCOUNTS_FILE, "rb");
    if (accFile != NULL) {
        fread(&account_count, sizeof(int), 1, accFile);
        fread(accounts, sizeof(struct Account), account_count, accFile);
        fclose(accFile);
    }

    FILE *txnFile = fopen(TRANSACTIONS_FILE, "rb");
    if (txnFile != NULL) {
        fread(&transaction_count, sizeof(int), 1, txnFile);
        fread(transactions, sizeof(struct Transaction), transaction_count, txnFile);
        fclose(txnFile);
    }
}

void save_data() {
    FILE *accFile = fopen(ACCOUNTS_FILE, "wb");
    if (accFile != NULL) {
        fwrite(&account_count, sizeof(int), 1, accFile);
        fwrite(accounts, sizeof(struct Account), account_count, accFile);
        fclose(accFile);
    }

    FILE *txnFile = fopen(TRANSACTIONS_FILE, "wb");
    if (txnFile != NULL) {
        fwrite(&transaction_count, sizeof(int), 1, txnFile);
        fwrite(transactions, sizeof(struct Transaction), transaction_count, txnFile);
        fclose(txnFile);
    }
}

struct Account* find_account(int id) {
    for (int i = 0; i < account_count; i++) {
        if (accounts[i].acc_id == id) {
            return &accounts[i];
        }
    }
    return NULL;
}

void create_account() {
    if (account_count >= MAX_ACCOUNTS) {
        printf("Error: Account database is full!\n");
        return;
    }

    struct Account newAcc;
    newAcc.acc_id = (account_count == 0) ? 1001 : accounts[account_count - 1].acc_id + 1;

    printf("Enter Account Holder Name: ");
    scanf(" %[^\n]s", newAcc.name);

    printf("Enter Initial Deposit Amount: ");
    scanf("%f", &newAcc.balance);

    if (newAcc.balance < 0) {
        printf("Error: Initial balance cannot be negative.\n");
        return;
    }

    accounts[account_count++] = newAcc;
    save_data();
    printf("Account created successfully! Assigned Account ID: %d\n", newAcc.acc_id);
}

void deposit_money() {
    int id;
    float amount;

    printf("Enter Account ID: ");
    scanf("%d", &id);

    struct Account *acc = find_account(id);
    if (acc == NULL) {
        printf("Error: Account not found!\n");
        return;
    }

    printf("Enter Amount to Deposit: ");
    scanf("%f", &amount);

    if (amount <= 0) {
        printf("Error: Invalid deposit amount.\n");
        return;
    }

    acc->balance += amount;

    if (transaction_count < MAX_TRANSACTIONS) {
        struct Transaction t;
        t.transaction_id = transaction_count + 1;
        t.acc_id = id;
        strcpy(t.type, "Deposit");
        t.amount = amount;
        t.extra.fee_charged = 0.0;
        transactions[transaction_count++] = t;
    }

    save_data();
    printf("Deposit successful! New Balance: %.2f\n", acc->balance);
}

void withdraw_money() {
    int id;
    float amount;

    printf("Enter Account ID: ");
    scanf("%d", &id);

    struct Account *acc = find_account(id);
    if (acc == NULL) {
        printf("Error: Account not found!\n");
        return;
    }

    printf("Enter Amount to Withdraw: ");
    scanf("%f", &amount);

    if (amount <= 0 || amount > acc->balance) {
        printf("Error: Insufficient funds or invalid amount.\n");
        return;
    }

    acc->balance -= amount;

    if (transaction_count < MAX_TRANSACTIONS) {
        struct Transaction t;
        t.transaction_id = transaction_count + 1;
        t.acc_id = id;
        strcpy(t.type, "Withdraw");
        t.amount = amount;
        t.extra.fee_charged = 0.0;
        transactions[transaction_count++] = t;
    }

    save_data();
    printf("Withdrawal successful! Remaining Balance: %.2f\n", acc->balance);
}

void transfer_funds() {
    int src_id, dest_id;
    float amount;

    printf("Enter Source Account ID: ");
    scanf("%d", &src_id);
    printf("Enter Destination Account ID: ");
    scanf("%d", &dest_id);

    if (src_id == dest_id) {
        printf("Error: Source and destination accounts cannot be the same.\n");
        return;
    }

    struct Account *src = find_account(src_id);
    struct Account *dest = find_account(dest_id);

    if (src == NULL || dest == NULL) {
        printf("Error: One or both accounts not found!\n");
        return;
    }

    printf("Enter Transfer Amount: ");
    scanf("%f", &amount);

    if (amount <= 0 || amount > src->balance) {
        printf("Error: Insufficient funds or invalid amount.\n");
        return;
    }

    src->balance -= amount;
    dest->balance += amount;

    if (transaction_count < MAX_TRANSACTIONS) {
        struct Transaction t1;
        t1.transaction_id = transaction_count + 1;
        t1.acc_id = src_id;
        strcpy(t1.type, "Transfer Out");
        t1.amount = amount;
        t1.extra.target_account_id = dest_id;
        transactions[transaction_count++] = t1;
    }

    if (transaction_count < MAX_TRANSACTIONS) {
        struct Transaction t2;
        t2.transaction_id = transaction_count + 1;
        t2.acc_id = dest_id;
        strcpy(t2.type, "Transfer In");
        t2.amount = amount;
        t2.extra.target_account_id = src_id;
        transactions[transaction_count++] = t2;
    }

    save_data();
    printf("Transfer successful! New Balance for Account %d: %.2f\n", src_id, src->balance);
}

void check_balance() {
    int id;
    printf("Enter Account ID: ");
    scanf("%d", &id);

    struct Account *acc = find_account(id);
    if (acc == NULL) {
        printf("Error: Account not found!\n");
        return;
    }

    printf("\n--- Account Details ---\n");
    printf("Account ID   : %d\n", acc->acc_id);
    printf("Holder Name  : %s\n", acc->name);
    printf("Current Balance: %.2f\n", acc->balance);
}

void view_transaction_history() {
    int id;
    printf("Enter Account ID for Statement: ");
    scanf("%d", &id);

    struct Account *acc = find_account(id);
    if (acc == NULL) {
        printf("Error: Account not found!\n");
        return;
    }

    printf("\n=========================================\n");
    printf("          STATEMENT FOR ACC: %d          \n", id);
    printf("  Holder: %s | Balance: %.2f           \n", acc->name, acc->balance);
    printf("=========================================\n");
    printf("%-5s | %-15s | %-10s\n", "ID", "Type", "Amount");
    printf("-----------------------------------------\n");

    int found = 0;
    for (int i = 0; i < transaction_count; i++) {
        if (transactions[i].acc_id == id) {
            printf("%-5d | %-15s | %.2f\n", 
                transactions[i].transaction_id, 
                transactions[i].type, 
                transactions[i].amount);
            found = 1;
        }
    }

    if (!found) {
        printf("No transactions recorded for this account yet.\n");
    }
    printf("=========================================\n");
}

int main() {
	int choice;
	load_data();
	while(1) {
		printf("\n=========================================\n");
        printf("    BANK ACCOUNT & TRANSACTION SYSTEM    \n");
        printf("=========================================\n");
        printf("1. Create Account\n");
        printf("2. Deposit Money\n");
        printf("3. Withdraw Money\n");
        printf("4. Transfer Funds\n");
        printf("5. Check Balance\n");
        printf("6. View Transaction Statement\n");
        printf("7. Exit\n");
        printf("Enter your choice (1-7): ");
		scanf("%d", &choice);
		if (choice == 1)
			create_account();
		else if (choice == 2)
			deposit_money();
		else if (choice == 3)
			withdraw_money();
		else if (choice == 4)
			transfer_funds();
		else if (choice == 5)
			check_balance();
		else if (choice == 6)
			view_transaction_history();
		else if (choice == 7) {
			printf("Thank you. Please Visit Again!\n");
			break;
		}
		else
			printf("Invalid choice. Please select a choice from 1 to 7 again");
	}
	return 0;
}
