// CS 309 HW 4 Submission
// Parallel simulation of Conway's Game of Life using pthreads
// And a barrier implemented with two condition variables 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
 
int* threads_arg;
char** firstBoard;
char** nextBoard;
char** temp;
float randyrand;
static pthread_mutex_t lifeMutex;
static pthread_cond_t cond_var_even;
static pthread_cond_t cond_var_odd;
int numThreads;
int numRounds;
int numRows;
int numCols;
int maxThreads;
int numArgs = 0;
int currRound = 0;
 
struct threadstruct {
 int begin_row;
 int end_row;
 int ithread;
};

void printBoard(char** toPrint, int args, int numRows, int numCols);

// function which searches the input range ex 0 5
// of the matrix and writes to the next round board
// applying the rules of Conway's Game of life

void updatePortion(int first, int second){
  int liveCounter = 0;
    for(int i=first; i<second; i++){
      for(int j=0; j<numCols; j++){
        liveCounter = 0;
        if(i>0 && j>0 && firstBoard[i-1][j-1] == 'X'){
          liveCounter++;
        }
        if(i>0 && firstBoard[i-1][j] == 'X'){
          liveCounter++;
        }
        if(i>0 && j<numCols-1 && firstBoard[i-1][j+1] == 'X'){
          liveCounter++;
        }
        if(j>0 && firstBoard[i][j-1] == 'X'){
          liveCounter++;
        }
        if(j<numCols-1 && firstBoard[i][j+1] == 'X'){
          liveCounter++;        
        }
        if(i<numRows-1 && j>0 && firstBoard[i+1][j-1] == 'X'){
          liveCounter++;
        }
        if((i<numRows-1) && firstBoard[i+1][j] == 'X'){
          liveCounter++;
        }
        if(i<numRows-1 && j<numCols-1 && firstBoard[i+1][j+1] == 'X'){
          liveCounter++;
        }
        if(liveCounter==3){
          nextBoard[i][j] = 'X';
        }
        if(liveCounter==2 && firstBoard[i][j]=='X'){
          nextBoard[i][j] = 'X';
        }
        if(liveCounter < 2 || liveCounter > 3){
          nextBoard[i][j] = '.';
        }
        if(liveCounter==2 && firstBoard[i][j]=='.'){
          nextBoard[i][j] = '.';
        }
   } 
 }
}

void* timestep(void *arg){
  struct threadstruct *actual_arg = arg;
  int currthread = ((struct threadstruct*)actual_arg)->ithread;
  int begin = ((struct threadstruct*)actual_arg)->begin_row;
  int end = ((struct threadstruct*)actual_arg)->end_row;
  // each step simulates the rounds, stopping 
  for(int g = 0; g<numRounds; g++){
    updatePortion(begin, end);   
    // BARRIER
    pthread_mutex_lock(&lifeMutex); 
    currRound++;
    if(currRound==numThreads){
        currRound = 0;
        temp = firstBoard;
        firstBoard = nextBoard;
        nextBoard = temp;
        printBoard(firstBoard, numArgs, numRows, numCols);
        if(g%2==0){
          // broadcast for even step
          pthread_cond_broadcast(&cond_var_even);}
        else{
          pthread_cond_broadcast(&cond_var_odd);
      }
    }  
    else {
      if(g%2==0){
        // Wait for even step
        while(pthread_cond_wait(&cond_var_even, &lifeMutex)!=0);}
      else{
        // wait for odd step
        while(pthread_cond_wait(&cond_var_odd, &lifeMutex)!=0);}
    }
    pthread_mutex_unlock(&lifeMutex);
  }
  return 0;
}

// Function which prints the board if debugging is selected
// Prints only the live count if debugging isn't selected
 
void printBoard(char** toPrint, int args, int numRows, int numCols){
 int z = 0;
 if(args!=7){
    for(int d=0; d<numRows; d++){
       for(int b=0; b<numCols; b++){
         if(toPrint[d][b]=='X')
          z++;
       }
   } 
   printf("\n");
   printf("Number of living cells %d", z);
   printf("\n");
   }
 else{
     for(int t=0; t<numRows; t++){
       printf("\n");
       for(int q=0; q<numCols; q++){
         printf("%C", toPrint[t][q]);
         if(toPrint[t][q]=='X')
         z++;
       }
     } 
  printf("\n");
  printf("Live count = %d", z);
  printf("\n");
  printf("-------------------");
  printf("\n");
 }
}
 
// allocates (mallocs) a 2-D Array/matrix and returns the matrix 

char** initializeBoard(int rows, int cols, float likely){
 int i;
 char** board = (char**) malloc(cols * sizeof(char*));
 for(int i=0; i<cols; i++)
    board[i] = (char*) malloc(rows*sizeof(char));
 
 for(int j=0; j<rows; j++){
   for(int r=0; r<cols; r++){
     float randyrand = (float)rand() / RAND_MAX;
     if(randyrand>likely){
      board[j][r] = '.';
     }
     else{
      board[j][r] = 'X';
     }
   }
 }
 return board;
}
 
int main(int argc, char** argv)  {
// read command line input ex 4 4 0.5 2 2 y 
 numRows = atoi(argv[1]);
 numCols = atoi(argv[2]);
 float likelihood = atof(argv[3]);
 numRounds = atoi(argv[4]);
 numThreads = atoi(argv[5]);
 numArgs = argc;
 int maxThreads = 0;
 int liveCounter = 0;
 char** temp;
 // Initalize / allocate the 2-D arrays
 firstBoard = initializeBoard(numRows, numCols, likelihood);
 nextBoard = initializeBoard(numRows, numCols, 0.7);
 if(numRows>numCols)
   maxThreads = numRows;
 else
   maxThreads = numCols;
 if(numThreads>maxThreads){
   printf("Too many threads, program could only efficiently utilize  %d", maxThreads);
   printf("threads. The number of threads has been reduced accordingly. \n");
   numThreads = maxThreads;
 }

 pthread_t threadArray[numThreads];
 struct threadstruct structArray[numThreads];
 pthread_mutex_t lifeMutex= PTHREAD_MUTEX_INITIALIZER;
 pthread_cond_t cond_var_even = PTHREAD_COND_INITIALIZER;
 pthread_cond_t cond_var_odd = PTHREAD_COND_INITIALIZER;
 //printf("The initial state: \n");
 printBoard(firstBoard, argc, numRows, numCols);
 for(int e=0; e<numThreads; e++){
  int bound = numRows / numThreads;
  int start = e * bound;
  int end = (e+1)*bound; 
  if(e == numThreads-1)
    end = numRows;
  structArray[e].begin_row = start;
  structArray[e].end_row = end;
  structArray[e].ithread = e;
  pthread_create(&threadArray[e], NULL, timestep, (void*)&structArray[e]);
}

// Join all threads

for(int f=0; f<numThreads;f++){
  if(pthread_join(threadArray[f], NULL))
    printf("failed join \n");
}

// Free everything allocated with malloc  

for(int t=0; t<numRows; t++){
       free(firstBoard[t]);
       free(nextBoard[t]);}
free(firstBoard);
free(nextBoard);
}