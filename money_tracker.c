#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CATEGORIES 10
#define MAX_LENGTH 20
#define FILENAME "money_tracker_data.txt"

typedef struct {
    char name[MAX_LENGTH];
    float spent;
    float limit;
} Category;

typedef struct {
    char date[11];  
    float dailySpent;
} DailyExpense;

void initializeCategories(Category categories[], int *categoryCount) {
    *categoryCount = 3;
    strcpy(categories[0].name, "Food");
    strcpy(categories[1].name, "Transport");
    strcpy(categories[2].name, "Stationeries");

    for (int i = 0; i < *categoryCount; i++) {
        categories[i].spent = 0;
        categories[i].limit = 1000.0;  
    }
}

void loadFromFile(Category categories[], int *categoryCount, float *totalBudget, float *currentBalance, float *savingGoal, DailyExpense dailyExpenses[], int *dailyCount) {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("No previous data found. Starting fresh.\n");
        return;
    }

    fscanf(file, "%f %f %f", totalBudget, currentBalance, savingGoal);
    fscanf(file, "%d", categoryCount);
    for (int i = 0; i < *categoryCount; i++) {
        fscanf(file, "%s %f %f", categories[i].name, &categories[i].spent, &categories[i].limit);
    }
    fscanf(file, "%d", dailyCount);
    for (int i = 0; i < *dailyCount; i++) {
        fscanf(file, "%s %f", dailyExpenses[i].date, &dailyExpenses[i].dailySpent);
    }

    fclose(file);
    printf("Data loaded successfully!\n");
}

void saveToFile(Category categories[], int categoryCount, float totalBudget, float currentBalance, float savingGoal, DailyExpense dailyExpenses[], int dailyCount) {
    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("Error saving data!\n");
        return;
    }

    fprintf(file, "%.2f %.2f %.2f\n", totalBudget, currentBalance, savingGoal);
    fprintf(file, "%d\n", categoryCount);
    for (int i = 0; i < categoryCount; i++) {
        fprintf(file, "%s %.2f %.2f\n", categories[i].name, categories[i].spent, categories[i].limit);
    }

    fprintf(file, "%d\n", dailyCount);
    for (int i = 0; i < dailyCount; i++) {
        fprintf(file, "%s %.2f\n", dailyExpenses[i].date, dailyExpenses[i].dailySpent);
    }

    fclose(file);
    printf("Data saved successfully!\n");
}

void getTodayDate(char date[]) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

