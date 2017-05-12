/*



	

	if my rank = 0
		i) create b dim mx1
		i) create M dim nxm 
		broadcast b
		scatter m each proccess geting a 1xm row.


	do multiplication 
	
	if my rank =0
		gather



	broadcast resultat
		to all


*/


#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

#define DEBUG_RESULT 0
#define DEBUG_CODE 0
#define EXTRA_DEBUG 1

int multiply(int* vector, int *b,int m, int rank)
{
	int i;
	int c=0;
	printf("vector, rank %d\n",rank);
	for(i=0; i<m;i++){

		printf("%d ",vector[i]);
		c += vector[i]*b[i];
	}
	printf("\n");

return c;
}




  

int main(int argc, char *argv[])
{
    MPI_Init(NULL, NULL);

    int numberOfProcesses;
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses); //Get number of processes.
    int m = 1, n=1;
	int i,j;
    if(argc > 1) {
         m = strtol(argv[1],NULL,10);
    }
	n = numberOfProcesses;
	int b [m];
	int recievedRow[m];
	int row[m];
    int rank;
	int matrix [n*m];

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if(rank ==0){

		srand(time(NULL) + rank);
    	int value;
		
		//creating matrix m and row vector b
		int sum=0;
		int index=0;
		int temp =0;
		int realIndex;
		for(i =0; i<n;i++){
			index = index + temp;
			for(j=0;j<m;j++){
				//value = (rand() % (m*n));
				temp = j;
				sum= sum+1;
				realIndex = index+temp;
				printf("sum: %d index %d, temp %d totindex %d\n",sum,index,temp,(index+temp));
				matrix[realIndex] = realIndex;
				}
			temp++;
			value = (rand() % (m*n));
			b[i] = value;
		}
		
		if(DEBUG_CODE || EXTRA_DEBUG){

			index=0;
			 temp =0;
			

			printf("matrix\n");
			for(i =0; i<n;i++){
				index = index + temp;	
				for(j=0;j<m;j++){
					temp = j;
					realIndex = index+temp;
					printf("%d ",matrix[realIndex]);
				}
				temp++;	
				printf("\n");
			}

			printf("b\n");
			for(j=0;j<m;j++){
				printf("%d\n",b[j]);
			}			
		}

	}

		// bcast b
		MPI_Bcast(&b,m,MPI_INT,0,MPI_COMM_WORLD);
		
		// scatter cast matrix
		MPI_Scatter(&matrix ,m,MPI_INT,&recievedRow,m,MPI_INT,0,MPI_COMM_WORLD);
		/*
			void* send_data, matrix, 
			int send_count, m
			MPI_Datatype send_datatype, MPI_FLOAT
			void* recv_data,  //* recievedRow
			int recv_count,   //* m 
			MPI_Datatype recv_datatype, MPI FLOAT
			int root, 0
			MPI_Comm communicator) MPU COMMWORLD
		*/


	if(DEBUG_CODE){
		printf("recieved b\n");
		for(i=0;i<m;i++){
			printf("rank: %d value %d\n",rank,b[i]);

		}
	}

	
	if(EXTRA_DEBUG){
		printf("recieved Row\n");
		for(i=0;i<m;i++){
			printf("rank: %d rowvalue %d\n",rank,recievedRow[i]);
		}
	}

	
	int result = multiply(recievedRow,b,m,rank);
	printf("rank: %d, result value of multiply: %d\n",rank,result);
	
	MPI_Gather(&result,1,MPI_INT,&recievedRow,1,MPI_INT,0,MPI_COMM_WORLD);
	

	if(DEBUG_CODE && rank ==0){
		printf("recieved Resulting Row\n");
		for(i=0;i<m;i++){
			printf("rank: %d rowvalue %d\n",rank,recievedRow[i]);
		}
	}

	/*,
void* send_data,
    int send_count, one element result
    MPI_Datatype send_datatype, mpi_int
    void* recv_data,			recievedROq
    int recv_count,				m
    MPI_Datatype recv_datatype,
    int root,
    MPI_Comm communicator)
	*/
   	//broadcast resulting vector to all other processes
	MPI_Bcast(&recievedRow,m,MPI_INT,0,MPI_COMM_WORLD);	
	printf("Rank %d has recived final result\n",rank);
		for(i=0;i<m;i++){
			printf("rank: %d rowvalue %d\n",rank,recievedRow[i]);
		}
	


    MPI_Finalize();
return 0;
}
