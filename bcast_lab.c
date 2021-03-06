#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

#define DEBUG 0
#define DEBUG2 1
#define EXTRA_DEBUG 0
#define EXTRA_DEBUG2 0
#define EXTRA_DEBUG3 0
void
broadcast_ring(void     *buffer,
               int       count,
               int       root,
               MPI_Comm  comm)
{
    MPI_Datatype datatype = MPI_INT; // Use this MPI function calls.

    // Implementation goes here.
    int i,j, p, myrank;
    MPI_Status status;



    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	int currenMessageToSend[count];
	if(myrank ==root){

			for(j = 0;j<count;j++){
				currenMessageToSend[j] = ((int *)buffer)[j];
			}
		
		for(i =1;i<p;i++){
			MPI_Send(&currenMessageToSend,count,datatype,i,root,MPI_COMM_WORLD);

		}
	}
	else
	{
 	   MPI_Recv(currenMessageToSend,count, MPI_FLOAT,root,0,MPI_COMM_WORLD,&status);
			if(EXTRA_DEBUG3){
				printf("rank %d CURREN RECIEEEVED MESSAGE \n",myrank);
				}
			for(j = 0;j<count;j++){
				((int *)buffer)[j] =currenMessageToSend[j];

				if(EXTRA_DEBUG3){
					printf("current  revieced message: %d\n", currenMessageToSend[j]); 
					printf("buffered recieved message: %d\n", ((int *)buffer)[j]); 
				}
			}
			if(EXTRA_DEBUG3){
				printf("\n");
			}

	}
	
}
void
broadcast_ring2(void     *buffer,
               int       count,
               int       root,
               MPI_Comm  comm)
{

	// HYPERCUBE

    MPI_Datatype datatype = MPI_INT; // Use this MPI function calls.

    // Implementation goes here.

    int i,powerOf2ToK,k,j, p, myrank;
    MPI_Status status;
	int active,mask, partner;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);


	int currenMessageToSend[count];
	if(myrank ==0){

			for(j = 0;j<count;j++){
				currenMessageToSend[j] = ((int *)buffer)[j];
			}

	}

	mask = p-1;
	//printf("number of processes: %d\n",p);
	int start = (int)(log10(p)/log10(2));
//	printf("start: %d myrank: %d\n",start,myrank);

	for(k = start-1;k>=0; k--){
		//printf("k: %d                                         RANK %d\n",k,myrank);
		powerOf2ToK = pow(2,k);
		mask = mask ^ powerOf2ToK;
			//printf("mask: %d k: %d\n",mask,k);
		active = (myrank & mask);
		if(active==0){
			partner = myrank ^powerOf2ToK;
			//printf("current %d partner %d\n",myrank,partner);
			if((myrank & powerOf2ToK) == 0){
				//printf("Send to: %d, from: %d\n",partner,myrank);
				MPI_Send(&currenMessageToSend,count,datatype,partner,0,MPI_COMM_WORLD);
				printf("I HAVE SENT:            %d to %d\n", myrank,partner);
			}//end if
			else{
				//printf("myrank: %d recieved from: %d\n",myrank,partner);
		 	   MPI_Recv(currenMessageToSend,count,datatype,partner,0,MPI_COMM_WORLD,&status);
				//printf("I HAVE Recieved:            %d from %d\n", myrank,partner);
				for(j = 0;j<count;j++){
					((int *)buffer)[j] =currenMessageToSend[j];
				} // end for
			} //end elseif
	
		}//end if	
	}// end for






	
}



void
broadcast_ring3(void     *buffer,
               int       count,
               int       root,
               MPI_Comm  comm)
{
    MPI_Datatype datatype = MPI_INT; // Use this MPI function calls.

    // Implementation goes here.
	// RING 

    int i,powerOf2ToK,k,j, p, myrank;
    MPI_Status status;
	int sender,reciever;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);


	int currenMessageToSend[count];
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
			printf("I HAVE SENT:            %d to %d\n", myrank,reciever);
		}
		if(myrank == reciever){
			MPI_Recv(currenMessageToSend,count,datatype,sender,0,MPI_COMM_WORLD,&status);
			//printf("I HAVE Recieved:            %d from %d\n", myrank,sender);
			for(j = 0;j<count;j++){
				((int *)buffer)[j] =currenMessageToSend[j];
			} 

		}



	}

}



