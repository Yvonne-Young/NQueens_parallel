// Date: 10/17/2020
// N-Queens problem solved by backtracking (no parallelism)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BILLION 1000000000L

int count       = 0;
int profit      = 0;
int profit_best = 0;

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
void solveHelper(int ** board, int n, int col,int **opt_board) {
    if (col == n) {
        if (profit > profit_best){
            profit_best = profit;
            for (int i = 0; i < n; i ++) {
                for (int j = 0; j < n; j ++) {
                opt_board[i][j] = board[i][j];
                }
            }
        }
        count ++;
        return;
    }
    for (int row = 0; row < n; row ++) {
        if (isSafe(board, row, col, n) == 1) {
            board[row][col] = 1;
            profit += abs(col - row);
            solveHelper(board, n, col + 1, opt_board);
            board[row][col] = 0;
            profit -= abs(col - row);
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
void solveNQueens(int n) {
    int ** board = (int **)malloc(n * sizeof(int *));
    int ** opt_board = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i ++) {
        board[i]    = (int *)malloc(n * sizeof(int));
        opt_board[i]= (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j ++) {
            board[i][j] = 0;
            opt_board[i][j] = 0;
        }
    }
    solveHelper(board, n, 0, opt_board);
    printBoard(opt_board,n);
}

int main(int argc, char **argv) {
    int n, p;
    double time;
    struct timespec start, end;
    if (argc != 3) {
        printf("Usage: nqueens n p\nAborting...\n");
        exit(0);
    }
    n = atoi(argv[1]);
    p = atoi(argv[2]);
    printf("n = %d\np = %d\n", n, p);
    fflush(stdout);
    clock_gettime(CLOCK_MONOTONIC, &start);
    solveNQueens(n);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("number of solutions: %d\n", count);
    printf("The best profit is %d\n",profit_best);

    time =
    BILLION *(end.tv_sec - start.tv_sec) +(end.tv_nsec - start.tv_nsec);
    time = time / BILLION;
    
    printf("Elapsed: %lf seconds\n", time);
    return 0;
}