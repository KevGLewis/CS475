#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

int		NowYear;		// 2017 - 2022
int		NowMonth;		// 0 - 11
unsigned int Seed;

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int		NowNumDeer;		// number of deer in the current population
int		NowClown;
int		MonthCount;

const float GRAIN_GROWS_PER_MONTH =		8.0;
const float ONE_DEER_EATS_PER_MONTH =	0.5;
const float ONE_CLOWN_KILLS_PER_MONTH = 0.25;

const float AVG_PRECIP_PER_MONTH =		6.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =				2.0;	// plus or minus noise

const float AVG_TEMP =					50.0;	// average
const float AMP_TEMP =					20.0;	// plus or minus
const float RANDOM_TEMP =				10.0;	// plus or minus noise

const float MIDTEMP =					40.0;
const float MIDPRECIP =					10.0;


float SQR( float x )
{
        return x*x;
}

float Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}


int Ranf( unsigned int *seedp, int ilow, int ihigh )
{
        float low = (float)ilow;
        float high = (float)ihigh + 0.9999f;

        return (int)(  Ranf(seedp, low,high) );
}


void GrainDeer( )
{
	while( NowYear < 2023 )
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		int tempDeer = 0;

		if(NowNumDeer > NowHeight)
		{
			tempDeer = NowNumDeer - 1;
		}
		else
		{
			tempDeer = NowNumDeer + 1;
		}

		if(NowClown >= 5)
		{
			tempDeer = 0;

		}

		if(tempDeer < 0)
			tempDeer = 0;

		// DoneComputing barrier:
		#pragma omp barrier
		
		NowNumDeer = tempDeer;

		// DoneAssigning barrier:
		#pragma omp barrier


		// DonePrinting barrier:
		#pragma omp barrier
	}
}

void Grain( )
{
	while( NowYear < 2023 )
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		float tmpNowHeight = NowHeight;

		float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
		float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );

		tmpNowHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
 		tmpNowHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
 		if(tmpNowHeight < 0)
 		{
 			tmpNowHeight = 0;
 		}

		// DoneComputing barrier:
		#pragma omp barrier
		
		NowHeight = tmpNowHeight;

		// DoneAssigning barrier:
		#pragma omp barrier

		// DonePrinting barrier:
		#pragma omp barrier
	}
}

void Watcher( )
{
	while( NowYear < 2023 )
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
		float temp = AVG_TEMP - AMP_TEMP * cos( ang );
		float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );

		// DoneComputing barrier:
		#pragma omp barrier
		
		MonthCount++;
		NowTemp = temp + Ranf( &Seed, -RANDOM_TEMP, RANDOM_TEMP );
		NowPrecip = precip + Ranf( &Seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
		if( NowPrecip < 0. )
			NowPrecip = 0.;

		// DoneAssigning barrier:
		#pragma omp barrier

		printf("Year, %d, Month, %d, OMonth, %d, Dr, %d, Clwn, %d, Grn, %4.2lf, T, %4.2lf, Precip, %4.2lf\n", 
				NowYear, NowMonth, MonthCount, NowNumDeer, NowClown, NowHeight, NowTemp, NowPrecip);

		NowMonth++;
		if(NowMonth >= 12)
		{
			NowMonth = 0;
			NowYear++;
		}

		// DonePrinting barrier:
		#pragma omp barrier
	}
}

void Clown( )
{
	while( NowYear < 2023 )
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		int tmpClown = NowClown;
		if(NowNumDeer > NowClown)
		{
			tmpClown++;
		}
		else
		{
			tmpClown = tmpClown - 2;
		}

		if(tmpClown < 0)
		{
			tmpClown = 0;
		}

		// DoneComputing barrier:
		#pragma omp barrier

		NowClown = tmpClown;
		// DoneAssigning barrier:
		#pragma omp barrier


		// DonePrinting barrier:
		#pragma omp barrier
	}
}

int main( int argc, char *argv[ ] )
{

#ifndef _OPENMP
    fprintf( stderr, "OpenMP is not available\n" );
    return 1;
#endif
    

    Seed = time(NULL);

	// starting date and time:
	NowMonth =    0;
	NowYear  = 2017;

	// starting state (feel free to change this if you want):
	NowNumDeer = 1;
	NowClown = 0;
	NowHeight =  1.;
	NowTemp = 30;
	    
	omp_set_num_threads( 4 );	// same as # of sections
	#pragma omp parallel sections
	{
		#pragma omp section
		{
			GrainDeer( );
		}

		#pragma omp section
		{
			Grain( );
		}

		#pragma omp section
		{
			Watcher( );
		}

		#pragma omp section
		{
			Clown( );	// your own
		}
	}       // implied barrier -- all functions must return in order
		// to allow any of them to get past here

    return 0;
}

