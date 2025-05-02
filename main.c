#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define SIZE 9

int sudoku[SIZE][SIZE];  // جدول سودوکو

// نتایج اعتبارسنجی توسط تردها
bool row_valid = false;
bool column_valid = false;
bool subgrid_valid[9] = { false };

// تابع بررسی سطرها
void* check_rows(void* arg) {
    for (int i = 0; i < SIZE; i++) {
        bool digits[SIZE + 1] = { false };
        for (int j = 0; j < SIZE; j++) {
            int num = sudoku[i][j];
            if (num < 1 || num > 9 || digits[num]) {
                pthread_exit(NULL);
            }
            digits[num] = true;
        }
    }
    row_valid = true;
    pthread_exit(NULL);
}
// تابع بررسی ستون‌ها
void* check_columns(void* arg) {
    for (int i = 0; i < SIZE; i++) {
        bool digits[SIZE + 1] = { false };
        for (int j = 0; j < SIZE; j++) {
            int num = sudoku[j][i];
            if (num < 1 || num > 9 || digits[num]) {
                pthread_exit(NULL);
            }
            digits[num] = true;
        }
    }
    column_valid = true;
    pthread_exit(NULL);
}

typedef struct {
    int start_row;
    int start_col;
    int index;
} SubgridArgs;

// تابع بررسی یک زیرشبکه 3x3
void* check_subgrid(void* args) {
    SubgridArgs* data = (SubgridArgs*) args;
    bool digits[SIZE + 1] = { false };

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int num = sudoku[data->start_row + i][data->start_col + j];
            if (num < 1 || num > 9 || digits[num]) {
                pthread_exit(NULL);
            }
            digits[num] = true;
        }
    }
    subgrid_valid[data->index] = true;
    pthread_exit(NULL);
}

int main(){
    // دریافت جدول سودوکو از ورودی
    printf("Please enter the Sudoku grid (9 9 numbers from 1 to 9):\n");
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            scanf("%d", &sudoku[i][j]);

    pthread_t threads[11];
    SubgridArgs* args[9];

    // ایجاد ترد برای سطرها و ستون‌ها
    pthread_create(&threads[0], NULL, check_rows, NULL);
    pthread_create(&threads[1], NULL, check_columns, NULL);

    // ایجاد ترد برای هر زیرشبکه 3x3
    int index = 0;
    for (int row = 0; row < SIZE; row += 3) {
        for (int col = 0; col < SIZE; col += 3) {
            args[index] = malloc(sizeof(SubgridArgs));
            args[index]->start_row = row;
            args[index]->start_col = col;
            args[index]->index = index;
            pthread_create(&threads[2 + index], NULL, check_subgrid, args[index]);
            index++;
        }
    }
    // منتظر اتمام تمام تردها
    for (int i = 0; i < 11; i++) {
        pthread_join(threads[i], NULL);
    }

    // آزادسازی حافظه برای آرگومان‌های زیرشبکه
    for (int i = 0; i < 9; i++) {
        free(args[i]);
    }

    // بررسی نتیجه
    bool all_subgrids_valid = true;
    for (int i = 0; i < 9; i++) {
        if (!subgrid_valid[i]) {
            all_subgrids_valid = false;
            break;
        }
    }
    if (row_valid && column_valid && all_subgrids_valid) {
        printf("The Sudoku grid is valid.\n");
    } else {
        printf("The Sudoku grid is not valid.\n");
    }

    return 0;
}

