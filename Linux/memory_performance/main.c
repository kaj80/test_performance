#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define BUFFER_SIZE 5*1024*1024
static int block_size =  1024;
static int tries  =  10;

typedef void(copy_buffer)(const char*, size_t);

char* prepare_test_data(size_t size){
	char* tmp = (char*)malloc(size*sizeof(char));
	size_t i = 0;

	for (i = 0; i < size; i++) {
		/* code */
		tmp[i]=i;
	}
	return tmp;
}

void destroy_test_data(void* data){
	free(data);
}

void copy_data_method_1(const char* buffer, size_t size){
	char* tmp = NULL;
	size_t i = 0;
	size_t block = 0;

	for (i = 0; i < size; i+=block_size) {
			tmp = realloc(tmp,(block+1)*block_size*sizeof(char));
			block++;
			memcpy(tmp+i,buffer+i,block_size); 
	}
	free(tmp);
}

void copy_data_method_2(const char* buffer, size_t size){
	char* tmp = (char*)malloc(size*sizeof(char));
	size_t i = 0;
	size_t block = 0;

	for (i = 0; i < size; i+=block_size) {
			block++;
			memcpy(tmp+i,buffer+i,block_size); 
	}
	free(tmp);
}

void copy_data_method_3(const char* buffer, size_t size){
	char* tmp = (char*)malloc(size*sizeof(char));
	memcpy(tmp,buffer,size); 
	
	free(tmp);
}

void copy_data_method_4(const char* buffer, size_t size){
	static char* tmp = NULL;
	if(tmp==NULL)
		tmp=(char*)malloc(size*sizeof(char));
	memcpy(tmp,buffer,size); 
}

void run_test(const char* buffer, size_t size,copy_buffer method,const char* test_name){
	clock_t start, end;
	double cpu_time_used;
	int i=0;

	start = clock();
	/* Do the work. */
	for (i = 0; i < tries; i++) {
		/* code */
		method(buffer,size);
	}
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("%s: cpu time: %.5f sec.\n",test_name,cpu_time_used/tries);
}

void run_all_tests(size_t size){
	char* buffer = prepare_test_data(size);
	run_test(buffer,size,copy_data_method_1,"realloc;copy block;destroy");
	run_test(buffer,size,copy_data_method_2,"alloc;copy block;destroy");
	run_test(buffer,size,copy_data_method_3,"alloc;copy all;destroy");
	run_test(buffer,size,copy_data_method_4,"reuse;copy all");
	destroy_test_data(buffer);
}

int main(int argc, char *argv[])
{
	int opt;
	int buffer_size = 1024;

	while ((opt = getopt(argc, argv, "s:b:")) != -1) {
		switch (opt) {
		case 'b':
			block_size = atoi(optarg);
		break;
		case 's':
			buffer_size = atoi(optarg);
		break;
		default: /* '?' */
			fprintf(stderr, "Usage: %s [-s buffer size in MB] [-b block size in bytes]\n",argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	printf("Buffer size: %d MB; Block size: %d bytes\n", buffer_size, block_size);

	/*
	if (optind >= argc) {
		fprintf(stderr, "Expected argument after options\n");
		exit(EXIT_FAILURE);
	}
	*/


	run_all_tests(buffer_size*1024*1024);

	/* Other code omitted */

	exit(EXIT_SUCCESS);
}
