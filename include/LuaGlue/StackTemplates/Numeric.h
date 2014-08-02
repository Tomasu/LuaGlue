#ifndef LUAGLUE_STACKTEMPLATES_NUMERIC_H_GUARD
#define LUAGLUE_STACKTEMPLATES_NUMERIC_H_GUARD

template<>
struct stack<float> {
	static float get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, float v)
	{
		lua_pushnumber(s, v);
	}
};

template<>
struct stack<float&> {
	static float get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, float v)
	{
		lua_pushnumber(s, v);
	}
};

template<>
struct stack<const float&> {
	static float get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, float v)
	{
		lua_pushnumber(s, v);
	}
};

template<>
struct stack<double> {
	static double get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, double v)
	{
		lua_pushnumber(s, v);
	}
};

template<>
struct stack<double&> {
	static double get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, double v)
	{
		lua_pushnumber(s, v);
	}
};

template<>
struct stack<const double&> {
	static double get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, double v)
	{
		lua_pushnumber(s, v);
	}
};

#endif /* LUAGLUE_STACKTEMPLATES_NUMERIC_H_GUARD */
