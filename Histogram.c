#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#include <omp.h>
int find_index(int temp, int* points, int intervals)
{
	
	int index;
	for( index = 0; index < intervals-1; index ++)
	{
		if( temp <= points[index+1]){ break;}
	}
	return index;
}
int main(int argc , char * argv[])
{
	int my_rank,p,source,dest,i,c,
	max=0,min=1000,d,threads_num,j,
	len,temp,index,elements_per_process,
	n_elements_recieved,n,bars;
	FILE *fp;
	fp=fopen("/shared/data.txt","r");
	int* a;
    int count [100];
	int cc[100];
	int ccc[100];
	int a2[100];
	int range[100];
	MPI_Status status;
	MPI_Init( &argc , &argv );
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	if(my_rank ==0)
	{
	printf("Enter a number of data: ");
    scanf("%d", & n);
	printf("Enter a number of bars: ");
    scanf("%d", & bars);
	a=malloc (n * sizeof(int));
    for ( i = 0; i < n; i++)
       fscanf(fp , " %d", &a[i]);
	for ( i = 0; i < n; i++){	
		if(a[i]>max)
			max=a[i];
		}
		len= max/bars;	
	temp=0;
	for(i = 0; i <=bars; i++)
	{
		range[i] = temp;
		temp += len;
	}
	elements_per_process=n/p;
	int in;
			for (i = 1; i < p - 1; i++)
			{
                in = i * elements_per_process;
                MPI_Send(&elements_per_process,1, MPI_INT, i, 0,MPI_COMM_WORLD);
                MPI_Send(&a[in],elements_per_process,MPI_INT, i, 0,MPI_COMM_WORLD);
				MPI_Send(&range,100,MPI_INT, i, 0,MPI_COMM_WORLD);
				MPI_Send(&bars,1,MPI_INT, i, 0,MPI_COMM_WORLD);
            }
			in = i* elements_per_process;
            int elements_left = n - in;

            MPI_Send(&elements_left,1, MPI_INT,i, 0,MPI_COMM_WORLD);
            MPI_Send(&a[in],elements_left,MPI_INT, i, 0,MPI_COMM_WORLD);
			MPI_Send(&range,100,MPI_INT, i, 0,MPI_COMM_WORLD);
			MPI_Send(&bars,1,MPI_INT, i, 0,MPI_COMM_WORLD);
	#pragma omp parallel private(i)
    {
    threads_num=omp_get_num_threads();
    #pragma omp for schedule(static)
	for(  i = 0; i< elements_per_process; i++)
	{
		 int index = find_index (a[i], range, bars);
		 count[i]=index;		 
	}
	}
	for ( i = 0; i <bars ; i++)
	{
		cc[i]=0;
	}
	for(  i = 0; i< bars; i++)
	{
		for(  j = 0; j<elements_per_process; j++){
		 if(count[j]==i){
			 cc[i]++;
			  
		 }
		 
		}
	}
	    for(i=1;i<p;i++)
        {
			MPI_Recv(&ccc,100, MPI_INT, i, 0,MPI_COMM_WORLD,&status);
			for ( j = 0; j<bars ; j++)
	        {
				cc[j]+=ccc[j];
		
	        }
        }
	#pragma omp parallel private(i)
    {
	threads_num=omp_get_num_threads();
	#pragma omp for schedule(static)
	for ( i = 0; i <bars ; i++)
	{
		printf("The range start with %d end with %d with count %d \n ", range[i], range[i+1],cc[i]);
	}
	}

	}
	else{
		MPI_Recv(&n_elements_recieved,1, MPI_INT, 0, 0,MPI_COMM_WORLD,&status);
        MPI_Recv(&a2, n_elements_recieved,MPI_INT, 0, 0,MPI_COMM_WORLD,&status);
		MPI_Recv(&range, 100,MPI_INT, 0, 0,MPI_COMM_WORLD,&status);
		MPI_Recv(&bars,1, MPI_INT, 0, 0,MPI_COMM_WORLD,&status);
	#pragma omp parallel private(i)
    {
    threads_num=omp_get_num_threads();
    #pragma omp for schedule(static)
	for(  i = 0; i< n_elements_recieved; i++)
	{
		 int index = find_index (a2[i], range, bars);
		 count[i]=index;
	}
	}
	for ( i = 0; i <bars ; i++)
	{
		cc[i]=0;
	}
	for(  i = 0; i< bars; i++)
	{
		for(  j = 0; j<n_elements_recieved; j++){
		 if(count[j]==i){
			 cc[i]++;
		 }
		}
	}
	 MPI_Send(&cc,100, MPI_INT,0, 0,MPI_COMM_WORLD);
	}
  MPI_Finalize();
	return 0;
}
