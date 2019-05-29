//Compilar con: gcc  −Wall −fopenmp −o oven Oven-sort.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>

typedef long long int LLi;

void Hello(void) {
	int my_rank = omp_get_thread_num();
	int thread_count = omp_get_num_threads();

	printf("Hello from thread %d of %d\n", my_rank, thread_count);
 } 

/*
void mostrar_Array(double a[],int n)
{
	for(int i=0 ; i < n;i++ )
		cout<<a[i]<<" ";
}*/
 
void odd_even_sort(double a[],int n)
{
	int phase,i,temp;
	for(phase = 0; phase < n; phase++)
	{
		if((phase % 2)==0)
		{
			for(i = 1; i < n; i+=2)
			{
				if(a[i-1]>a[i])
				{
					temp= a[i];
					a[i] = a[i-1];
					a[i-1] = temp;	
				}
			}
		}
		else {
			for(i = 1; i < n-1; i+=2)
			{
				if(a[i] > a[i+1])
				{
					temp= a[i];
					a[i] = a[i+1];
					a[i+1] = temp;
				}
			}
		}
	}

}
void odd_even_sort_OpenMp1(double *a,int n,int thread_count)
{
	int phase,i,temp;
	double start, finish, tiempoF;
	start = omp_get_wtime();
	#ifdef DEBUG
	char title[100];
	#endif
	
	for(phase = 0; phase < n; phase++)
	{
		if((phase % 2)==0)
		{
			#pragma omp parallel for num_threads(thread_count) default(none) shared(a,n) private(i,temp)
			for(i = 1; i < n; i+=2)
			{
				if(a[i-1]>a[i])
				{
					temp= a[i];
					a[i] = a[i-1];
					a[i-1] = temp;	
				}
			}
		}
		else {
			#pragma omp parallel for num_threads(thread_count) default(none) shared(a,n) private(i,temp)
			for(i = 1; i < n-1; i+=2)
			{
				if(a[i] > a[i+1])
				{
					temp= a[i];
					a[i] = a[i+1];
					a[i+1] = temp;
				}
			}
		}
		#ifdef DEBUG
		sprintf(title,"After phase %d",phase);
		Print_list(a,n,title);
		#endif

	}
	finish = omp_get_wtime();
	tiempoF = finish - start;
	printf("Tiempo de ejecucion es: %e\n",tiempoF);

}

void odd_even_sort_OpenMp2(double *a,LLi n,int thread_count)
{
	LLi phase,i,temp;
	double start, finish, tiempoF;
	start = omp_get_wtime();
	#pragma omp parallel for num_threads(thread_count) default(none) shared(a,n) private(i,temp,phase)
	for(phase = 0; phase < n; phase++)
	{
		if((phase % 2)==0)
		{
			#pragma omp parallel for

			for(i = 1; i < n; i+=2)
			{

				if(a[i-1]>a[i])
				{
					temp= a[i];
					a[i] = a[i-1];
					a[i-1] = temp;	
				}
			}
		}
		else {
			#pragma omp parallel for 
			for(i = 1; i < n-1; i+=2)
			{
				if(a[i] > a[i+1])
				{
					temp= a[i];
					a[i] = a[i+1];
					a[i+1] = temp;
				}
			}
		}
		
	}
	finish = omp_get_wtime();
	tiempoF = finish - start;
	printf("Tiempo de ejecucion es: %e\n",tiempoF);

}

void Omp_matriz_vector(double *A,double *x,double  *y,LLi m,LLi n,int thread_count)
{
	LLi i,j;
	double start, finish, tiempoF;
	start = omp_get_wtime();
	#pragma omp parallel for num_threads(thread_count) default(none) private(i,j) shared(A,x,y,m,n)
	for(i=0;i<m;i++)
	{
		y[i] = 0.0;
		for(j=0; j < n; j++)
		{
			y[i] += A[i*n+j]*x[j];
		}
	}
	finish = omp_get_wtime();
	tiempoF = finish - start;
	printf("Tiempo de ejecucion es: %e\n",tiempoF);
}

void Llenarmatriz(double * matriz, int fila, int col)
{
    LLi i;
    for (i=0;i<(fila*col);i++) 
    {
        *(matriz + i)=1+(double)(10*rand()/(RAND_MAX+1.0));
    }
    return;
}
int main(int argc, char const *argv[])
{
	int thread_count = strtol(argv[1],NULL,16);
	LLi odev=20000;
	double *a=malloc(odev*sizeof(double));
	Llenarmatriz(a,1,odev);
	odd_even_sort_OpenMp1(a,odev,thread_count);
	
/*
	LLi m=8,n=8000000;
	double *x=malloc(n*sizeof(double));
	Llenarmatriz(x,1,n);
	
	double *y=malloc(m*sizeof(double));

	double *A=malloc(m*n*sizeof(double));
    Llenarmatriz(A,m,n);
	
	Omp_matriz_vector(A,x,y,m,n,thread_count);
	*/
	return 0;
}