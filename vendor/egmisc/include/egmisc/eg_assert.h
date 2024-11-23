#pragma once

#define EG_DEBUG
#ifdef EG_DEBUG

#include <stdint.h>
#include <stdlib.h>

int eg_assert_(
	const char *expr, 
	const char *file, 
	int32_t line, 
	const char *fn, 
	const char *fmt, 
	...
	);

/** Assert. 
 * Aborts if condition is false, disabled in debug mode. */
#define eg_assert(expr, ...) ((expr) ? (void)0: (void)(eg_assert_(#expr, __FILE__, __LINE__, __func__, __VA_ARGS__), abort()))

/** Assert. 
 * Aborts if condition is false, disabled in debug mode. */
#define eg_assert_notnull(expr) eg_assert(expr, "%s", "Should not be NULL")


#else
#define eg_assert(expr, ...)
#define eg_assert_notnull(expr)
#endif
