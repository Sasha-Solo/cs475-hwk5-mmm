#ifndef MMM_H_
#define MMM_H_

// globals (anything here would be shared with all threads) */
// I would declare the pointers to the matrices here (i.e., extern double **A, **B, **C),
// as well as the size of the matrices, etc.

extern int size;
extern double** matrix1; //for the first matrix
extern double** matrix2; //for the second matrix
extern double** outputMatrix; //for the output matrix after multiplication
extern double** partialMult;
extern double** tempOutput;

typedef struct threadArgs { //struct for input args that go to mmm_par() 
  int tid;    // the thread id 
  long start; // where to start from
  long end;   // where to end at
} threadArgs;


void mmm_init();
void mmm_reset(double **);
void mmm_freeup();
void mmm_seq();
void *mmm_par(void *args);
double mmm_verify();

#endif /* MMM_H_ */