void
broadcast_ring4(void     *buffer,
               int       count,
               int       root,
               MPI_Comm  comm)
{
    MPI_Datatype datatype = MPI_INT; // Use this MPI function calls.

    // Implementation goes here.
	// MESH

	// if p is a square ( it has a integer root) we will use the sqrt as deciding source


    int i,powerOf2ToK,k,j, p, myrank;
    MPI_Status status;
	int sender,reciever;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);



	int sourceStep;
	int rootOfP = (int) sqrt(p);
	if (rootOfP *rootOfP ==p){
		sourceStep = rootOfP;
		//printf("source step: %d proccessors: %d\n",sourceStep,p); 
	}	
	else{
		sourceStep = (int) sqrt(p*2);
	//	printf("source step: %d proccessors: %d\n",sourceStep,p); 
	}



	int currenMessageToSend[count];
	if(myrank ==0){

			for(j = 0;j<count;j++){
				currenMessageToSend[j] = ((int *)buffer)[j];
			}

	}
	

	int youAreSource =0;
	int hasNotSendAsSource =1;
	int hasRecieved =0;
	int youAreRecieverFromSource=0;
	int sourceReciever;
	int DoneSendingToOtherSource=0;
	int DoneRecivingFromOtherSource=0;
	int layer,start,end;
	
	if(myrank ==0){
		hasRecieved =1;
		DoneRecivingFromOtherSource=1;

	}

	for(i =0;i<2;i++){ // kanske med myrank
		
		if((DoneSendingToOtherSource == 1 && DoneRecivingFromOtherSource==1) || (myrank % sourceStep !=0)){

				layer = myrank/sourceStep;
				start = layer*sourceStep;
				end =  start + sourceStep-1;
				//printf("layer: %d, start %d\n",layer,start);
				for(k =start;k< end;k++){
					sender = k;
					reciever = (k+1) %p;
					if(myrank ==sender){
					//	MPI_Send(&currenMessageToSend,count,datatype,reciever,0,MPI_COMM_WORLD);
					printf("I HAVE SENT(nextoNext):            %d to %d\n", myrank,reciever);
					}
					if(myrank == reciever){
					//	MPI_Recv(currenMessageToSend,count,datatype,sender,0,MPI_COMM_WORLD,&status);
						printf("I HAVE Recieved(nextonext:            %d from %d\n", myrank,sender);
						for(j = 0;j<count;j++){
							((int *)buffer)[j] =currenMessageToSend[j];
						} 

					}



				}




		}

		else{
			if(myrank % sourceStep ==0){        // if iam a canditade for being a source,
				if(myrank + sourceStep< p){
				youAreSource = 1;
				sourceReciever = myrank + sourceStep;
				}
				if(hasRecieved ==0){
				youAreRecieverFromSource=1;	
				}
			}
			if( youAreSource && hasNotSendAsSource && hasRecieved){
					//printf("hasrecieved: %d", hasRecieved);
					//printf("iam rank: %d, sender: %d\n",myrank,sender);
					//MPI_Send(&currenMessageToSend,count,datatype,sourceReciever,0,MPI_COMM_WORLD);
					printf("I HAVE SENT:            %d to %d\n", myrank,sourceReciever);
					hasNotSendAsSource =1;
					DoneSendingToOtherSource=1;

			}
			if(youAreRecieverFromSource){
				sender = myrank - sourceStep;
				//printf("iam rank: %d, sender: %d\n",myrank,sender);
				//MPI_Recv(currenMessageToSend,count,datatype,sender,0,MPI_COMM_WORLD,&status);
				printf("I HAVE Recieved:            %d from %d\n", myrank,sender);
				hasRecieved = 1;
				DoneRecivingFromOtherSource=1;
				for(j = 0;j<count;j++){
					((int *)buffer)[j] =currenMessageToSend[j];
				} 
			

			}


		}// else nopt done recieving sending from source.



	}//end for

}



