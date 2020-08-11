#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<time.h>
#include <stdbool.h>
#include "mpi.h"
#include <unistd.h>

int main(int argc, char **argv)
{

    //Usual MPI init
    MPI_Init(&argc, &argv);
    int rank, numranks;
    MPI_Comm_size(MPI_COMM_WORLD, &numranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Status stat;
    
    double startTime = MPI_Wtime();
    int usleep(useconds_t usec);

    int numDigits = 6;
    int maxNum = 1;
    int pin;

    for( int i = 0; i < numDigits; i++){
        maxNum = maxNum*10;
    }
    maxNum = maxNum - 1;

    

    if(rank == 0){
        srand(time(NULL));
        pin = rand() % maxNum;
    }

    MPI_Bcast(&pin,1,MPI_INT,0,MPI_COMM_WORLD);
    
    int numTrys = maxNum / numranks;
    int r = maxNum % numranks;
    int myStart = rank * numTrys;
    int myEnd = myStart + numTrys;
    
    if(rank == numranks - 1){
        myEnd = myEnd + r;
    }

    bool searching = true;
    int foundPin;

    MPI_Request recv; 
    MPI_Irecv(&searching,1,MPI_C_BOOL,MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD,&recv);
    
    int i;

    for( i = myStart; i < myEnd; i++){
            
       if(searching){

            usleep(30); 

           if( i == pin){

                foundPin = i;
                searching = false;

                for(int j = 0; j < numranks - 1; j++){
                    if(j != rank){
                        MPI_Send(&searching,1,MPI_C_BOOL,j,0,MPI_COMM_WORLD);
                        
                    }
                }

                printf("RANK %d FOUND THE PIN! It is: %d\r\n",rank,foundPin);
           }
       }

       else{

            printf("Rank %d checked %d combos\r\n",rank,i);
            i = myEnd;

       }     

    }

  
    MPI_Cancel(&recv);

    double endTime = MPI_Wtime();

    double calcTime = endTime - startTime;

    if(rank == 0){
        printf("Took %.12f Sec\r\n",calcTime);
    }
 
  
    MPI_Finalize();
}
