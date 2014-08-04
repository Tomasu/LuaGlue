#ifndef LUAGLUE_STACKTEMPLATES_H_GUARD
#define LUAGLUE_STACKTEMPLATES_H_GUARD

	bool stack<bool>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return lua_toboolean(s, idx) ? true : false;
	}
	
	void stack<bool>::put(LuaGlueBase *, lua_State *s, bool v)
	{
		lua_pushboolean(s, v);
	}

	bool stack<bool&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return lua_toboolean(s, idx) ? true : false;
	}
	
	void stack<bool&>::put(LuaGlueBase *, lua_State *s, bool &v)
	{
		lua_pushboolean(s, v);
	}
	
	bool stack<const bool&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return lua_toboolean(s, idx) ? true : false;
	}
	
	void stack<const bool&>::put(LuaGlueBase *, lua_State *s, const bool &v)
	{
		lua_pushboolean(s, v);
	}

#endif /* LUAGLUE_STACKTEMPLATES_H_GUARD */
