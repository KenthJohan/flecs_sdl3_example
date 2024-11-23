#include "egmath.h"

void test_m4f32_trs()
{
	float t[3] = {1, 2, 3};
	float q[4];
	float s[3] = {4, 5, 6};
	qf32_from_euler(q, 123, 22, 34564567);
	m4f32 m1;
	m4f32 m2;
	m4f32 m3;
	m4f32_trs(t, q, s, &m1);
	m4f32_trs_inverse(t, q, s, &m2);
	m4f32_inverse((float*)&m1, (float*)&m3);
	m4f32_print(&m1);
	m4f32_print(&m2);
	m4f32_print(&m3);
}
