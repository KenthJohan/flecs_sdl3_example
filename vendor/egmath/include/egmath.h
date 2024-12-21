
#pragma once

#include <math.h>
#include <assert.h>
#include <stdio.h>

#define MAX(a, b) (((a) > (b)) ? a : b)
#define MIN(a, b) (((a) < (b)) ? a : b)

#define V1_DOT(a, b) ((a)[0] * (b)[0])
#define V2_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1])
#define V3_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])
#define V4_DOT(a, b) ((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2] + (a)[3] * (b)[3])

#define V1_DOTE_(a, b0) ((a)[0] * b0)
#define V2_DOTE_(a, b0, b1) ((a)[0] * b0 + (a)[1] * b1)
#define V3_DOTE_(a, b0, b1, b2) ((a)[0] * b0 + (a)[1] * b1 + (a)[2] * b2)
#define V4_DOTE_(a, b0, b1, b2, b3) ((a)[0] * b0 + (a)[1] * b1 + (a)[2] * b2 + (a)[3] * b3)

#define V1_DOTE(...) V1_DOTE_(__VA_ARGS__)
#define V2_DOTE(...) V2_DOTE_(__VA_ARGS__)
#define V3_DOTE(...) V3_DOTE_(__VA_ARGS__)
#define V4_DOTE(...) V4_DOTE_(__VA_ARGS__)

#define V1_ARG(a) (a[0])
#define V2_ARG(a) (a[0]), (a[1])
#define V3_ARG(a) (a[0]), (a[1]), (a[2])
#define V4_ARG(a) (a[0]), (a[1]), (a[2]), (a[3])

#define M3_R0(a) ((a).c0[0]), ((a).c1[0]), ((a).c2[0])
#define M3_R1(a) ((a).c0[1]), ((a).c1[1]), ((a).c2[1])
#define M3_R2(a) ((a).c0[2]), ((a).c1[2]), ((a).c2[2])

#define M4_R0(a) ((a).c0[0]), ((a).c1[0]), ((a).c2[0]), ((a).c3[0])
#define M4_R1(a) ((a).c0[1]), ((a).c1[1]), ((a).c2[1]), ((a).c3[1])
#define M4_R2(a) ((a).c0[2]), ((a).c1[2]), ((a).c2[2]), ((a).c3[2])
#define M4_R3(a) ((a).c0[3]), ((a).c1[3]), ((a).c2[3]), ((a).c3[3])

#define V3_CROSS0(a, b) ((a)[1] * (b)[2] - (a)[2] * (b)[1])
#define V3_CROSS1(a, b) ((a)[2] * (b)[0] - (a)[0] * (b)[2])
#define V3_CROSS2(a, b) ((a)[0] * (b)[1] - (a)[1] * (b)[0])

typedef struct
{
	float c0[3];
	float c1[3];
	float c2[3];
} m3f32;

typedef struct
{
	float c0[4];
	float c1[4];
	float c2[4];
	float c3[4];
} m4f32;

// clang-format off
#define SWAP(T, a, b) do {T tmp = a;a = b;b = tmp;} while (0)
#define QF32_IDENTITY {0, 0, 0, 1}
#define M3_IDENTITY {{1, 0, 0}, {0, 1, 0}, { 0, 0, 1 }}
#define M4_IDENTITY {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, { 0, 0, 0, 1 }}
// clang-format on

void qf32_xyza(float q[4], float x, float y, float z, float a);
void qf32_normalize(float r[4], float const q[4], float epsilon);
void qf32_mul(float r[4], float const p[4], float const q[4]);
void qf32_unit_to_m4(float const q[4], m4f32 *r);
void qf32_unit_to_m3(float const q[4], m3f32 *r);
void qf32_from_euler(float q[4], float pitch, float yaw, float roll);
void qf32_rotate_vector(float const q[4], float const v[3], float output[3]);


void v3f32_xyz(float v[3], float x, float y, float z);
void v3f32_cross(int const a[3], int const b[], int p[]);
void v3f32_normalize(float r[3], float const q[3], float epsilon);
void v3f32_mul(float r[3], float const a[3], float b);
void v3f32_add(float r[3], float const a[3], float const b[3]);
float v3f32_l1l2_distance(float const p1[3], float const v1[3], float const p2[3], float const v2[3]);
void v3f32_proj(float const bnorm[3], float const a[3], float r[3]);
void v3f32_oproj(float const bnorm[3], float const a[3], float r[3]);
void v3f32_print(float const x[3]);
float v3f32_plane_point_line_distance(float const v[3], float const l0[3], float const c[3], float const n[3]);
float v3f32_distance(float const a[3], float const b[3]);
float v3f32_distance2(float const a[3], float const b[3]);



void m4f32_perspective1(m4f32 *m, float fov, float aspect, float n, float f);
void m4f32_translation3(m4f32 *m, float const t[3]);
void m4f32_transpose(m4f32 *x);
void m4f32_mul_transpose(m4f32 *y, m4f32 const *at, m4f32 *const b);
void m4f32_mul(m4f32 *y, m4f32 const *a, m4f32 const *b);
void m4f32_mulv(m4f32 const *a, float const x[4], float y[4]);
void m4f32_print(m4f32 const *x);
void m4f32_inverse(float const m[16], float o[16]);
void m4f32_trs(float const t[3], float const q[4], float const s[3], m4f32 *r);
void m4f32_trs_inverse(float const t[3], float const q[4], float const s[3], m4f32 *r);

void m3f32_mulv(m3f32 const *a, float const x[3], float y[3]);
void m3f32_tmulv(m3f32 const *a, float const x[3], float y[3]);
void m3f32_mul(m3f32 *y, m3f32 const *a, m3f32 const *b);
void m3f32_transpose(m3f32 *x);
void m3f32_rs_inverse(float const q[4], float const s[3], m3f32 *r);
void m3f32_rs_inverse_transposed(float const q[4], float const s[3], m3f32 *r);

void v4f32_xyzw(float v[4], float x, float y, float z, float w);
void v4f32_mul(float r[4], float const a[4], float b);
void v4f32_add(float r[4], float const a[4], float const b[4]);
void v4f32_print(float const x[4]);



/**
 * @brief Calculates two intesections point on a infinite elliptic cylinder
 *
 * @param v Vector that defines the line direction in world space
 * @param l Point on the line in world space
 * @param cylinder Center of the elliptic cylinder in world space
 * @param h Elliptic cylinder axis vector
 * @param tt (Transposed) The transform matrix is the result of the composition of the matrices that describe translation, rotation and scale of the scaled cylinder.
 * @return
 */
float v3f32_intersect_cylinder_i0i1(float const v[3], float const l[3], float const cylinder[3], float const h[3], m3f32 const * tt, float i0[3], float i1[3]);

/**
 * @brief Check if line intesects a finite elliptic cylinder
 *
 * @param v Vector that defines the line direction in world space
 * @param l Point on the line in world space
 * @param cylinder Center of the elliptic cylinder in world space
 * @param h Elliptic cylinder axis vector
 * @param tt (Transposed) The transform matrix is the result of the composition of the matrices that describe translation, rotation and scale of the scaled cylinder.
 * @return
 */
int v3f32_intersect_cylinder(float const v[3], float const l[3], float const cylinder[3], float const h[3], m3f32 const *tt);




void test_m4f32_trs();