void
broadcast_ring5(void     *buffer,
               int       count,
               int       root,
               MPI_Comm  comm)
{
    MPI_Datatype datatype = MPI_INT; // Use this MPI function calls.

    // Implementation goes here.
	// MESH

	// if p is a square ( it has a integer root) we will use the sqrt as deciding source


    int i,powerOf2ToK,k,j, p, myrank;
    MPI_Status status;
	int sender,reciever;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);



	int sourceStep;
	int rootOfP = (int) sqrt(p);
	if (rootOfP *rootOfP ==p){
		sourceStep = rootOfP;
		//printf("source step: %d proccessors: %d\n",sourceStep,p); 
	}	
	else{
		sourceStep = (int) sqrt(p*2);
	//	printf("source step: %d proccessors: %d\n",sourceStep,p); 
	}



	int currenMessageToSend[count];
	if(myrank ==0){

			for(j = 0;j<count;j++){
				currenMessageToSend[j] = ((int *)buffer)[j];
			}

	}
	

	int youAreSource =0;
	int hasRecieved =0;
	int youAreRecieverFromSource=0;
	int sourceReciever;
	int DoneRecivingFromOtherSource=0;
	int layer,start,end;
	
	if(myrank ==0){
		hasRecieved =1;
		DoneRecivingFromOtherSource=1;

	}


	int specialrank1 = 1;
	if(myrank % sourceStep ==0){ 


		for(i =0;i<2;i++){       // if iam a canditade for being a source,

		//printf(" mesh sending: myrank %d, iteration%d\n",myrank,i);
			if(myrank + sourceStep< p){
				youAreSource = 1;
				sourceReciever = myrank + sourceStep;
			}			
			if(hasRecieved ==0){
				youAreRecieverFromSource=1;	
			}
			if( youAreSource &&  hasRecieved && specialrank1){
					//printf("hasrecieved: %d\n", hasRecieved);
					//printf("iam rank: %d, sender: %d\n",myrank,sender);
					MPI_Send(&currenMessageToSend,count,datatype,sourceReciever,0,MPI_COMM_WORLD);
					if(EXTRA_DEBUG){
						printf("I HAVE SENT:            %d to %d\n", myrank,sourceReciever);
					}
					if(myrank ==0){
						specialrank1=0;
					}

			}
			if(youAreRecieverFromSource){
				sender = myrank - sourceStep;
				//printf("iam rank: %d, sender: %d\n",myrank,sender);
				MPI_Recv(currenMessageToSend,count,datatype,sender,0,MPI_COMM_WORLD,&status);
			//	printf("I HAVE Recieved:            %d from %d\n", myrank,sender);
				hasRecieved = 1;
				youAreRecieverFromSource=0;
				DoneRecivingFromOtherSource=1;
				for(j = 0;j<count;j++){
					((int *)buffer)[j] =currenMessageToSend[j];
				} 
			

			}


		}
	
	}


	layer = myrank/sourceStep;
	start = layer*sourceStep;
	end =  start + sourceStep-1;
	//printf("layer: %d, start %d\n",layer,start);
	for(k =start;k< end;k++){
		//printf(" ring sending: myrank %d, iteration%d start: %d end: %d\n",myrank,k,start,end);
		sender = k;
		reciever = (k+1) %p;
		//printf("recieever::::::: %d\n",reciever);
		if(myrank ==sender && (reciever<(layer+1)*sourceStep) && (reciever != 0) ){

			//printf("hasrecieved: %d\n", hasRecieved);
			//printf("iam rank: %d, sender: %d\n",myrank,sender);

			MPI_Send(&currenMessageToSend,count,datatype,reciever,0,MPI_COMM_WORLD);
			if(EXTRA_DEBUG){
				printf("I HAVE SENT(nextoNext):            %d to %d\n", myrank,reciever);
			}
		}
		if(myrank == reciever && (reciever%sourceStep!=0)){


		//	printf("iam rank: %d, sender: %d\n",myrank,sender);
			MPI_Recv(currenMessageToSend,count,datatype,sender,0,MPI_COMM_WORLD,&status);
			//printf("I HAVE Recieved(nextonext:            %d from %d\n", myrank,sender);
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
}

void
broadcast_hypercube(int      *buffer,
                    int       count,
                    int       root,
                    MPI_Comm  comm)
{
    MPI_Datatype datatype = MPI_INT; // Use this MPI function calls.

	


    // Implementation goes here.
}

int main(int argc, char *argv[])
{
    if ( argc < 2 )
        exit(-1);

    MPI_Init(&argc, &argv);

    int i,l, cur_msg_len, p, myrank;

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

		//printf("myrank is heree can it be other than zero %d\n",myrank);
        for (i = 0; i < nr_tests; i++)
        {
           // MPI_Bcast(message, cur_msg_len, MPI_INT, 0, MPI_COMM_WORLD);
           // broadcast_ring(message, cur_msg_len, 0, MPI_COMM_WORLD);
            //broadcast_ring2(message, cur_msg_len, 0, MPI_COMM_WORLD);
            //broadcast_ring3(message, cur_msg_len, 0, MPI_COMM_WORLD);
           // broadcast_ring4(message, cur_msg_len, 0, MPI_COMM_WORLD);
            broadcast_ring5(message, cur_msg_len, 0, MPI_COMM_WORLD);
            // broadcast_mesh(message, cur_msg_len, 0, MPI_COMM_WORLD);
            // broadcast_hypercube(message, cur_msg_len, 0, MPI_COMM_WORLD);
			if(EXTRA_DEBUG){
				printf("rank: %d message: \n",myrank);
				for(l = 0;l<cur_msg_len;l++){
					printf(" %d ",message[l]);

				}
					printf("\n");
			}

        }

		if(DEBUG){
				printf("rank: %d message: \n",myrank);
				for(l = 0;l<cur_msg_len;l++){
					printf(" %d ",message[l]);

				}
					printf("\n");
			}

        // MPI_Wtime() -> take end time on rank 0
        // Calculate on rank 0:
        // time = (end_time - start_time)/nr_tests;
        // print message length and time
    }
	if(DEBUG2){
				printf("rank: %d message:  ",myrank);
				for(l = 0;l<max_msg_len;l++){
					printf(" %d ",message[l]);

				}
					printf("\n");
			}

    MPI_Finalize();
    return 0;
}
