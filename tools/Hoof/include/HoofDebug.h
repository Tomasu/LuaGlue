#ifndef HOOF_DEBUG_H_GUARD
#define HOOF_DEBUG_H_GUARD

#ifdef LG_DEBUG

#include <stdarg.h>

static inline void HF_Debug_(const char *FILE, const char *FUNCTION, int LINE, const char *t, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	int msg_len = vsnprintf(0, 0, format, ap);
	va_end(ap);
	
	char *msg = new char[msg_len+1];
	if(!msg)
		return;
	
	msg[msg_len] = 0;
	
	va_start(ap, format);
	vsnprintf(msg, (size_t)msg_len+1, format, ap);
	va_end(ap);
	
	const char *fptr = strrchr(FILE, '/');
	if(!fptr)
		fptr = strrchr(FILE, '\\');
	
	if(!fptr)
		fptr = FILE;
	else
		fptr++;
	
	//char *buff = (char*)calloc(1, strlen(FUNCTION)+1);
	//LG_Debug_subst_symbol(FUNCTION, buff);
	fprintf(stderr, "%s:%s:%i: %s: %s\n", fptr, FUNCTION, LINE, t, msg);
	//(void)FUNCTION;
	//printf("%s:%i: %s: %s\n", fptr, LINE, t, msg);
	
	delete[] msg;
}

#define HF_Debug(a, ...) HF_Debug_(__FILE__, __FUNCTION__, __LINE__, "D", a, ##__VA_ARGS__)
#define HF_Warn(a, ...) HF_Debug_(__FILE__, __FUNCTION__, __LINE__, "W", a, ##__VA_ARGS__)
#define HF_Error(a, ...) HF_Debug_(__FILE__, __FUNCTION__, __LINE__, "E", a, ##__VA_ARGS__)

#else

#define HF_Debug(a, ...) ((void)a)
#define HF_Warn(a, ...) ((void)a)
#define HF_Error(a, ...) ((void)a)

#endif /* !defined LG_DEBUG */

#endif /* HOOF_DEBUG_H_GUARD */
