#ifndef LUAGLUE_STACKTEMPLATES_NUMERIC_H_GUARD
#define LUAGLUE_STACKTEMPLATES_NUMERIC_H_GUARD

	float stack<float>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	void stack<float>::put(LuaGlueBase *, lua_State *s, float v)
	{
		lua_pushnumber(s, v);
	}

	float stack<float&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	void stack<float&>::put(LuaGlueBase *, lua_State *s, float &v)
	{
		lua_pushnumber(s, v);
	}

	float stack<const float&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}

	void stack<const float&>::put(LuaGlueBase *, lua_State *s, const float &v)
	{
		lua_pushnumber(s, v);
	}

	double stack<double>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	void stack<double>::put(LuaGlueBase *, lua_State *s, double v)
	{
		lua_pushnumber(s, v);
	}

	double stack<double&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}

	void stack<double&>::put(LuaGlueBase *, lua_State *s, double &v)
	{
		lua_pushnumber(s, v);
	}
	
	double stack<const double&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	void stack<const double&>::put(LuaGlueBase *, lua_State *s, const double &v)
	{
		lua_pushnumber(s, v);
	}

#endif /* LUAGLUE_STACKTEMPLATES_NUMERIC_H_GUARD */
