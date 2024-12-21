#include "egmath.h"

void v3f32_xyz(float v[3], float x, float y, float z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

void v3f32_mul(float r[3], float const a[3], float b)
{
	r[0] = a[0] * b;
	r[1] = a[1] * b;
	r[2] = a[2] * b;
}

void v3f32_add(float r[3], float const a[3], float const b[3])
{
	r[0] = a[0] + b[0];
	r[1] = a[1] + b[1];
	r[2] = a[2] + b[2];
}

void v3f32_cross(int const a[3], int const b[], int p[])
{
	p[0] = a[1] * b[2] - a[2] * b[1];
	p[1] = a[2] * b[0] - a[0] * b[2];
	p[2] = a[0] * b[1] - a[1] * b[0];
}



float v3f32_distance(float const a[3], float const b[3])
{
	float d[3];
	d[0] = a[0] - b[0];
	d[1] = a[1] - b[1];
	d[2] = a[2] - b[2];
	return sqrtf(V3_DOT(d, d));
}

float v3f32_distance2(float const a[3], float const b[3])
{
	float d[3];
	d[0] = a[0] - b[0];
	d[1] = a[1] - b[1];
	d[2] = a[2] - b[2];
	return V3_DOT(d, d);
}

void v3f32_proj(float const bnorm[3], float const a[3], float r[3])
{
	float k = V3_DOT(a, bnorm);
	r[0] = bnorm[0] * k;
	r[1] = bnorm[1] * k;
	r[2] = bnorm[2] * k;
}

void v3f32_oproj(float const bnorm[3], float const a[3], float r[3])
{
	float k = V3_DOT(a, bnorm);
	r[0] = a[0] - bnorm[0] * k;
	r[1] = a[1] - bnorm[1] * k;
	r[2] = a[2] - bnorm[2] * k;
}


/**
 * @brief Calculates the shortest distance between L1 and L2
 * https://math.stackexchange.com/questions/3081301/shortest-distance-between-two-vectors
 *
 * @param p1 Point on L1
 * @param v1 Direction of L1
 * @param p2 Point on L2
 * @param v2 Direction of L2
 * @return Returns the shortest distance between L1 and L2
 */
float v3f32_l1l2_distance(float const p1[3], float const v1[3], float const p2[3], float const v2[3])
{
	float d[3];
	d[0] = p1[0] - p2[0];
	d[1] = p1[1] - p2[1];
	d[2] = p1[2] - p2[2];
	float n[3];
	n[0] = V3_CROSS0(v1, v2);
	n[1] = V3_CROSS1(v1, v2);
	n[2] = V3_CROSS2(v1, v2);
	v3f32_normalize(n, n, 0.0001);
	float l = fabs(V3_DOT(d, n));
	return l;
}



// http://www.illusioncatalyst.com/notes_files/mathematics/line_plane_intersection.php
float v3f32_plane_point_line_distance(float const v[3], float const l0[3], float const c[3], float const n[3])
{
	float w[3];
	w[0] = l0[0] - c[0];
	w[1] = l0[1] - c[1];
	w[2] = l0[2] - c[2];
	float vn = V3_DOT(v, n);
	if (vn == 0.0) {
		// The line is parallel to the plane.
		return 0;
	}

	float t = -V3_DOT(w, n) / V3_DOT(v, n);
	// There is one intersection at:
	float i[3];
	i[0] = l0[0] + t * v[0];
	i[1] = l0[1] + t * v[1];
	i[2] = l0[2] + t * v[2];
	return v3f32_distance(c, i);
}

















void v3f32_print(float const x[3])
{
	printf("%f %f %f\n", x[0], x[1], x[2]);
}

