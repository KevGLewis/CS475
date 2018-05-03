#include <stdio.h>

#include <stdlib.h>

#include <math.h>

#include <omp.h>

 

// constants:
#define NUMTRIES 3

struct s
{
    float value;
    int pad[NUM];
} Array[4];


int main( int argc, char *argv[ ] )
{

#ifndef _OPENMP
    fprintf( stderr, "OpenMP is not available\n" );
    return 1;
#endif
    
    //printf( "Pad size of %d\n", NUM);
    
    omp_set_num_threads( NUMT );
    double sumMegaCalcs = 0;
    double maxMegaCalcs = 0;
    
    for(int k = 0; k < NUMTRIES; k++)
    {
    
        int someBigNumber = 1000000000;
        double time0 = omp_get_wtime( );
    
#pragma omp parallel for
        for( int i = 0; i < 4; i++ )
        {
            
             for( int j = 0; j < someBigNumber; j++ )
             {
             Array[ i ].value = Array[ i ].value + 2.;
             }
        }
        
/*#pragma omp parallel for
        for( int i = 0; i < 4; i++ )
        {
            float tmp = Array[i].value;
            for( int j = 0; j < someBigNumber; j++ )
            {
                tmp = tmp + (float)rand( );
            }
            Array[i].value = tmp;
        }*/

         double time1 = omp_get_wtime( );
        
         // print performance here:::
        double megaCalcs = (double)(someBigNumber)/(time1-time0)/1000000.;
        sumMegaCalcs += megaCalcs;
        if( megaCalcs > maxMegaCalcs )
        {
            maxMegaCalcs = megaCalcs;
        }
    }
    
    double avgMegaMult = sumMegaCalcs/(double)NUMTRIES;
    printf( "Average Performance = %8.2lf MegaMult/Sec\n", avgMegaMult );

    return 0;
}

