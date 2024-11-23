#include "egmath.h"

void v4f32_mul(float r[4], float const a[4], float b)
{
	r[0] = a[0] * b;
	r[1] = a[1] * b;
	r[2] = a[2] * b;
	r[3] = a[3] * b;
}

void v4f32_add(float r[4], float const a[4], float const b[4])
{
	r[0] = a[0] + b[0];
	r[1] = a[1] + b[1];
	r[2] = a[2] + b[2];
	r[3] = a[3] + b[3];
}


void v4f32_print(float const x[4])
{
	printf("%f %f %f %f\n", x[0], x[1], x[2], x[3]);
}




