#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "simd.p5.h"

// constants:
#define NUMTRIES 3

float   aArr[ARRAYSIZE];
float   bArr[ARRAYSIZE];
float   cArr[ARRAYSIZE];

void ManualArrMult(float *aIn, float *bIn, float *cIn, int size)
{
    for(int i = 0; i < size; i++)
    {
        cIn[i] = aIn[i] * bIn[i];
    }
}



float ManualArrMultSum(float *aIn, float *bIn, int size)
{
    // Multiply them together, then take the sum
    float sum = 0.0f;
    for(int i = 0; i < size; i++)
    {
        sum += aIn[i] * bIn[i];
    }
    
}

float PickTheFunctionToUse(int i)
{
    switch (i){
        case 0 : SimdMul(aArr, bArr, cArr, ARRAYSIZE);
            return 0.0f;
            break;
        case 1 : ManualArrMult(aArr, bArr, cArr, ARRAYSIZE);
            return 0.0f;
            break;
        case 2 : return SimdMulSum(aArr, bArr, ARRAYSIZE);
            break;
        case 3 : return ManualArrMultSum(aArr, bArr, ARRAYSIZE);
            break;
    }
    
    return 0.0f;
}


int main( int argc, char *argv[ ] )
{

#ifndef _OPENMP
    fprintf( stderr, "OpenMP is not available\n" );
    return 1;
#endif
    int i, j, k;
    
    for(i = 0; i < ARRAYSIZE; i++)
    {
        aArr[i] = 1.0f;
        bArr[i] = 2.0f;
        cArr[i] = 0.0f;
    }
    
    double time0 = 0;
    double time1 = 0;
    double sumMegaArrSize[4];
    double maxMegaArrSize[4];
    float sum = 0.0f;
    
    for(i = 0; i < 4; i++)
    {
        sumMegaArrSize[i] = 0;
        maxMegaArrSize[i] = 0;
    }
    
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < NUMTRIES; j++)
        {
            
            time0 = omp_get_wtime( );
            sum = PickTheFunctionToUse(i);
            time1 = omp_get_wtime( );
            
            //printf("Sum = %8.2lf\n", sum);
            double megaIndices = (double)(ARRAYSIZE)/(time1-time0)/1000000.;
            sumMegaArrSize[i] += megaIndices;
            if( megaIndices > maxMegaArrSize[i] )
            {
                maxMegaArrSize[i] = megaIndices;
            }
        }
    }

    for(i = 0; i < 4; i++)
    {
        sumMegaArrSize[i] = sumMegaArrSize[i] / (double)NUMTRIES;
    }
    
    printf( "AS %d SL1 %8.2lf %8.2lf SL2 %8.2lf %8.2lf SL3 %8.2lf %8.2lf SL4 %8.2lf %8.2lf\n", ARRAYSIZE,
           sumMegaArrSize[0], maxMegaArrSize[0], sumMegaArrSize[1], maxMegaArrSize[1],
           sumMegaArrSize[2], maxMegaArrSize[2], sumMegaArrSize[3], maxMegaArrSize[3]);

    return 0;
}

float ReduceArray(float *cIn, int size)
{
    int mask = 0;
    for (int offset = 1; offset <= size / 2; offset *= 2)
    {
        mask = (offset * 2) - 1;
        for(int k = 0; k + offset <= size - 1; k += (mask + 1))
        {
            cIn[k] += cIn[k + offset];
        }
    }
    
    return cIn[0];
}
