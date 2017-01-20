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

#define MAX_PATH_LENGTH 50
#define WAIT_TIME 3

int psreadline_from_file(char *fname, char *lbuf) //(filename, linebufer) 
{
	char *s;
	int i;
	memset(lbuf, 0, MAX_PATH_LENGTH);
	FILE* file = fopen(fname,"r");
	if (!file) return -1;
	s = fgets(lbuf,MAX_PATH_LENGTH,file);
	if (!s) return -2;
	for (i=0; (*s)&&(i<MAX_PATH_LENGTH); i++) {
		if (*s == '\n') *s = 0;
		s++;
	}
	return 0;
}

int main(int argc, char **argv)
{
	int i, fd;
	void * map_region;
	char size_hex[MAX_PATH_LENGTH];
	char file_path[MAX_PATH_LENGTH];
	char * file;
	long size;


    	if (argc!=2){
          fprintf(stderr, "USAGE: meta_reset </dev/uioN>\n");
	  exit(-1);	
	}
	file=strdup(argv[1]);		
	
	for(i=0; i<WAIT_TIME; i++)
	{
	  printf("Reseting Memroy of device <%s> in %d seconds!\n",file,WAIT_TIME-i);
	  sleep(1);
	}	

	sprintf(file_path, "/sys/class/uio/uio%ld/maps/map1/size", strtol(file, NULL, 0));
    	psreadline_from_file(file_path, size_hex);
   	size = strtol(size_hex, NULL, 0);	
	printf("Device size %ld\n",size);	
	
	printf("Opening device file %s\n",file);	
	if ((fd=open(file, O_RDWR)) < 0){
		fprintf(stderr, "ERROR: cannot open file\n");
		exit(-1);
	}

	if ((map_region=mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 1*getpagesize()))<0){
		fprintf(stderr, "ERROR: cannot mmap file\n");
	} else {
		printf("Memory mapped to %p\n", map_region);
		memset(map_region, 0,size);  // clean device memory
		munmap(map_region,size); 	
		printf("Reset completed - all %ld bytes were set to zero!\n",size);
	}

	close(fd);
	
	return 0;

error_out:
	fprintf(stderr, "An error ouccurred unexpectedly!\n");
	return -1;
}
