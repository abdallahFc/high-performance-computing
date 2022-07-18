#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define expected 1.202056903159594
int power(int x, int y)
{
    if (y == 0)
        return 1;
    else if (y%2 == 0)
        return power(x, y/2)*power(x, y/2);
    else
        return x*power(x, y/2)*power(x, y/2);

}
double F ( int a){
	return ( double )1.0/power(a,3);
}
int main ( int argc , char * argv []){
	int n , id , numprocs , i;
	double  result , sum ;
	MPI_Init(& argc ,& argv );
	MPI_Comm_size( MPI_COMM_WORLD ,& numprocs );
	MPI_Comm_rank( MPI_COMM_WORLD ,& id );
	if ( id == 0) {
		printf (" Enter the number = ");
		scanf ("%d" ,&n );
	}
	MPI_Bcast(&n, 1, MPI_INT , 0 , MPI_COMM_WORLD );
	if(n>0)
	{
			sum = 0.0;
			for ( i = id + 1; i <= n ; i += numprocs) {
				sum += F(i);
				printf("Process %d with num %d \n", id,i);
			}
			MPI_Reduce(&sum , &result , 1 , MPI_DOUBLE , MPI_SUM , 0 , MPI_COMM_WORLD );
			if ( id == 0) {
			printf ("result is : %.16f Error is: %.16f\n", result ,expected-result );
		}
		}
	MPI_Finalize();
	return 0;
}
