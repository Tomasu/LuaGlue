#ifndef LUAGLUE_STACKTEMPLATES_STRING_H_GUARD
#define LUAGLUE_STACKTEMPLATES_STRING_H_GUARD

	inline const char *stack<const char *&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}
	
	inline void stack<const char *&>::put(LuaGlueBase *, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}

	inline const char *stack<const char * const &>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}
	
	inline void stack<const char * const &>::put(LuaGlueBase *, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}

	inline const char *stack<const char *>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}

	inline void stack<const char *>::put(LuaGlueBase *, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}

	template<size_t N>
	inline const char *stack<const char (&)[N]>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}
	
	template<size_t N>
	inline void stack<const char (&)[N]>::put(LuaGlueBase *, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}

	inline std::string stack<std::string>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		const char *str = luaL_checkstring(s, idx);
		LG_Debug("str: %s", str);
		return str;
	}
	
	inline void stack<std::string>::put(LuaGlueBase *, lua_State *s, std::string v)
	{
		lua_pushstring(s, v.c_str());
	}
	
	inline void stack<std::string>::put(LuaGlueBase *, lua_State *s, std::string *v)
	{
		LG_Debug("str: %s", v->c_str());
		lua_pushstring(s, v->c_str());
	}

	inline std::string stack<std::string &>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		const char *str = luaL_checkstring(s, idx);
		LG_Debug("str: %s", str);
		return str;
	}

	inline void stack<std::string &>::put(LuaGlueBase *, lua_State *s, std::string v)
	{
		lua_pushstring(s, v.c_str());
	}
	
	inline void stack<std::string &>::put(LuaGlueBase *, lua_State *s, std::string *v)
	{
		LG_Debug("str: %s", v->c_str());
		lua_pushstring(s, v->c_str());
	}

	inline std::string stack<const std::string&>::get(LuaGlueBase *, lua_State *s, int idx)
	{
		const char *str = luaL_checkstring(s, idx);
		LG_Debug("str: %s", str);
		return str;
	}
	
	inline void stack<const std::string&>::put(LuaGlueBase *, lua_State *s, std::string v)
	{
		lua_pushstring(s, v.c_str());
	}
	
	inline void stack<const std::string&>::put(LuaGlueBase *, lua_State *s, std::string *v)
	{
		LG_Debug("str: %s", v->c_str());
		lua_pushstring(s, v->c_str());
	}

#endif /* LUAGLUE_STACKTEMPLATES_STRING_H_GUARD */