void recordDailySpending(Category categories[], int categoryCount, float *currentBalance, DailyExpense dailyExpenses[], int *dailyCount) {
    int choice;
    float amount;
    char today[11];
    getTodayDate(today);

    printf("\nCurrent Balance: %.2f\n", *currentBalance);
    printf("Categories:\n");
    for (int i = 0; i < categoryCount; i++) {
        printf("%d. %s (Limit: %.2f, Spent: %.2f)\n", i + 1, categories[i].name, categories[i].limit, categories[i].spent);
    }
    printf("0. Exit\n");

    printf("Enter the category number to record spending: ");
    scanf("%d", &choice);

    if (choice >= 1 && choice <= categoryCount) {
        printf("Enter amount spent on %s: ", categories[choice - 1].name);
        scanf("%f", &amount);

        if (amount > *currentBalance) {
            printf("Insufficient balance! You only have %.2f left.\n", *currentBalance);
        } else if (categories[choice - 1].spent + amount > categories[choice - 1].limit) {
            printf("Warning: Spending exceeds the limit for %s!\n", categories[choice - 1].name);
        } else {
            categories[choice - 1].spent += amount;
            *currentBalance -= amount;

            int found = 0;
            for (int i = 0; i < *dailyCount; i++) {
                if (strcmp(dailyExpenses[i].date, today) == 0) {
                    dailyExpenses[i].dailySpent += amount;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                strcpy(dailyExpenses[*dailyCount].date, today);
                dailyExpenses[*dailyCount].dailySpent = amount;
                (*dailyCount)++;
            }

            printf("Recorded %.2f spent on %s. Remaining balance: %.2f\n", amount, categories[choice - 1].name, *currentBalance);
        }
    } else if (choice != 0) {
        printf("Invalid choice! Try again.\n");
    }
}

void analyzeSpending(float totalBudget, float currentBalance, Category categories[], int categoryCount, float savingGoal, DailyExpense dailyExpenses[], int dailyCount) {
    float totalSpent = totalBudget - currentBalance;

    printf("\n--- Monthly Spending Analysis ---\n");
    for (int i = 0; i < categoryCount; i++) {
        float percentage = (categories[i].spent / categories[i].limit) * 100;
        printf("%s: %.2f/%.2f (%.1f%%)", categories[i].name, categories[i].spent, categories[i].limit, percentage);
        if (categories[i].spent > categories[i].limit) {
            printf(" - Exceeded Limit!");
        }
        printf("\n");
    }

    printf("\nDaily Spending:\n");
    for (int i = 0; i < dailyCount; i++) {
        printf("%s: %.2f\n", dailyExpenses[i].date, dailyExpenses[i].dailySpent);
    }

    printf("\nTotal Spent: %.2f\n", totalSpent);
    printf("Remaining Balance: %.2f\n", currentBalance);

    if (currentBalance < 0) {
        printf("Warning: You have overspent by %.2f!\n", -currentBalance);
    }

    if (savingGoal > 0) {
        printf("\nSaving Goal Progress:\n");
        printf("Goal: %.2f\n", savingGoal);
        printf("Current Savings: %.2f\n", currentBalance);
        if (currentBalance >= savingGoal) {
            printf("Congratulations! You have met your saving goal.\n");
        } else {
            printf("You need to save %.2f more to reach your goal.\n", savingGoal - currentBalance);
        }
    }
}

void addCategory(Category categories[], int *categoryCount) {
    if (*categoryCount >= MAX_CATEGORIES) {
        printf("Cannot add more categories. Maximum limit reached.\n");
        return;
    }

    printf("Enter the name of the new category: ");
    scanf("%s", categories[*categoryCount].name);
    categories[*categoryCount].spent = 0;
    printf("Enter the spending limit for %s: ", categories[*categoryCount].name);
    scanf("%f", &categories[*categoryCount].limit);

    (*categoryCount)++;
    printf("Category added successfully!\n");
}

void setSavingGoal(float *savingGoal, float totalBudget, float currentBalance) {
    printf("\nCurrent Saving Goal: %.2f\n", *savingGoal);
    printf("Enter your new saving goal: ");
    scanf("%f", savingGoal);

    if (*savingGoal > currentBalance) {
        printf("Warning: Your saving goal exceeds your current balance!\n");
    }
    printf("Saving goal updated successfully!\n");
}

void resetData(Category categories[], int *categoryCount, float *totalBudget, float *currentBalance, float *savingGoal, DailyExpense dailyExpenses[], int *dailyCount) {
    char confirm;
    printf("Are you sure you want to reset all data? This cannot be undone. (y/n): ");
    scanf(" %c", &confirm);
    if (confirm == 'y' || confirm == 'Y') {
        if (remove(FILENAME) == 0) {
            printf("Data file deleted successfully.\n");
        } else {
            printf("No data file found or error deleting file.\n");
        }
        initializeCategories(categories, categoryCount);
        *totalBudget = 0;
        *currentBalance = 0;
        *savingGoal = 0;
        *dailyCount = 0;
        printf("Enter your new total budget for the month: ");
        scanf("%f", totalBudget);
        *currentBalance = *totalBudget;
        printf("Data reset successfully. New budget set to %.2f\n", *totalBudget);
    } else {
        printf("Reset cancelled.\n");
    }
}

void addIncome(float *totalBudget, float *currentBalance) {
    float income;
    printf("Enter the amount of income to add: ");
    scanf("%f", &income);
    if (income <= 0) {
        printf("Invalid amount. Income must be positive.\n");
        return;
    }
    *totalBudget += income;
    *currentBalance += income;
    printf("Added %.2f to your budget. New total budget: %.2f\n", income, *totalBudget);
}

int main() {
    float totalBudget = 0, currentBalance = 0, savingGoal = 0;
    Category categories[MAX_CATEGORIES];
    DailyExpense dailyExpenses[100];
    int categoryCount = 0, dailyCount = 0;

    initializeCategories(categories, &categoryCount);

    printf("Loading previous data...\n");
    loadFromFile(categories, &categoryCount, &totalBudget, &currentBalance, &savingGoal, dailyExpenses, &dailyCount);

    if (currentBalance == 0) {
        printf("Enter your total budget for the month: ");
        scanf("%f", &totalBudget);
        currentBalance = totalBudget;
    }

    int choice;
    do {
        printf("\n--- Menu ---\n");
        printf("1. Record Daily Spending\n");
        printf("2. View Spending Analysis\n");
        printf("3. Add New Category\n");
        printf("4. Set Saving Goal\n");
        printf("5. Add Income\n");
        printf("6. Reset Data\n");
        printf("7. Save and Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                recordDailySpending(categories, categoryCount, &currentBalance, dailyExpenses, &dailyCount);
                break;
            case 2:
                analyzeSpending(totalBudget, currentBalance, categories, categoryCount, savingGoal, dailyExpenses, dailyCount);
                break;
            case 3:
                addCategory(categories, &categoryCount);
                break;
            case 4:
                setSavingGoal(&savingGoal, totalBudget, currentBalance);
                break;
            case 5:
                addIncome(&totalBudget, &currentBalance);
                break;
            case 6:
                resetData(categories, &categoryCount, &totalBudget, &currentBalance, &savingGoal, dailyExpenses, &dailyCount);
                break;
            case 7:
                saveToFile(categories, categoryCount, totalBudget, currentBalance, savingGoal, dailyExpenses, dailyCount);
                break;
            default:
                printf("Invalid choice! Try again.\n");
        }
    } while (choice != 7);

    return 0;
}