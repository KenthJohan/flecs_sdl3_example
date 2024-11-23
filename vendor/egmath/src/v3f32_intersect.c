#include "egmath.h"

float v3f32_intersect_line_circle(float const v[3], float const l0[3], float const c[3], float const n[3], float r)
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
	return v3f32_distance2(c, i) < (r * r);
}

// https://math.stackexchange.com/questions/2613781/line-cylinder-intersection
void v3f32_intersect_cylinder_abc(float const v[3], float const h[3], float const w[3], float *a, float *b, float *c)
{
	float vv = V3_DOT(v, v);
	float vh = V3_DOT(v, h);
	float vw = V3_DOT(v, w);
	float wh = V3_DOT(w, h);
	float ww = V3_DOT(w, w);
	(*a) = vv - (vh * vh);
	(*b) = 2.0 * (vw - (vh * wh));
	(*c) = ww - (wh * wh) - 1.0;
}

float v3f32_intersect_cylinder_i0i1(float const v[3], float const l[3], float const cylinder[3], float const h[3], m3f32 const *tt, float i0[3], float i1[3])
{
	float tv[3] = {v[0], v[1], v[2]};
	float tw[3];
	tw[0] = l[0] - cylinder[0];
	tw[1] = l[1] - cylinder[1];
	tw[2] = l[2] - cylinder[2];
	m3f32_tmulv(tt, tv, tv);
	m3f32_tmulv(tt, tw, tw);
	float a;
	float b;
	float c;
	v3f32_intersect_cylinder_abc(tv, h, tw, &a, &b, &c);
	float b2_4ac = (b * b) - (4.0 * a * c);
	// The distance between the cylinder axis and the line is smaller than the cylinder radius (r = 1). There are two intersections at (i0) and (i1):
	if (b2_4ac > 0) {
		float sq = sqrtf(b2_4ac);
		float k1 = (-b + sq) / (2 * a);
		float k2 = (-b - sq) / (2 * a);
		i0[0] = l[0] + v[0] * k1;
		i0[1] = l[1] + v[1] * k1;
		i0[2] = l[2] + v[2] * k1;
		i1[3] = l[0] + v[0] * k2;
		i1[4] = l[1] + v[1] * k2;
		i1[5] = l[2] + v[2] * k2;
	}
	// float k = (-b) / (2 * a);
	return b2_4ac;
}

int v3f32_intersect_cylinder(float const v[3], float const l[3], float const cylinder[3], float const h[3], m3f32 const *tt)
{
	float tv[3] = {v[0], v[1], v[2]};
	float tw[3];
	tw[0] = l[0] - cylinder[0];
	tw[1] = l[1] - cylinder[1];
	tw[2] = l[2] - cylinder[2];
	m3f32_tmulv(tt, tv, tv);
	m3f32_tmulv(tt, tw, tw);
	float a;
	float b;
	float c;
	v3f32_intersect_cylinder_abc(tv, h, tw, &a, &b, &c);
	float b2_4ac = (b * b) - (4.0 * a * c);
	// The distance between the cylinder axis and the line is smaller than the cylinder radius (r = 1).
	if (b2_4ac > 0) {
		float sq = sqrtf(b2_4ac);
		float k1 = (-b + sq) / (2 * a);
		float k2 = (-b - sq) / (2 * a);
		// There are two intersections at (i0) and (i1):
		float i0[3];
		float i1[3];
		i0[0] = l[0] + v[0] * k1 - cylinder[0];
		i0[1] = l[1] + v[1] * k1 - cylinder[1];
		i0[2] = l[2] + v[2] * k1 - cylinder[2];
		i1[0] = l[0] + v[0] * k2 - cylinder[0];
		i1[1] = l[1] + v[1] * k2 - cylinder[1];
		i1[2] = l[2] + v[2] * k2 - cylinder[2];
		// Transform intersections to unit and based in the origin:
		m3f32_tmulv(tt, i0, i0);
		m3f32_tmulv(tt, i1, i1);
		// Check for intesection in non infinite cylinder:
		// The y axis is the cylinder axis.
		// Intesection happens when point-y is less than radius = 1:
		if ((fabs(i0[1]) < 0.5) || (fabs(i1[1]) < 0.5)) {
			return 1;
		}
	}
	// float k = (-b) / (2 * a);
	return 0;
}

#if 0
/**
 * @brief Convert fs_readfile error to string
 *
 * @param v Vector that defines the line direction in world space
 * @param l0 Point on the line in world space
 * @param c Center of the elliptic cylinder in world space
 * @param h Elliptic cylinder height vector (along local z axis)
 * @param t The transform matrix is the result of the composition of the matrices that describe translation, rotation and scale of the scaled cylinder.
 * @return
 */
float v3f32_intersect_cylinder1(float const v[3], float const l[3], float const c[3], float const h[3], m4f32 const *t)
{
	float tv[4] = {v[0], v[1], v[2], 0.0};
	float tl[4] = {l[0], l[1], l[2], 1.0};
	float tc[4] = {c[0], c[1], c[2], 1.0};
	m4f32_mulv(t, tl, tl);
	m4f32_mulv(t, tc, tc);
	m4f32_mulv(t, tv, tv);
	float tw[3];
	tw[0] = tl[0] - tc[0];
	tw[1] = tl[1] - tc[1];
	tw[2] = tl[2] - tc[2];
	float b2_4ac = v3f32_intersect_cylinder_b2_4ac(tv, h, tw);
	return b2_4ac;
}

/**
 * @brief Convert fs_readfile error to string
 *
 * @param v Vector that defines the line direction in world space
 * @param l0 Point on the line in world space
 * @param c Center of the elliptic cylinder in world space
 * @param h Elliptic cylinder height vector (along local z axis)
 * @param t The transform matrix is the result of the composition of the matrices that describe translation, rotation and scale of the scaled cylinder.
 * @return
 */
float v3f32_intersect_cylinder2(float const v[3], float const l[3], float const c[3], float const h[3], m4f32 const *t)
{
	float tv[4] = {v[0], v[1], v[2], 0.0};
	// float tl[4] = {l[0], l[1], l[2], 1.0};
	// float tc[4] = {c[0], c[1], c[2], 1.0};
	// m4f32_mulv(t, tl, tl);
	// m4f32_mulv(t, tc, tc);
	m4f32_mulv(t, tv, tv);
	float tw[4];
	tw[0] = l[0] - c[0];
	tw[1] = l[1] - c[1];
	tw[2] = l[2] - c[2];
	tw[3] = 0.0;
	m4f32_mulv(t, tw, tw);
	float b2_4ac = v3f32_intersect_cylinder_b2_4ac(tv, h, tw);
	return b2_4ac;
}
#endif