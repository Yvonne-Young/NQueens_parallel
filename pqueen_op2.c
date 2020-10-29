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

typedef struct{
    int n,p,firstrow,secondrow,pos_first,pos_second;
    int ** board_max;
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
void solveHelper(int ** board,int n,int col,int **opt_board,int *count,int *profit,int*profit_best,int i){
    if (col == n) {
        // memcpy(res[count++][0], board[0], sizeof(board));
        //int temp=*count;
        /*
        for (int i = 0; i < n; i ++) {
            for (int j = 0; j < n; j ++) {
                res[temp][i][j] = board[i][j];
            }
        }
        */
        
        if (*profit>*profit_best){
            *profit_best=*profit;
            for(int i=0;i<n;i++){
                for(int j=0;j<n;j++){
                    opt_board[i][j] = board[i][j];
                }
            }
        }
        
        if (i== (n+1)/2 - 1 && (n%2!=0) ){
            *count = *count + 1;         //it is deal with the mid row
        }  
        else {
            *count = *count + 2;
        }
        
        //*count = *count + 2;
        
        //printf("count increase by 1,now is %d\n",*count);
        return;
    }
    for (int row = 0; row < n; row ++) {
        if (isSafe(board, row, col, n) == 1) {
            board[row][col] = 1;
            *profit+=abs(col-row);
            //printf("row = %d, col= %d is safe!\n",row,col);
            solveHelper(board, n, col + 1,opt_board,count,profit,profit_best,i);
            //printf("Remove row = %d, col= %d\n",row,col);
            board[row][col] = 0;
            *profit-=abs(col-row);
            //printf("check count %d\n",*count );
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
void solveNQueens(int n,int p,int **board_max,int firstrow,int secondrow,int pos_first,int pos_second) {
    //int *** res         = (int ***)malloc(100000 * sizeof(int **));
    int ** board        = (int **)malloc(n * sizeof(int *));
    int *  count        = (int *)malloc(sizeof(int));
    int ** opt_board    = (int**)malloc(n * sizeof(int *));
    //int ** board_max    = (int**)malloc(n * sizeof(int *));
    int *  profit       = (int *)malloc(sizeof(int));
    int *  profit_best  = (int *)malloc(sizeof(int));

    //a counter for each thread
     * count=0;
     //* profit=0;
     * profit_best=0;
    for (int i = 0; i < n; i ++) {
        board[i] = (int *)malloc(n * sizeof(int));
        opt_board[i]= (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j ++) {
            board[i][j] = 0;
            opt_board[i][j]=0;
        }
    }
    int iteration = (n+1)/2;
    for (int i=pos_first;i<iteration;i+=firstrow)
    {
        board[i][0]=1;
        *profit+=+i;
        for (int j=pos_second; j<n;j+=secondrow){
            if( isSafe(board,j,1,n)==1 ){
                board[j][1]=1;
                *profit+=abs(j-1);
                solveHelper(board, n, 2,opt_board,count,profit,profit_best,i);
                board[j][1]=0;
                *profit-=abs(j-1);
            }
        }
        //solveHelper(board, n, 1,opt_board,count,profit,profit_best,i);
        board[i][0]=0;
        *profit-=i;
    }
    //solveHelper(board, res, n, 1);
    //int res_size = sizeof(res) / (n * sizeof(int**));
    //int iteration=(n+1)/2;
    //printf("Now iteration is %d\n",iteration);
    
    pthread_mutex_lock(&lock);
    totalcount+=*count;
    if (*profit_best>profitmax){
        profitmax = *profit_best;
        for (int i = 0; i < n; i ++) {
                for (int j = 0; j < n; j ++) {
                board_max[i][j] = opt_board[i][j];
                }
            }
    }
    pthread_mutex_unlock(&lock);
    //printBoard(board_max,n); 
    //printf("===============\n");
    //printf("number of solutions: %d\n", *count);
    //printf("totalcount is %d\n",totalcount);
    fflush(stdout);

    /*
    for (int i = 0; i < count; i ++) {
        printBoard(res[i], n);
        printf("===============\n");
    }
    */
}

void *nqueensHelper(void *varg){
    GM *arg =varg;
    int pid,n,p,count,firstrow,secondrow,pos_first,pos_second;
    int **board_max;
    //pid  = arg->pid;
    p    = arg->p;
    n    = arg->n;
    firstrow=arg->firstrow;
    secondrow=arg->secondrow;
    pos_first=arg->pos_first;
    pos_second=arg->pos_second;
    board_max=arg->board_max;
    solveNQueens(n,p,board_max,firstrow,secondrow,pos_first,pos_second); 
    return NULL;
}

int main(int argc, char **argv) {
    int n, p;
    double total_time, tcreation_time, execution_time, finish_time;
    struct timespec start, tcreation_end, execution_end, program_end;
    if (argc != 3) {
        printf("Usage: nqueens n p\nAborting...\n");
        exit(0);
    }
    n = atoi(argv[1]);
    p = atoi(argv[2]);
    printf("n = %d\np = %d\n", n, p);
    fflush(stdout);
    int ** board_max    = (int**)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i ++) {
        board_max[i]= (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j ++) {
            board_max[i][j]=0;
        }
    }
    /*modify the parallel version*/
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_t *threads = malloc (p* sizeof(threads));
    if (p>1){
    	for (int i=0;i<2;i++){      //check 2 position in first row each time
        	for (int j=0;j<p/2;j++){ 
            	GM *arg =malloc(sizeof(*arg));
            	arg->n=n;
            	//arg->pid=i*2+j;
            	arg->p=p;
            	arg->board_max=board_max;
            	arg->firstrow= 2;
            	arg->secondrow= p/2;
            	arg->pos_first=i;
            	arg->pos_second=j;
            	pthread_create(&threads[i*p/2+j],NULL,nqueensHelper,arg);
        	}
    	}
    }
    else{
    	for (int i = 0; i < p; i ++){
        	GM *arg = malloc(sizeof(*arg));
        	arg->n = n;
        	//arg->pid = i;
        	arg->p = p;
        	arg->board_max  = board_max;
        	arg->firstrow   = 1;
        	arg->secondrow  = p;
        	arg->pos_first  = i;
        	arg->pos_second = 0;
        	pthread_create(&threads[i], NULL, nqueensHelper, arg);
    }
    }

    clock_gettime(CLOCK_MONOTONIC, &tcreation_end);

    for (int i=0;i<p;i++){
        pthread_join(threads[i],NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &execution_end);

    printf("The number of solution is %d\n", totalcount);
    printf("The max profit is %d\n", profitmax);
    printBoard(board_max, n);

    free(threads);
    free(board_max);

    printf("The number of solution is %d\n",totalcount);
    printf("The max profit is %d\n",profitmax);

    tcreation_time =
    BILLION *(tcreation_end.tv_sec - start.tv_sec) +(tcreation_end.tv_nsec - start.tv_nsec);
    tcreation_time = tcreation_time / BILLION;

    execution_time =
    BILLION *(execution_end.tv_sec - tcreation_end.tv_sec) +(execution_end.tv_nsec - tcreation_end.tv_nsec);
    execution_time = execution_time / BILLION;

    clock_gettime(CLOCK_MONOTONIC, &program_end);

    finish_time = 
    BILLION * (program_end.tv_sec - execution_end.tv_sec) + (program_end.tv_nsec - execution_end.tv_nsec);
    finish_time = finish_time / BILLION;

    total_time = 
    BILLION * (program_end.tv_sec - start.tv_sec) + (program_end.tv_nsec - start.tv_nsec);
    total_time = total_time / BILLION;

    printf("Elapsed:              %lf seconds\n", total_time);
    printf("Thread creation time: %lf seconds\n", tcreation_time);
    printf("Computation time:     %lf seconds\n", execution_time);
    printf("Finish time:          %lf seconds\n", finish_time);


    return 0;
}