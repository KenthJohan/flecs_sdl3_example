#pragma once

#include "matrix.h"

#include <SDL3/SDL_stdinc.h>

/*
 * Simulates desktop's glRotatef. The matrix is returned in column-major
 * order.
 */
void rotate_matrix(float angle, float x, float y, float z, float *r);

/*
 * Simulates gluPerspectiveMatrix
 */
void perspective_matrix(float fovy, float aspect, float znear, float zfar, float *r);
/*
 * Multiplies lhs by rhs and writes out to r. All matrices are 4x4 and column
 * major. In-place multiplication is supported.
 */
void multiply_matrix(float *lhs, float *rhs, float *r);