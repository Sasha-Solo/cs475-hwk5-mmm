#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "rtclock.h"
#include "mmm.h"

/**
* Global defs
*/
int size; //for keeping track of size
double** partialMult; //array partialMult

int main(int argc, char *argv[]) {
	double clockstart, clockend;
	int numThreads; //for keeping track of thread count
	double pfirstTimeTaken = 0.0;
	double sfirstTimeTaken = 0.0;
	double ptimeTaken = 0.0;
	double stimeTaken = 0.0;

	if ((argc == 3 && strcmp(argv[1], "P") == 0) || argc > 4 || argc < 3) { //we are using ./mmm incorrectly
		printf("Use: ./mmm S <size> or /mmm P <threads> <size>");
		return 0;
	}
	else if (argc == 4){ //if we type in 4 things, mode P (./mmm P <threads> <size>)
		//deal with possible errors with use of ./mmm:
		sscanf(argv[2], "%d", &numThreads);
		if (numThreads <= 0){
			printf("\nNumber of threads should be positive.\n");
			return 0;
		}
		sscanf(argv[3], "%d", &size);
		if (numThreads > size || numThreads == size){
			printf("\nNumber of threads must be less than size %d.\n", size);
			return 0;
		}
		if (size < 0){
			printf("\nSize should be positive.\n");
			return 0;
		}
		printf("========\nmode: parallel\n");
		printf("thread count: %d\n", numThreads);
		printf("size: %d\n========", size);

		for (int i = 0; i < 4; i++){ //run par version 4 times
			mmm_init(); //allocate space and initialize matrices
			clockstart = rtclock(); // start clocking

			//malloc an for partialMult where the partial results will be stored
			partialMult = (double**) malloc(sizeof(double*) * size);
			for (int i = 0; i < size; i++){
				partialMult[i] = (double*) malloc(sizeof(double) * size);
			}

			//malloc space for struct for input args
			threadArgs *args = (threadArgs*) malloc(numThreads * sizeof(threadArgs));

			//initialize input arguments from struct
			//printf("numT = %d \n", numThreads);
			for (int i = 0; i < numThreads; i++){
				args[i].tid = i;
				args[i].start = (i * size / numThreads + 1) - 1;
			//args[i].start = start - 1;
				args[i].end = (i + 1) * size / numThreads;
			//args[i].end = end;
			//printf("thread #: %d, begin: %ld, end: %ld\n", args[i].tid, args[i].start, args[i].end);
			}
		
			// allocate space to hold threads
  			pthread_t *threads = (pthread_t*) malloc(numThreads * sizeof(pthread_t));

			for (int i = 0; i < numThreads; i++) { //spawn off threads
    			pthread_create(&threads[i], NULL, mmm_par, &args[i]);
  			}

			for (int i = 0; i < numThreads; i++){ //join threads
				pthread_join(threads[i], NULL);
			}

			for (int i = 0; i < size; i++) { //put partialMult into output
				for (int j = 0; j < size; j++){
					outputMatrix[i][j] = partialMult[i][j];
				}	
			}

			clockend = rtclock(); // stop clocking

			if (i == 0){
				pfirstTimeTaken = (clockend - clockstart);
			}
			ptimeTaken = ptimeTaken + (clockend-clockstart);

			mmm_reset(matrix1);
			mmm_reset(matrix2);
		}

		ptimeTaken = (ptimeTaken - pfirstTimeTaken)/3.0;
		printf("\nParallel Time (avg of 3 runs): %.6f sec\n", ptimeTaken);

		//printf("\nTime taken: %.6f sec\n", (clockend - clockstart));
			mmm_freeup(); //free everything
			for (int i = 0; i < size; i++) { //free each row in matrix2
    			free(partialMult[i]);
    			partialMult[i] = NULL;  //remove dangling pointer
			}
			//free everything else
			free(partialMult);
			partialMult = NULL;
			free(args);
			args = NULL;

		for (int i = 0; i < 4; i++){ //run seq version 4 times
			mmm_init(); //allocate space and initialize matrices
			clockstart = rtclock(); // start clocking
			mmm_seq();
			clockend = rtclock(); // stop clocking
			//printf("\ns time taken: %.6f sec\n", (clockend - clockstart));
			mmm_freeup(); //free everything
			if (i == 0){
				sfirstTimeTaken = (clockend - clockstart);
			}
			stimeTaken = stimeTaken + (clockend-clockstart);
		}

		stimeTaken = (stimeTaken - sfirstTimeTaken)/ 3.0;
		//printf("firstTime : %.6f", firstTimeTaken);
		printf("Sequential Time (avg of 3 runs): %.6f sec\n", stimeTaken);
		printf("Speedup: %.6f\n", (ptimeTaken/stimeTaken));
		//mmm_verify();

	}
	else if (argc == 3){ //3 things is mode S, ./mmm S <size>
		printf("========\nmode: sequential\n");
		sscanf(argv[2], "%d", &size);
		if (size < 0){
			printf("\nSize should be positive.");
			//return 0;
		}
		printf("thread count: 1\n");
		printf("size: %d\n========", size);

		for (int i = 0; i < 4; i++){
			mmm_init(); //allocate space and initialize matrices
			clockstart = rtclock(); // start clocking
			mmm_seq();
			clockend = rtclock(); // stop clocking
			//printf("\nTime taken: %.6f sec\n", (clockend - clockstart));
			mmm_freeup(); //free everything

			if (i == 0){
				sfirstTimeTaken = (clockend - clockstart);
			}
			stimeTaken = stimeTaken + (clockend-clockstart);
		}

		stimeTaken = (stimeTaken - sfirstTimeTaken)/ 3.0;
		//printf("firstTime : %.6f", firstTimeTaken);
		printf("\nSequential Time (avg of 3 runs): %.6f sec\n", stimeTaken);
		//printf("Speedup: %.6f\n", (ptimeTaken/stimeTaken));
		//mmm_verify();

	

	}

	return 0;
}
