#ifndef LUAGLUE_STACKTEMPLATES_NUMERIC_H_GUARD
#define LUAGLUE_STACKTEMPLATES_NUMERIC_H_GUARD

	inline float stack<float>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	inline void stack<float>::put(LuaGlueBase *, lua_State *s, float v)
	{
		lua_pushnumber(s, v);
	}

	inline float stack<float&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	inline void stack<float&>::put(LuaGlueBase *, lua_State *s, float &v)
	{
		lua_pushnumber(s, v);
	}

	inline float stack<const float&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}

	inline void stack<const float&>::put(LuaGlueBase *, lua_State *s, const float &v)
	{
		lua_pushnumber(s, v);
	}

	inline double stack<double>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	inline void stack<double>::put(LuaGlueBase *, lua_State *s, double v)
	{
		lua_pushnumber(s, v);
	}

	inline double stack<double&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}

	inline void stack<double&>::put(LuaGlueBase *, lua_State *s, double &v)
	{
		lua_pushnumber(s, v);
	}
	
	inline double stack<const double&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	inline void stack<const double&>::put(LuaGlueBase *, lua_State *s, const double &v)
	{
		lua_pushnumber(s, v);
	}
	
	inline bool stack<bool>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		luaL_checktype (s, idx, LUA_TBOOLEAN);
		return lua_toboolean (s, idx);
	}

	inline void stack<bool>::put(LuaGlueBase *, lua_State *s, bool v)
	{
		lua_pushboolean(s, v);
	}

	inline bool stack<bool&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		luaL_checktype (s, idx, LUA_TBOOLEAN);
		return lua_toboolean (s, idx);
	}

	inline void stack<bool&>::put(LuaGlueBase *, lua_State *s, bool &v)
	{
		lua_pushboolean(s, v);
	}

	inline bool stack<const bool&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		luaL_checktype (s, idx, LUA_TBOOLEAN);
		return lua_toboolean (s, idx);
	}

	inline void stack<const bool&>::put(LuaGlueBase *, lua_State *s, const bool &v)
	{
		lua_pushboolean(s, v);
	}

#endif /* LUAGLUE_STACKTEMPLATES_NUMERIC_H_GUARD */
