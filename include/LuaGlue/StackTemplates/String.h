#ifndef LUAGLUE_STACKTEMPLATES_STRING_H_GUARD
#define LUAGLUE_STACKTEMPLATES_STRING_H_GUARD

	const char *stack<const char *&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}
	
	void stack<const char *&>::put(LuaGlueBase *, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}

	const char *stack<const char * const &>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}
	
	void stack<const char * const &>::put(LuaGlueBase *, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}

	const char *stack<const char *>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}

	void stack<const char *>::put(LuaGlueBase *, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}

	template<size_t N>
	const char *stack<const char (&)[N]>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}
	
	template<size_t N>
	void stack<const char (&)[N]>::put(LuaGlueBase *, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}

	std::string stack<std::string>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		const char *str = luaL_checkstring(s, idx);
		LG_Debug("str: %s", str);
		return str;
	}
	
	void stack<std::string>::put(LuaGlueBase *, lua_State *s, std::string v)
	{
		lua_pushstring(s, v.c_str());
	}
	
	void stack<std::string>::put(LuaGlueBase *, lua_State *s, std::string *v)
	{
		LG_Debug("str: %s", v->c_str());
		lua_pushstring(s, v->c_str());
	}

	std::string stack<std::string &>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		const char *str = luaL_checkstring(s, idx);
		LG_Debug("str: %s", str);
		return str;
	}

	void stack<std::string &>::put(LuaGlueBase *, lua_State *s, std::string v)
	{
		lua_pushstring(s, v.c_str());
	}
	
	void stack<std::string &>::put(LuaGlueBase *, lua_State *s, std::string *v)
	{
		LG_Debug("str: %s", v->c_str());
		lua_pushstring(s, v->c_str());
	}

	std::string stack<const std::string&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		const char *str = luaL_checkstring(s, idx);
		LG_Debug("str: %s", str);
		return str;
	}
	
	void stack<const std::string&>::put(LuaGlueBase *, lua_State *s, std::string v)
	{
		lua_pushstring(s, v.c_str());
	}
	
	void stack<const std::string&>::put(LuaGlueBase *, lua_State *s, std::string *v)
	{
		LG_Debug("str: %s", v->c_str());
		lua_pushstring(s, v->c_str());
	}

#endif /* LUAGLUE_STACKTEMPLATES_STRING_H_GUARD */
