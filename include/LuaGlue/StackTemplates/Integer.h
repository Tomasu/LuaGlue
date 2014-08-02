#ifndef LUAGLUE_STACKTEMPLATES_INTEGER_H_GUARD
#define LUAGLUE_STACKTEMPLATES_INTEGER_H_GUARD

template<>
struct stack<int&> {
	static int get(LuaGlueBase *, lua_State *s, int idx)
	{
		int v = luaL_checkint(s, idx);
		LG_Debug("stack::get<int&>:%i: %i", idx, v);
		return v;
	}
	
	static void put(LuaGlueBase *, lua_State *s, int &v)
	{
		LG_Debug("stack::put<int&>: %i", v);
		lua_pushinteger(s, v);
	}
};

template<>
struct stack<const int&> {
	static int get(LuaGlueBase *, lua_State *s, int idx)
	{
		int v = luaL_checkint(s, idx);
		LG_Debug("stack::get<const int&>:%i: %i", idx, v);
		return v;
	}
	
	static void put(LuaGlueBase *, lua_State *s, int &v)
	{
		LG_Debug("stack::put<const int&>: %i", v);
		lua_pushinteger(s, v);
	}
};

template<>
struct stack<int> {
	static int get(LuaGlueBase *, lua_State *s, int idx)
	{
		int v = luaL_checkint(s, idx);
		LG_Debug("stack::get<int>:%i: %i", idx, v);
		return v;
	}
	
	static void put(LuaGlueBase *, lua_State *s, int &v)
	{
		LG_Debug("stack::put<int>: %i", v);
		lua_pushinteger(s, v);
	}
};

/*
template<>
struct stack<int*&> {
	static int *get(LuaGlueBase *, lua_State *s, int idx)
	{
		LG_Debug("stack::get<int *&>:%i", idx);
		return (int *)luaL_checkinteger(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, int *v)
	{
		lua_pushinteger(s, (lua_Integer)v);
	}
};

template<>
struct stack<int*> {
	static int *get(LuaGlueBase *, lua_State *s, int idx)
	{
		LG_Debug("stack::get<int *>:%i", idx);
		return (int *)luaL_checkinteger(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, int *v)
	{
		lua_pushinteger(s, (lua_Integer)v);
	}
};*/

template<>
struct stack<unsigned int> {
	static unsigned int get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkinteger(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, unsigned int v)
	{
		lua_pushinteger(s, v);
	}
};

template<>
struct stack<unsigned int&> {
	static unsigned int get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkinteger(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, unsigned int v)
	{
		lua_pushinteger(s, v);
	}
};

template<>
struct stack<const unsigned int&> {
	static unsigned int get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkinteger(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, unsigned int v)
	{
		lua_pushinteger(s, v);
	}
};

// char

template<>
struct stack<char> {
	static char get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkinteger(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, char v)
	{
		lua_pushinteger(s, v);
	}
};

template<>
struct stack<char&> {
	static char get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkinteger(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, char v)
	{
		lua_pushinteger(s, v);
	}
};

template<>
struct stack<const char&> {
	static char get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkinteger(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, char v)
	{
		lua_pushinteger(s, v);
	}
};

#endif /* LUAGLUE_STACKTEMPLATES_INTEGER_H_GUARD */
