#include "egmath.h"

void qf32_xyza(float q[4], float x, float y, float z, float a)
{
	float const c = cosf(a * 0.5f);
	float const s = sinf(a * 0.5f);
	assert((c != 0) || (s != 0));
	q[0] = s * x;
	q[1] = s * y;
	q[2] = s * z;
	q[3] = c;
}

void qf32_normalize(float r[4], float const q[4], float epsilon)
{
	float l2 = V4_DOT(q, q);
	float l = sqrtf(l2 + epsilon);
	r[0] = q[0] / l;
	r[1] = q[1] / l;
	r[2] = q[2] / l;
	r[3] = q[3] / l;
}

void v3f32_normalize(float r[3], float const q[3], float epsilon)
{
	float l2 = V3_DOT(q, q);
	float l = sqrtf(l2 + epsilon);
	r[0] = q[0] / l;
	r[1] = q[1] / l;
	r[2] = q[2] / l;
}

void qf32_mul(float r[4], float const p[4], float const q[4])
{
	float t[4] = {0};
	t[0] = (p[3] * q[0]) + (p[0] * q[3]) + (p[1] * q[2]) - (p[2] * q[1]);
	t[1] = (p[3] * q[1]) - (p[0] * q[2]) + (p[1] * q[3]) + (p[2] * q[0]);
	t[2] = (p[3] * q[2]) + (p[0] * q[1]) - (p[1] * q[0]) + (p[2] * q[3]);
	t[3] = (p[3] * q[3]) - (p[0] * q[0]) - (p[1] * q[1]) - (p[2] * q[2]);
	r[0] = t[0];
	r[1] = t[1];
	r[2] = t[2];
	r[3] = t[3];
}

void qf32_unit_to_m4(float const q[4], m4f32 *r)
{
	float a = q[3];
	float b = q[0];
	float c = q[1];
	float d = q[2];
	float a2 = a * a;
	float b2 = b * b;
	float c2 = c * c;
	float d2 = d * d;
	// Column vector 0:
	r->c0[0] = a2 + b2 - c2 - d2;
	r->c0[1] = 2.0f * (b * c + a * d);
	r->c0[2] = 2.0f * (b * d - a * c);
	// Column vector 1:
	r->c1[0] = 2.0f * (b * c - a * d);
	r->c1[1] = a2 - b2 + c2 - d2;
	r->c1[2] = 2.0f * (c * d + a * b);
	// Column vector 2:
	r->c2[0] = 2.0f * (b * d + a * c);
	r->c2[1] = 2.0f * (c * d - a * b);
	r->c2[2] = a2 - b2 - c2 + d2;
}

void qf32_unit_to_m3(float const q[4], m3f32 *r)
{
	float a = q[3];
	float b = q[0];
	float c = q[1];
	float d = q[2];
	float a2 = a * a;
	float b2 = b * b;
	float c2 = c * c;
	float d2 = d * d;
	// Column vector 0:
	r->c0[0] = a2 + b2 - c2 - d2;
	r->c0[1] = 2.0f * (b * c + a * d);
	r->c0[2] = 2.0f * (b * d - a * c);
	// Column vector 1:
	r->c1[0] = 2.0f * (b * c - a * d);
	r->c1[1] = a2 - b2 + c2 - d2;
	r->c1[2] = 2.0f * (c * d + a * b);
	// Column vector 2:
	r->c2[0] = 2.0f * (b * d + a * c);
	r->c2[1] = 2.0f * (c * d - a * b);
	r->c2[2] = a2 - b2 - c2 + d2;
}

void qf32_from_euler(float q[4], float pitch, float yaw, float roll)
{
	float cr = cos(roll * 0.5f);
	float sr = sin(roll * 0.5f);
	float cp = cos(pitch * 0.5f);
	float sp = sin(pitch * 0.5f);
	float cy = cos(yaw * 0.5f);
	float sy = sin(yaw * 0.5f);
	q[0] = cr * cp * cy + sr * sp * sy;
	q[1] = sr * cp * cy - cr * sp * sy;
	q[2] = cr * sp * cy + sr * cp * sy;
	q[3] = cr * cp * sy - sr * sp * cy;
}

void qf32_rotate_vector(float const q[4], float const v[3], float output[3])
{
	assert(output != NULL);
	float result[3];
	float ww = q[3] * q[3];
	float xx = q[0] * q[0];
	float yy = q[1] * q[1];
	float zz = q[2] * q[2];
	float wx = q[3] * q[0];
	float wy = q[3] * q[1];
	float wz = q[3] * q[2];
	float xy = q[0] * q[1];
	float xz = q[0] * q[2];
	float yz = q[1] * q[2];

	// Formula from http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/transforms/index.htm
	// p2.x = w*w*p1.x + 2*y*w*p1.z - 2*z*w*p1.y + x*x*p1.x + 2*y*x*p1.y + 2*z*x*p1.z - z*z*p1.x - y*y*p1.x;
	// p2.y = 2*x*y*p1.x + y*y*p1.y + 2*z*y*p1.z + 2*w*z*p1.x - z*z*p1.y + w*w*p1.y - 2*x*w*p1.z - x*x*p1.y;
	// p2.z = 2*x*z*p1.x + 2*y*z*p1.y + z*z*p1.z - 2*w*y*p1.x - y*y*p1.z + 2*w*x*p1.y - x*x*p1.z + w*w*p1.z;

	result[0] = ww * v[0] + 2 * wy * v[2] - 2 * wz * v[1] +
	            xx * v[0] + 2 * xy * v[1] + 2 * xz * v[2] -
	            zz * v[0] - yy * v[0];
	result[1] = 2 * xy * v[0] + yy * v[1] + 2 * yz * v[2] +
	            2 * wz * v[0] - zz * v[1] + ww * v[1] -
	            2 * wx * v[2] - xx * v[1];
	result[2] = 2 * xz * v[0] + 2 * yz * v[1] + zz * v[2] -
	            2 * wy * v[0] - yy * v[2] + 2 * wx * v[1] -
	            xx * v[2] + ww * v[2];
	// Copy result to output
	output[0] = result[0];
	output[1] = result[1];
	output[2] = result[2];
}