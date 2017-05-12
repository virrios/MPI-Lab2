#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

#define DEBUG 0
#define DEBUG2 0
#define EXTRA_DEBUG 0
#define EXTRA_DEBUG2 0
#define EXTRA_DEBUG3 0

void
broadcast_hypercube(void     *buffer,
               int       count,
               int       root,
               MPI_Comm  comm)
{

	// HYPERCUBE

    MPI_Datatype datatype = MPI_INT; // Use this MPI function calls.

    // Implementation goes here.

    int i,powerOf2ToK,k,j, p, myrank;
	int active,mask, partner;
	int currenMessageToSend[count];

    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	// rank 0 makes the message to send 
	if(myrank ==0){
		for(j = 0;j<count;j++){
			currenMessageToSend[j] = ((int *)buffer)[j];
		}
	}

	mask = p-1;
	int start = (int)(log10(p)/log10(2));


	for(k = start-1;k>=0; k--){
		powerOf2ToK = pow(2,k);
		mask = mask ^ powerOf2ToK;
		active = (myrank & mask);

		if(active==0){
			partner = myrank ^powerOf2ToK;
			if((myrank & powerOf2ToK) == 0){
				MPI_Send(&currenMessageToSend,count,datatype,partner,0,MPI_COMM_WORLD);
			}
			else{
		 	    MPI_Recv(currenMessageToSend,count,datatype,partner,0,MPI_COMM_WORLD,&status);
				//used to store current message in buffer.
				for(j = 0;j<count;j++){
					((int *)buffer)[j] =currenMessageToSend[j];
				}
			} 
	
		}
	}

}



void
broadcast_ring(void     *buffer,
               int       count,
               int       root,
               MPI_Comm  comm)
{
    MPI_Datatype datatype = MPI_INT; // Use this MPI function calls.

    // Implementation goes here.
	// RING 

    int i,powerOf2ToK,k,j, p, myrank;
	int sender,reciever;
	int currenMessageToSend[count];

    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);



	
	if(myrank ==0){
		for(j = 0;j<count;j++){
			currenMessageToSend[j] = ((int *)buffer)[j];
		}
	}
	

	for(i =0;i<p-1;i++){
		sender = i;
		reciever = (i+1 )% p;
		if(myrank ==sender){
			MPI_Send(&currenMessageToSend,count,datatype,reciever,0,MPI_COMM_WORLD);
		}
		if(myrank == reciever){
			MPI_Recv(currenMessageToSend,count,datatype,sender,0,MPI_COMM_WORLD,&status);
			for(j = 0;j<count;j++){
				((int *)buffer)[j] =currenMessageToSend[j];
			} 
		}
	}
}


