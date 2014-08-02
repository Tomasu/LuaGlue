#ifndef LUAGLUE_STACKTEMPLATES_STRING_H_GUARD
#define LUAGLUE_STACKTEMPLATES_STRING_H_GUARD

template<>
struct stack<const char *&> {
	static const char *get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}
};

template<>
struct stack<const char * const &> {
	static const char *get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}
};

template<>
struct stack<const char *> {
	static const char *get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}
};

template<size_t N>
struct stack<const char (&)[N]> {
	static const char *get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}
};

template<>
struct stack<std::string> {
	static std::string get(LuaGlueBase *, lua_State *s, int idx)
	{
		const char *str = luaL_checkstring(s, idx);
		LG_Debug("str: %s", str);
		return str;
	}
	
	static void put(LuaGlueBase *, lua_State *s, std::string v)
	{
		lua_pushstring(s, v.c_str());
	}
	
	static void put(LuaGlueBase *, lua_State *s, std::string *v)
	{
		LG_Debug("str: %s", v->c_str());
		lua_pushstring(s, v->c_str());
	}
};

template<>
struct stack<std::string &> {
	static std::string get(LuaGlueBase *, lua_State *s, int idx)
	{
		const char *str = luaL_checkstring(s, idx);
		LG_Debug("str: %s", str);
		return str;
	}
	
	static void put(LuaGlueBase *, lua_State *s, std::string v)
	{
		lua_pushstring(s, v.c_str());
	}
	
	static void put(LuaGlueBase *, lua_State *s, std::string *v)
	{
		LG_Debug("str: %s", v->c_str());
		lua_pushstring(s, v->c_str());
	}
};

template<>
struct stack<const std::string&> {
	static std::string get(LuaGlueBase *, lua_State *s, int idx)
	{
		const char *str = luaL_checkstring(s, idx);
		LG_Debug("str: %s", str);
		return str;
	}
	
	static void put(LuaGlueBase *, lua_State *s, std::string v)
	{
		lua_pushstring(s, v.c_str());
	}
	
	static void put(LuaGlueBase *, lua_State *s, std::string *v)
	{
		LG_Debug("str: %s", v->c_str());
		lua_pushstring(s, v->c_str());
	}
};

#endif /* LUAGLUE_STACKTEMPLATES_STRING_H_GUARD */
