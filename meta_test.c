#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <uuid/uuid.h>
#include <pthread.h>

int TestAndSet(unsigned char volatile*);

int main(int argc, char **argv)
{
	int num_ranks;
	int my_rank;
	int fd;
	void * map_region;


	MPI_Status status;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);

	printf("Rank %d opening device file /dev/uio0\n",my_rank);

	if ((fd=open("/dev/uio0", O_RDWR)) < 0){
		fprintf(stderr, "ERROR: cannot open file\n");
		exit(-1);
	}

	if ((map_region=mmap(NULL, 20000, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 1*getpagesize()))<0){
		fprintf(stderr, "ERROR: cannot mmap file\n");
		goto error_out;
	} 

	//##### Metadata Variables
	unsigned char volatile *first_byte = map_region;
	pthread_spinlock_t * volatile spinlock = map_region + sizeof(char);
	uuid_t* volatile uuid = map_region + sizeof(char) + sizeof(pthread_spinlock_t);
	char uuid_str[37];
	//#####

	/* 
	 * Memory layout
	 * =============
	 * first_byte
	 * spinlock
	 * uuid
	 * ...
	 *
	 */

	//printf("Value before __syinc_bool_compare_and_swap = %d\nand default ret value=%d\n",*p,ret);

	MPI_Barrier(MPI_COMM_WORLD);

	if (uuid_is_null(*uuid) && TestAndSet(first_byte)){
          pthread_spin_init(spinlock, PTHREAD_PROCESS_SHARED);
	  uuid_generate(*uuid);
	  uuid_unparse_lower(*uuid, uuid_str);
	  sleep(1);
	  printf("\n\n##### critical section #####\nI am the very first! My rank is %d.\nGenerated uuid: %s\n############################\n\n",my_rank,uuid_str);
	}

	//printf("Value after __sync_bool_compare_and_swap = %d\nand new ret value=%d\n",*p,ret);

	sleep(2);

	MPI_Barrier(MPI_COMM_WORLD);

	pthread_spin_lock(spinlock);
       	  (*first_byte)++;
    	  uuid_unparse_lower(*uuid, uuid_str);
    	  printf("The first byte is char=%d\tMy rank is %d.\texisting uuid: %s\n",(int)*first_byte,my_rank,uuid_str);
    	  usleep(250000); // helps to keep the output in order
	pthread_spin_unlock(spinlock);

	MPI_Barrier(MPI_COMM_WORLD);
	if(my_rank==0) printf("Programm exited normally\n");
	usleep(250000);
	MPI_Finalize();
	memset(map_region, 0, 100 );  // clean device memory
	munmap(map_region,20000); 	
        close(fd);
	
	
	return 0;

error_out:
	fprintf(stderr, "An error ouccurred unexpectedly - job aborted!\n");
	return -1;

}


int TestAndSet(unsigned char volatile* p){
/*
 * This function provides atomic test-and-set
 */

	return  __sync_bool_compare_and_swap(p,0,1);

}

