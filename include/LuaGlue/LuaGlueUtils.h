#ifndef LuaGlueUtils_H_GUARD
#define LuaGlueUtils_H_GUARD

#ifdef HAVE_CXXABI_H
#include <cxxabi.h>
#endif

#include <string>
#include <stdlib.h>

#define CxxDemangle(T) ((const char *)CxxDemangle_<T>())

template<typename T>
class CxxDemangle_
{
	char *realname;
	int status;
	public:
		CxxDemangle_()
		{
#ifdef HAVE_CXXABI_H
			realname = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
#else
			// some compilers don't have a way to demangle symbols :(
			// just use the mangled name instead.
			realname = strdup(typeid(T).name());
#endif
		}
		
		~CxxDemangle_() { free(realname); }
		operator const char *()
		{
			return realname;
		}
};

std::string lua_demangle_sym(const char *sym);
inline std::string lua_demangle_sym(const char *sym)
{
#ifdef HAVE_CXXABI_H
	int status = 0;
	char *s = abi::__cxa_demangle(sym, 0, 0, &status);
	std::string str = s ? s : sym;
	free(s);
#else
	std::string str = sym;
#endif
	
	return str;
}

#define lua_dump_stack(L) lua_dump_stack_(__FILE__, __LINE__, __FUNCTION__, L)
inline void lua_dump_userdata(lua_State *L, int idx);

inline void lua_dump_stack_(const char *file, int line, const char *func, lua_State *L)
{
	int i;
	int top = lua_gettop(L);
	
	const char *fptr = strrchr(file, '/');
	if(!fptr)
		fptr = strrchr(file, '\\');
	
	if(!fptr)
		fptr = file;
	else
		fptr++;
	
	printf("%s:%s:%i: STACK: ", fptr, func, line);
	for (i = 1; i <= top; i++) {  /* repeat for each level */
		int t = lua_type(L, i);
		switch (t) {
			case LUA_TSTRING:  /* strings */
				printf("%i:`%s'", i, lua_tostring(L, i));
			break;

			case LUA_TBOOLEAN:  /* booleans */
				printf("%i:%s", i, lua_toboolean(L, i) ? "true" : "false");
			break;

			case LUA_TNUMBER:  /* numbers */
				printf("%i:%g", i, lua_tonumber(L, i));
			break;

			case LUA_TUSERDATA: { /* userdata */
				printf("%i:", i);
				lua_dump_userdata(L, i);
			} break;
			
			default:  /* other values */
			printf("%i:%s", i, lua_typename(L, t));
			break;

		}
		printf(", ");  /* put a separator */
	}
	printf("\n");  /* end the listing */
}

#include "LuaGlue/LuaGlueTypeBase.h"

inline void lua_dump_userdata(lua_State *L, int idx)
{
	int ret = luaL_getmetafield(L, idx, LuaGlueTypeBase::METATABLE_TYPENAME_FIELD); 
	if(!ret)
	{
		printf("udata(typename:%s)", lua_typename(L, lua_type(L, idx)));
		return;
	}
	
	const char *name luaL_checkstring(L, -1);
	lua_pop(L, 1);
	
	printf("udata(%s)", lua_demangle_sym(name).c_str());
}

inline void lua_dump_table(lua_State *L, int index)
{
	/* table is in the stack at index 'idx' */
	lua_pushvalue(L, index);

	lua_pushnil(L);  /* first key */
	while (lua_next(L, -2) != 0) {
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		
		lua_pushvalue(L, -2);
		
		printf("%s => ", lua_tostring(L, -1));
		int t = lua_type(L, -2);
		switch (t) {

			case LUA_TSTRING:  /* strings */
			printf("`%s'", lua_tostring(L, -2));
			break;

			case LUA_TBOOLEAN:  /* booleans */
			printf(lua_toboolean(L, -2) ? "true" : "false");
			break;

			case LUA_TNUMBER:  /* numbers */
			printf("%g", lua_tonumber(L, -2));
			break;

			default:  /* other values */
			printf("%s", lua_typename(L, t));
			break;

		}
		printf("\n");
		
		/* removes 'value'; keeps 'key' for next iteration */
		lua_pop(L, 2);
	}
	
	lua_pop(L, 1);
}

#endif /* LuaGlueUtils_H_GUARD */
