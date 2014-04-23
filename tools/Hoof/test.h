#ifndef TEST_H_GUARD
#define TEST_H_GUARD

#include <stdlib.h>

#define FOO
#define BAR 1
#define FUNC(a,b,c) (a+b+c)

#if defined(FOO) && \
	defined BAR
#define BAZ 1
#else /* FOO */
#define BAZ 2
#endif

#endif /* TEST_H_GUARD */

#define BBBB AAA\
 CCC
 
/* this
 * is a
 * multiline
 * comment
 */

#if !defined FOO && -BAR || BAZ
#define WE_GOT_THIS 1
#else
#define WE_GOT_THIS 0
#endif

#if (defined FOO && \
	defined BAR)
#define WE_GOOD 1
#else
#define WE_GOOD 0
#endif

#if WE_GOOD

#endif

