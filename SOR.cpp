#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#define MAXTHREADS 64/* max. # threads */
void * thread_main(void *);
void InitializeData();
void barrier();
pthread_mutex_t update_lock;
pthread_mutex_t barrier_lock;/* mutex for the barrier */
pthread_cond_t all_here;/* condition variable for barrier */
int count=0;/* counter for barrier */
int n, t, rowsPerThread;
double myDelta, threshold;
double **val, **new;
double delta=0.0;
/* Command line args: matrix size, number of threads, threshold
*/
int main(int argc, char * argv[])
{
	 /* thread ids and attributes */
	 pthread_t tid[MAXTHREADS];
	 pthread_attr_t attr;
	 long i, j;
	 long startTime, endTime,seqTime,parTime;
	 /* set global thread attributes */
	 pthread_attr_init(&attr);
	 pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	 /* initial mutex and condition variable */
	 pthread_mutex_init(&update_lock, NULL);
	 pthread_mutex_init(&barrier_lock, NULL);
	 pthread_cond_init(&all_here, NULL);
	 /* read command line arguments */
	 if (argc != 4) {
		 printf("usage: %s <matrix size> <# threads> <threshold>\n",
		 argv[0]);
		 exit(1);
	 } // end if

	 sscanf(argv[1], "%d", &n);
	 sscanf(argv[2], "%d", &t);
	 sscanf(argv[3], "%f", &threshold);
	 rowsPerThread = n/t;
	 InitializeData();
	 for (i=0; i<t; i++) {
	 	pthread_create(&tid[i], &attr, thread_main, (void *) i);
	 } // end for

	 for (i=0; i < t; i++) {
	 	pthread_join(tid[i], NULL);
	 } // end for
	 printf("maximum difference: %e\n", delta);
} // end main
void* thread_main(void * arg) {
	long id=(long) arg;
	double average;
	double **myVal, **myNew;
	double **temp;
	int i, j;
	int start;
	/* determine first row that this thread owns */
	start = id*rowsPerThread+1;
	myVal = val;
	myNew = new;
	do {
		myDelta = 0.0;
		if (id == 0) {
			delta=0.0;/* reset shared value of delta */
		} // end if
		barrier();
		/* update each point */
		for (i = start; i < start+rowsPerThread; i++) {
			for (j = 1; j < n+1; j++) {
				average = (myVal[i-1][j] + myVal[i][j+1] + myVal[i+1][j] + myVal[i][j-1])/4;
				if (myDelta < fabs(average - myVal[i][j]))
					myDelta=fabs(average-myVal[i][j]);
				myNew[i][j] = average;
			} // end for j
		} // end for i
		temp = myNew; /* prepare for next iteration */
		myNew = myVal;
		myVal = temp;
		pthread_mutex_lock(&update_lock);
		if (myDelta > delta) {
			delta = myDelta; /* update delta */
		} // end if
		pthread_mutex_unlock(&update_lock);
		barrier();
	} while (delta >threshold); // end do
} // end thread_main
void InitializeData() {
	int i, j;
	new = (double **) malloc((n+2)*sizeof(double *));
	val = (double **) malloc((n+2)*sizeof(double *));
	for (i = 0; i < n+2; i++) {
		new[i] = (double *) malloc((n+2)*sizeof(double));
		val[i] = (double *) malloc((n+2)*sizeof(double));
	} // end for i
	/* initialize to 0.0 except to 1.0 along the left boundary */
	for (i = 0; i < n+2; i++) {
		val[i][0] = 1.0;
		new[i][0] = 1.0;
	} // end for i
	for (i = 0; i < n+2; i++) {
		for (j = 1; j < n+2; j++) {
		val[i][j] = 0.0;
		new[i][j] = 0.0;
		} // end for j
	} // end for i
} // end InitializeData
void barrier() {
	pthread_mutex_lock(&barrier_lock);
	count++;
	if (count == t) {
		count = 0;
		pthread_cond_broadcast(&all_here);
	} else {
		while(pthread_cond_wait(&all_here, &barrier_lock) != 0);
	} // end if
	pthread_mutex_unlock(&barrier_lock);
} // end barrier