// Date: 10/17/2020
// N-Queens problem solved by backtracking (no parallelism)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <time.h>

#define BILLION 1000000000L

pthread_mutex_t lock= PTHREAD_MUTEX_INITIALIZER;
int totalcount = 0;
int profitmax  = 0;
double calc_time_arr[20];

typedef struct{
    int n,pid,p;
    int ** board_max;
    // double *calc_time_ptr;
}GM;

// To check if a location is safe to place a queen
int isSafe(int ** board, int row, int col, int n) {
    // check same row
    for (int c = 0; c < col; c ++) {
        if (board[row][c] == 1) {return 0;}
    }
    int i = row - 1; 
    int j = col - 1;
    // check same diagonal(up)
    while (i >= 0 && j >= 0) {
        if (board[i][j] == 1) {return 0;}
        i --;
        j --;
    }
    i = row + 1;
    j = col - 1;
    // check same diagonal(down)
    while (i < n && j >= 0) {
        if (board[i][j] == 1) {return 0;}
        i ++;
        j --;
    }
    return 1;
}

// Recursive function
void solveHelper(int ** board, int n, int col, int **opt_board, int *count, int *profit, int*profit_best, int i) {
    if (col == n) {
        if (*profit > *profit_best){
            *profit_best = *profit;
            for (int i = 0; i < n;i ++){
                for (int j = 0; j < n; j ++){
                    opt_board[i][j] = board[i][j];
                }
            }
        }
        if (i == (n + 1) / 2 - 1 && (n % 2 != 0) ) {
            *count = *count + 1;         //it is deal with the mid row
        }  
        else {
            *count = *count + 2;
        }
        return;
    }
    for (int row = 0; row < n; row ++) {
        if (isSafe(board, row, col, n) == 1) {
            board[row][col] = 1;
            *profit += abs(col - row);
            solveHelper(board, n, col + 1, opt_board, count, profit, profit_best, i);
            board[row][col] = 0;
            *profit -= abs(col - row);
        }
    }
}

// Print the board
void printBoard(int ** board, int n) {
    for (int i = 0; i < n; i ++) {
        for (int j = 0; j < n; j ++) {
            printf("%d  ", board[i][j]);
        }
        printf("\n");
    }
}

// Initialization and entry to the recursive function
void solveNQueens(int n,int pid,int p,int **board_max) {
    int ** board        = (int **)malloc(n * sizeof(int *));
    int *  count        = (int *)malloc(sizeof(int));
    int ** opt_board    = (int**)malloc(n * sizeof(int *));
    int *  profit       = (int *)malloc(sizeof(int));
    int *  profit_best  = (int *)malloc(sizeof(int));

    //a counter for each thread
     * count = 0;
     * profit_best = 0;
    for (int i = 0; i < n; i ++) {
        board[i] = (int *)malloc(n * sizeof(int));
        opt_board[i]= (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j ++) {
            board[i][j] = 0;
            opt_board[i][j] = 0;
        }
    }
    int iteration = (n + 1) / 2;
    for (int i = pid; i < iteration; i += p)
    {
        board[i][0] = 1;
        *profit += +i;
        solveHelper(board, n, 1, opt_board, count, profit, profit_best, i);
        board[i][0] = 0;
        *profit -= i;
    }
    
    pthread_mutex_lock(&lock);
    totalcount += *count;
    if (*profit_best > profitmax){
        profitmax = *profit_best;
        for (int i = 0; i < n; i ++) {
                for (int j = 0; j < n; j ++) {
                board_max[i][j] = opt_board[i][j];
                }
            }
    }
    pthread_mutex_unlock(&lock);
}

void *nqueensHelper(void *varg){
    GM *arg =varg;
    int pid, n, p, count;
    int **board_max;
    struct timespec cal_start, cal_end_time;
    double calc_time;
    clock_gettime(CLOCK_MONOTONIC, &cal_start);
    pid           = arg->pid;
    p             = arg->p;
    n             = arg->n;
    board_max     = arg->board_max;

    solveNQueens(n, pid, p, board_max); 
    clock_gettime(CLOCK_MONOTONIC, &cal_end_time);
    calc_time = BILLION * (cal_end_time.tv_sec - cal_start.tv_sec) + (cal_end_time.tv_nsec - cal_start.tv_nsec);
    calc_time = calc_time / BILLION;
    calc_time_arr[pid] = calc_time;
    return NULL;
}

int main(int argc, char **argv) {
    int n, p;
    double total_time, tcreation_time, finish_time;
    struct timespec start, end;
    struct timespec tcreation_end, finish_end;
    
    if (argc != 3) {
        printf("Usage: nqueens n p\nAborting...\n");
        exit(0);
    }
    n = atoi(argv[1]);
    p = atoi(argv[2]);
    int tnum = p > n ? n : p;
    
    printf("n = %d\np = %d\n", n, p);
    fflush(stdout);
    int ** board_max    = (int**)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i ++) {
        board_max[i] = (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j ++) {
            board_max[i][j] = 0;
        }
    }

    /*modify the parallel version*/
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_t *threads = malloc (p* sizeof(threads));
    for (int i = 0; i < tnum; i ++){
        GM *arg = malloc(sizeof(*arg));
        arg->n = n;
        arg->pid = i;
        arg->p = p;
        arg->board_max = board_max;
        pthread_create(&threads[i], NULL, nqueensHelper, arg);
    }
    clock_gettime(CLOCK_MONOTONIC, &tcreation_end);

    for (int i = 0; i < p; i ++){
        pthread_join(threads[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &finish_end);

    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("The number of solution is %d\n", totalcount);
    printf("The max profit is %d\n", profitmax);
    printBoard(board_max, n);
    total_time =
    BILLION *(end.tv_sec - start.tv_sec) +(end.tv_nsec - start.tv_nsec);
    total_time = total_time / BILLION;
    tcreation_time =
    BILLION *(tcreation_end.tv_sec - start.tv_sec) +(tcreation_end.tv_nsec - start.tv_nsec);
    tcreation_time = tcreation_time / BILLION;

    double cal_max_time = 0;
    for (int i = 0; i < tnum; i ++) {
        double cal_time = calc_time_arr[i];
        if (cal_time > cal_max_time) {
            cal_max_time = cal_time;
        }
    }

    finish_time = (total_time - tcreation_time - cal_max_time > 0) ? (total_time - tcreation_time - cal_max_time) : 0;
    
    printf("Elapsed:              %lf seconds\n", total_time);
    printf("Thread creation time: %lf seconds\n", tcreation_time);
    printf("Computation time:     %lf seconds\n", cal_max_time);
    printf("Finish time:          %lf seconds\n", finish_time);

    free(threads);
    free(board_max);
    return 0;
}