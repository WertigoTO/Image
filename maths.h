#ifndef _MATHS_H_
#define _MATHS_H_

#define PIF 3.1415927410125732421875f

typedef struct _Noise
{
	bool flip;
	float z;
} Noise;

float sigmoidf(float x);

float gaussianNoisef(float mu, float sigma, Noise* noise);

#endif //_MATHS_H_