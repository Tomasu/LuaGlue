#ifndef LUAGLUE_STACKTEMPLATES_H_GUARD
#define LUAGLUE_STACKTEMPLATES_H_GUARD

template<>
struct stack<bool> {
	static bool get(LuaGlueBase *, lua_State *s, int idx)
	{
		return lua_toboolean(s, idx) ? true : false;
	}
	
	static void put(LuaGlueBase *, lua_State *s, bool v)
	{
		lua_pushboolean(s, v);
	}
};

template<>
struct stack<bool&> {
	static bool get(LuaGlueBase *, lua_State *s, int idx)
	{
		return lua_toboolean(s, idx) ? true : false;
	}
	
	static void put(LuaGlueBase *, lua_State *s, bool v)
	{
		lua_pushboolean(s, v);
	}
};

template<>
struct stack<const bool&> {
	static bool get(LuaGlueBase *, lua_State *s, int idx)
	{
		return lua_toboolean(s, idx) ? true : false;
	}
	
	static void put(LuaGlueBase *, lua_State *s, bool v)
	{
		lua_pushboolean(s, v);
	}
};

#endif /* LUAGLUE_STACKTEMPLATES_H_GUARD */
