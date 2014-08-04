#ifndef LUAGLUE_STACKTEMPLATES_INTEGER_H_GUARD
#define LUAGLUE_STACKTEMPLATES_INTEGER_H_GUARD

	int stack<int&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		int v = luaL_checkint(s, idx);
		LG_Debug("stack::get<int&>:%i: %i", idx, v);
		return v;
	}
	
	void stack<int&>::put(LuaGlueBase *, lua_State *s, int &v)
	{
		LG_Debug("stack::put<int&>: %i", v);
		lua_pushinteger(s, v);
	}

	int stack<const int&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		int v = luaL_checkint(s, idx);
		LG_Debug("stack::get<const int&>:%i: %i", idx, v);
		return v;
	}
	
	void stack<const int&>::put(LuaGlueBase *, lua_State *s, const int &v)
	{
		LG_Debug("stack::put<const int&>: %i", v);
		lua_pushinteger(s, v);
	}

	int stack<int>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		int v = luaL_checkint(s, idx);
		LG_Debug("stack::get<int>:%i: %i", idx, v);
		return v;
	}
	
	void stack<int>::put(LuaGlueBase *, lua_State *s, int v)
	{
		LG_Debug("stack::put<int>: %i", v);
		lua_pushinteger(s, v);
	}

	unsigned int stack<unsigned int&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkinteger(s, idx);
	}
	
	void stack<unsigned int&>::put(LuaGlueBase *, lua_State *s, unsigned int &v)
	{
		lua_pushinteger(s, v);
	}

	unsigned int stack<unsigned int>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkinteger(s, idx);
	}
	
	void stack<unsigned int>::put(LuaGlueBase *, lua_State *s, unsigned int v)
	{
		lua_pushinteger(s, v);
	}

	unsigned int stack<const unsigned int&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkinteger(s, idx);
	}
	
	void stack<const unsigned int&>::put(LuaGlueBase *, lua_State *s, const unsigned int &v)
	{
		lua_pushinteger(s, v);
	}

// char

	char stack<char>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkinteger(s, idx);
	}
	
	void stack<char>::put(LuaGlueBase *, lua_State *s, char v)
	{
		lua_pushinteger(s, v);
	}

	char stack<char&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkinteger(s, idx);
	}
	
	void stack<char&>::put(LuaGlueBase *, lua_State *s, char &v)
	{
		lua_pushinteger(s, v);
	}

	char stack<const char&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkinteger(s, idx);
	}
	
	void stack<const char&>::put(LuaGlueBase *, lua_State *s, const char &v)
	{
		lua_pushinteger(s, v);
	}

#endif /* LUAGLUE_STACKTEMPLATES_INTEGER_H_GUARD */
