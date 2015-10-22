#ifndef LUAGLUE_DEBUG_H_GUARD
#define LUAGLUE_DEBUG_H_GUARD

#include <cstring>
#include <vector>
#include <cstdio>
#include <cstdarg>

enum {
	LG_LOG_NONE = 0,
	LG_LOG_TRACE,
	LG_LOG_DEBUG,
	LG_LOG_WARN,
	LG_LOG_ERROR
};

static inline void LG_Log_(const char *FILE, const char *FUNCTION, int LINE, int mode, const char *format, ...)
{
	static const char *mode_str[] = {
		"N", "T", "D", "W", "E", 0
	};
	
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
	
	if(LG_DEBUG)
	{
		// log everything, with file/line/function details, in debug mode
		const char *fptr = strrchr(FILE, '/');
		if(!fptr)
			fptr = strrchr(FILE, '\\');
		
		if(!fptr)
			fptr = FILE;
		else
			fptr++;
	
		fprintf(stderr, "%s:%s:%i: %s: %s\n", fptr, FUNCTION, LINE, mode_str[mode], msg);
	}
	else if(mode > LG_LOG_DEBUG)
	{
		// only log warnings and errors in release mode.
		fprintf(stderr, "%s: %s\n", mode_str[mode], msg);
	}
	
	delete[] msg;
}

#define LG_Debug(a, ...) LG_Log_(__FILE__, __FUNCTION__, __LINE__, LG_LOG_DEBUG, a, ##__VA_ARGS__)
#define LG_Trace(a, ...) LG_Log_(__FILE__, __FUNCTION__, __LINE__, LG_LOG_TRACE, a, ##__VA_ARGS__)
#define LG_Warn(a, ...) LG_Log_(__FILE__, __FUNCTION__, __LINE__, LG_LOG_WARN, a, ##__VA_ARGS__)
#define LG_Error(a, ...) LG_Log_(__FILE__, __FUNCTION__, __LINE__, LG_LOG_ERROR, a, ##__VA_ARGS__)

#include <LuaGlue/LuaGlueBase.h>
static inline void LG_TypeCheck(LuaGlueBase *g, const char *module, int type, int what = -1)
{
	if(LUAGLUE_TYPECHECK)
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
}


#endif /* LUAGLUE_DEBUG_H_GUARD */