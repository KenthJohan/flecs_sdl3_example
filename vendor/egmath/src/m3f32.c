#include "egmath.h"

void m3f32_mul(m3f32 *y, m3f32 const *a, m3f32 const *b)
{
	m3f32 t;
	// Column vector 0:
	t.c0[0] = V3_DOTE(b->c0, M3_R0(*a));
	t.c1[0] = V3_DOTE(b->c1, M3_R0(*a));
	t.c2[0] = V3_DOTE(b->c2, M3_R0(*a));
	// Column vector 1:
	t.c0[1] = V3_DOTE(b->c0, M3_R1(*a));
	t.c1[1] = V3_DOTE(b->c1, M3_R1(*a));
	t.c2[1] = V3_DOTE(b->c2, M3_R1(*a));
	// Column vector 2:
	t.c0[2] = V3_DOTE(b->c0, M3_R2(*a));
	t.c1[2] = V3_DOTE(b->c1, M3_R2(*a));
	t.c2[2] = V3_DOTE(b->c2, M3_R2(*a));
	*y = t;
}

void m3f32_mulv(m3f32 const *a, float const x[3], float y[3])
{
	float temp[3];
	temp[0] = V3_DOTE(x, M3_R0(*a));
	temp[1] = V3_DOTE(x, M3_R1(*a));
	temp[2] = V3_DOTE(x, M3_R2(*a));
	y[0] = temp[0];
	y[1] = temp[1];
	y[2] = temp[2];
}

void m3f32_tmulv(m3f32 const *a, float const x[3], float y[3])
{
	float temp[3];
	temp[0] = V3_DOT(x, a->c0);
	temp[1] = V3_DOT(x, a->c1);
	temp[2] = V3_DOT(x, a->c2);
	y[0] = temp[0];
	y[1] = temp[1];
	y[2] = temp[2];
}

void m3f32_transpose(m3f32 *x)
{
	SWAP(float, x->c0[1], x->c1[0]);
	SWAP(float, x->c0[2], x->c2[0]);
	SWAP(float, x->c1[2], x->c2[1]);
}



/*
http://www.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche0053.html
http://graphics.cs.cmu.edu/nsp/course/15-462/Spring04/slides/04-transform.pdf
https://math.stackexchange.com/questions/1234948/inverse-of-a-rigid-transformation
https://www.cuemath.com/algebra/inverse-of-diagonal-matrix/
*/
void m3f32_rs_inverse(float const q[4], float const s[3], m3f32 *r)
{
	float x = q[0];
	float y = q[1];
	float z = q[2];
	float w = q[3];

	float sx = 1.0 / s[0];
	float sy = 1.0 / s[1];
	float sz = 1.0 / s[2];

	float xx = x * x;
	float yy = y * y;
	float zz = z * z;
	//float ww = w * w;

	float xy = x * y;
	float xz = x * z;
	float zw = z * w;
	float yw = y * w;
	float yz = y * z;
	float xw = x * w;

	// Store as transposed rotation matrix
	r->c0[0] = (1.0f - 2.0f * (yy + zz)) * sx;
	r->c1[0] = (xy + zw) * sx * 2.0f;
	r->c2[0] = (xz - yw) * sx * 2.0f;
	
	r->c0[1] = (xy - zw) * sy * 2.0f;
	r->c1[1] = (1.0f - 2.0f * (xx + zz)) * sy;
	r->c2[1] = (yz + xw) * sy * 2.0f;

	r->c0[2] = (xz + yw) * sz * 2.0f;
	r->c1[2] = (yz - xw) * sz * 2.0f;
	r->c2[2] = (1.0f - 2.0f * (xx + yy)) * sz;
}

void m3f32_rs_inverse_transposed(float const q[4], float const s[3], m3f32 *r)
{
	float x = q[0];
	float y = q[1];
	float z = q[2];
	float w = q[3];

	float sx = 1.0 / s[0];
	float sy = 1.0 / s[1];
	float sz = 1.0 / s[2];

	float xx = x * x;
	float yy = y * y;
	float zz = z * z;
	//float ww = w * w;

	float xy = x * y;
	float xz = x * z;
	float zw = z * w;
	float yw = y * w;
	float yz = y * z;
	float xw = x * w;

	r->c0[0] = (1.0f - 2.0f * (yy + zz)) * sx;
	r->c0[1] = (xy + zw) * sx * 2.0f;
	r->c0[2] = (xz - yw) * sx * 2.0f;

	r->c1[0] = (xy - zw) * sy * 2.0f;
	r->c1[1] = (1.0f - 2.0f * (xx + zz)) * sy;
	r->c1[2] = (yz + xw) * sy * 2.0f;

	r->c2[0] = (xz + yw) * sz * 2.0f;
	r->c2[1] = (yz - xw) * sz * 2.0f;
	r->c2[2] = (1.0f - 2.0f * (xx + yy)) * sz;
}



