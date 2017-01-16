#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mpi.h>
//======================
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/eventfd.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <semaphore.h> //<--
#include <limits.h>
//======================
unsigned char dummy = 0;


typedef struct meta_data{
    
    int magic;
    sem_t meta_semaphore;
    char hostname[65];
    long memSize;
    long bitmapOffset;
    long numOfFrames;
    long frameSize;
    long metaSize; //Byte
}meta_data_t;


int main(int argc, char **argv)
{
	int i;
	int num_ranks;
	int remote_rank, my_rank;
	int length;
	int round;
	double timer = 0;

	MPI_Status status;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

	if(num_ranks != 2) {
		if(my_rank == 0) 
			fprintf(stderr, "Meta_test needs exactly two UEs; try again\n");
		exit(-1);
	}

	remote_rank = (my_rank + 1) % 2;

	printf("Rank: %d; PID: %d\n", my_rank, getpid());

int fd;
void * map_region;

	printf("[readMetadata] locking on file /dev/uio0\n");

	if ((fd=open("/dev/uio0", O_RDWR)) < 0){
		fprintf(stderr, "ERROR: cannot open file\n");
		exit(-1);
	}

	if ((map_region=mmap(NULL, 20000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 1*getpagesize()))<0){
		fprintf(stderr, "ERROR: cannot mmap file\n");
	} else {
		printf("[readMetadata] mapped to %p\n", map_region);
	}

sleep(2);

int a=0;
int b=1;
int c=2;


if(my_rank == 0){
printf("Test...\n");
    volatile meta_data_t *metadata = (volatile meta_data_t*) map_region;

    printf("Metadata:\n");
    printf("  -magic = %d\n", metadata->magic);
    printf("  -hostname = %s\n", metadata->hostname);
    printf("  -memSize = %lu\n", metadata->memSize);
    printf("  -bitmapOffset = %d\n", metadata->bitmapOffset);
    printf("  -numOfFrames = %d\n", metadata->numOfFrames);
    printf("  -frameSize = %d\n", metadata->frameSize);
    printf("  -metaSize = %d\n", metadata->metaSize);
}







//	sleep(10);


//	if(my_rank == 0) 
//		printf("#bytes\t\tusec\t\tMB/sec\n");

/*	if(my_rank == 0) {
		for(length=1; length <= maxlen; length*=2) {


			// synchronize before starting PING-PONG: 
			MPI_Barrier(MPI_COMM_WORLD);

			for(round=0; round<numrounds+WARM_UP; round++) {


				// send PING: 
				MPI_Ssend(send_buffer, 
				    	  length, 
					  MPI_CHAR, 
					  remote_rank, 
					  0, 
					  MPI_COMM_WORLD);

				// recv PONG:
				MPI_Recv(recv_buffer, 
				    	 length, 
					 MPI_CHAR, 
					 remote_rank, 
					 0, 
					 MPI_COMM_WORLD, 
					 &status);

				// start timer:
				if (round == WARM_UP-1)
					timer = MPI_Wtime();

			}

			// stop timer:
			timer = MPI_Wtime() - timer;

			printf("%d\t\t%1.2lf\t\t%1.2lf\n", 
			       length, 
			       timer/(2.0*numrounds)*1000000, 
			       (length/(timer/(2.0*numrounds)))/(1024*1024));
			fflush(stdout);
		}
	} else {
		for(length=1; length <= maxlen; length*=2) {

			// synchronize before starting PING-PONG:
			MPI_Barrier(MPI_COMM_WORLD);

			for(round=0; round<numrounds+WARM_UP; round++) {


				// recv PING:
				MPI_Recv(recv_buffer, 
				    	 length, 
					 MPI_CHAR, 
					 remote_rank, 
					 0, 
					 MPI_COMM_WORLD, 
					 &status);

				//  send PONG:
				MPI_Ssend(send_buffer, 
				    	  length, 
					  MPI_CHAR, 
					  remote_rank, 
					  0, 
					  MPI_COMM_WORLD);

			}
		}
	}

*/
	MPI_Finalize();

	return 0;
}