void
broadcast_mesh(void     *buffer,
               int       count,
               int       root,
               MPI_Comm  comm)
{
    MPI_Datatype datatype = MPI_INT; // Use this MPI function calls.

    // Implementation goes here.
	// MESH




    int i,powerOf2ToK,k,j, p, myrank;
    MPI_Status status;
	int sender,reciever;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);



	int sourceStep;
	int rootOfP = (int) sqrt(p);
	// if p is a square ( it has a integer root) we will use the sqrt as deciding source
	// otherwise we want the root to be the next multiple of 2s root.
	if (rootOfP *rootOfP ==p){
		sourceStep = rootOfP;
	}	
	else{
		sourceStep = (int) sqrt(p*2);
	}



	int currenMessageToSend[count];
	if(myrank ==0){

			for(j = 0;j<count;j++){
				currenMessageToSend[j] = ((int *)buffer)[j];
			}

	}
	
	// CONDITIONS.
	int youAreSource =0;
	int hasRecieved =0;
	int youAreRecieverFromSource=0;
	int specialrank1 = 1;	// to make sure rank 0 does not send twice
	
	int sourceReciever;
	int layer,start,end;
	
	if(myrank ==0){
		hasRecieved =1;  // ROOT source so it has already got the message
	}

	if(myrank % sourceStep ==0){   		// is it a canditate for being a parallel point of interest.

		for(i =0;i<2;i++){       		// only need two o iterations at most.
			if(myrank + sourceStep< p){
				youAreSource = 1;  		// edge case for last canditate of point of interst, should only send to directly next.
				sourceReciever = myrank + sourceStep;   // next parallel point of interest to send to.
			}			
			if(hasRecieved ==0){
				youAreRecieverFromSource=1;	// if you have not recieve anything yet, you are a reciever
			}
			if( youAreSource &&  hasRecieved && specialrank1){  // if you have a message and you are a source, send (specialrank1 if myrank =0)
					MPI_Send(&currenMessageToSend,count,datatype,sourceReciever,0,MPI_COMM_WORLD);
					if(myrank ==0){
						specialrank1=0;   // to make sure it does rank 0 send twice and creates a deadlock situation.
					}

			}
			if(youAreRecieverFromSource){
				sender = myrank - sourceStep;
				//printf("iam rank: %d, sender: %d\n",myrank,sender);
				MPI_Recv(currenMessageToSend,count,datatype,sender,0,MPI_COMM_WORLD,&status);
			//	printf("I HAVE Recieved:            %d from %d\n", myrank,sender);
				hasRecieved = 1;     		   // will be a sender next time (if not edge case)
				youAreRecieverFromSource=0;   //should only recieve once.
				for(j = 0;j<count;j++){
					((int *)buffer)[j] =currenMessageToSend[j];
				} 
			

			}


		}
	
	}

	// layer, etc in a 16dim mesh, 0-3 = layer 0, 4-7 = layer 1,....12-15 = layer 3.
	//  starting point is the first node in the layer, so amount nodes per layer * layer, example mesh dim 16 sourceStep=4, start for layer 0 = 0*4, layer 1 = 1*4..layer 2 = 2*4.

	// end is ofcourse start + number of node per layer-1 since the last node will not send..	

	layer = myrank/sourceStep;
	start = layer*sourceStep;
	end =  start + sourceStep-1;

	for(k =start;k< end;k++){

		sender = k;
		reciever = (k+1) %p;
		if(myrank ==sender && (reciever<(layer+1)*sourceStep) && (reciever != 0) ){

			MPI_Send(&currenMessageToSend,count,datatype,reciever,0,MPI_COMM_WORLD); // last condition for edge case example 7 should not send to 8 in a 16 dim mesh.
		}
		if(myrank == reciever && (reciever%sourceStep!=0)){  // last condition edge case, should not send to rank 0.
			MPI_Recv(currenMessageToSend,count,datatype,sender,0,MPI_COMM_WORLD,&status);
			for(j = 0;j<count;j++){
				((int *)buffer)[j] =currenMessageToSend[j];
			} 
		}

	}

}




int main(int argc, char *argv[])
{
    if ( argc < 2 )
        exit(-1);

    MPI_Init(&argc, &argv);

    int i,l, cur_msg_len, p, myrank;
	double t1, t2, time; 
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    int max_msg_len = atoi(argv[1]);
    int nr_tests    = atoi(argv[2]);

    int message[max_msg_len];

    if ( myrank == 0 )
       for (i = 0; i < max_msg_len; i++) message[i] = i;

    for (cur_msg_len = 1; cur_msg_len <= max_msg_len; cur_msg_len *= 2)
    {
        // MPI_Wtime() -> take start time on rank 0
		/*	
		if(myrank==0){
			t1 = MPI_Wtime(); 
		}
		*/
        for (i = 0; i < nr_tests; i++)
        {
           // MPI_Bcast(message, cur_msg_len, MPI_INT, 0, MPI_COMM_WORLD);
           broadcast_ring(message, cur_msg_len, 0, MPI_COMM_WORLD);
         // broadcast_mesh(message, cur_msg_len, 0, MPI_COMM_WORLD);
           broadcast_hypercube(message, cur_msg_len, 0, MPI_COMM_WORLD);
        }
        // MPI_Wtime() -> take end time on rank 0
        // Calculate on rank 0:

		/*
		if(myrank==0){
			t2 = MPI_Wtime(); 
		    time = ( t2 - t1 )/nr_tests;
			printf( "Elapsed time is %f, number of Tests: %d\n", time,nr_tests );
		}
        // print message length and time
		*/

    }
    MPI_Finalize();
    return 0;
}
