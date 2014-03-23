#ifndef LUAGLUE_DEBUG_H_GUARD
#define LUAGLUE_DEBUG_H_GUARD

#ifdef LG_DEBUG

#include <cstring>
static inline void LG_Debug_(const char *FILE, const char *FUNCTION, int LINE, const char *t, const char *format, ...)
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
	
	printf("%s:%s:%i: %s: %s\n", fptr, FUNCTION, LINE, t, msg);
	
	delete[] msg;
}

#define LG_Debug(a, ...) LG_Debug_(__FILE__, __PRETTY_FUNCTION__, __LINE__, "D", a, ##__VA_ARGS__)
#define LG_Warn(a, ...) LG_Debug_(__FILE__, __PRETTY_FUNCTION__, __LINE__, "W", a, ##__VA_ARGS__)
#define LG_Error(a, ...) LG_Debug_(__FILE__, __PRETTY_FUNCTION__, __LINE__, "E", a, ##__VA_ARGS__)

#else

#define LG_Debug(a, ...) ((void)a)
#define LG_Warn(a, ...) ((void)a)
#define LG_Error(a, ...) ((void)a)

#endif /* !defined LG_DEBUG */

#ifdef LUAGLUE_TYPECHECK

#include <LuaGlue/LuaGlueBase.h>
static inline void LG_TypeCheck(LuaGlueBase *g, const char *module, int type, int what = -1)
{
	int what_type = lua_type(g->state(), what);
	if(what_type != type)
	{
		char tmp[4096];
		snprintf(tmp, sizeof(tmp), "%s expected a %s, got a %s", module, lua_typename(g->state(), type), lua_typename(g->state(), lua_type(g->state(), -1)));
		lua_pushstring(g->state(), tmp);
		lua_error(g->state());
	}
}

//#define LG_TypeCheck(g, module, type, what) LG_TypeCheck_(g, module, what, type)

#else /* !LUAGLUE_TYPECHECK */

//#define LG_TypeCheck(g, module, type, what) (void)((g), (module), (type), (what))
#define LG_TypeCheck(g, module, type, what) (void)g

#endif /* LUAGLUE_TYPECHECK */

#endif /* LUAGLUE_DEBUG_H_GUARD */