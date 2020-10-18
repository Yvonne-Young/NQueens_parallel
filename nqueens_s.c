// Date: 10/17/2020
// N-Queens problem solved by backtracking (no parallelism)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int count = 0;

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
void solveHelper(int ** board, int *** res, int n, int col) {
    if (col == n) {
        // memcpy(res[count++][0], board[0], sizeof(board));
        for (int i = 0; i < n; i ++) {
            for (int j = 0; j < n; j ++) {
                res[count][i][j] = board[i][j];
            }
        }
        count ++;
        return;
    }
    for (int row = 0; row < n; row ++) {
        if (isSafe(board, row, col, n) == 1) {
            board[row][col] = 1;
            solveHelper(board, res, n, col + 1);
            board[row][col] = 0;
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
    int *** res = (int ***)malloc(1000 * sizeof(int **));
    int ** board = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i ++) {
        board[i] = (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j ++) {
            board[i][j] = 0;
        }
    }
    for (int i = 0; i < 1000; i ++) {
        res[i] = (int **)malloc(n * sizeof(int *));
        for (int j = 0; j < n; j ++) {
            res[i][j] = (int *)malloc(n * sizeof(int));
        }
    }
    solveHelper(board, res, n, 0);
    int res_size = sizeof(res) / (n * sizeof(int**));
    printf("number of solutions: %d\n", count);
    fflush(stdout);
    for (int i = 0; i < count; i ++) {
        printBoard(res[i], n);
        printf("===============\n");
    }
}

int main(int argc, char **argv) {
    int n, p;
    if (argc != 3) {
        printf("Usage: nqueens n p\nAborting...\n");
        exit(0);
    }
    n = atoi(argv[1]);
    p = atoi(argv[2]);   
    printf("n = %d\np = %d\n", n, p);
    fflush(stdout);
    solveNQueens(n);
    return 0;
}
