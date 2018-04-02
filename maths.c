#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#include "maths.h"


float sigmoidf(float x)
{
	return 1.f / (1.f + exp(-x));
}

float gaussianNoisef(float mu, float sigma, Noise* noise)
{
	static const float piMul2 = 2 * PIF;	

	float a, b;
	float d, e;
	if(noise->flip)
	{
		a = 1.f / (float)RAND_MAX * (float)rand();
		b = 1.f / (float)RAND_MAX * (float)rand();

		d = sqrt(-2.f * logf(a)) * cos(piMul2 * b);
		e = sqrt(-2.f * logf(a)) * sin(piMul2 * b);

		noise->flip = false;
		noise->z = e;
		return d;
	}
	
	noise->flip = true;
	return noise->z;
}