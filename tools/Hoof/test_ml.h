#if FOO && \
	defined BAR
#define BAZ 1
#else /* FOO */
#define BAZ 2
#endif