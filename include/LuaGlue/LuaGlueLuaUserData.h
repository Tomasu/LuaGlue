#ifndef LUAGLUE_LUA_USERDATA_H_GUARD
#define LUAGLUE_LUA_USERDATA_H_GUARD

class LuaGlueTypeValueBase;

class LuaGlueLuaUserData
{
	public:
		LuaGlueLuaUserData() : _ref(LUA_NOREF) { }
		virtual ~LuaGlueLuaUserData() { }
		
		void load(LuaGlueBase *b, lua_State *s, int idx);
		
		void put()
		{
			lua_rawgeti(_s, LUA_REGISTRYINDEX, _ref);
		}
		
		void _dump()
		{
			put();
			const char *str = lua_tostring(_s, -1);
			LG_Debug("%s", str);
			lua_pop(_s, 1);
		}
		
		std::string toString()
		{
			put();
			const char *str = lua_tostring(_s, -1);
			lua_pop(_s, 1);
			return str;
		}
		
		LuaGlueTypeValueBase *ptr() const { return _ptr; }
		
	private:
		int _ref;
		LuaGlueBase *_b;
		lua_State *_s;
		LuaGlueTypeValueBase *_ptr;
};

inline void LuaGlueLuaUserData::load(LuaGlueBase *b, lua_State *s, int idx)
{
	_b = b;
	_s = s;
	
	// should allow us to keep the table around as long as we have wrapped it
	// and pass around the table by reference, rather than always creating
	// and pushing a new table onto the stack
	
	lua_pushvalue(s, idx); // copy item in stack at idx to top of stack
	_ref = luaL_ref(s, LUA_REGISTRYINDEX); // gets ref, and pops top of stack
	
	int ret = luaL_getmetafield(s, idx, LuaGlueTypeBase::METATABLE_TYPENAME_FIELD);
	if(!ret)
	{
		LG_Warn("failed to get metafield for obj at idx %i", idx);
		return;
	}

	std::string name = stack<std::string>::get(b, s, idx);
	lua_pop(s, 1);
	
	auto ptr = GetLuaUdata(s, idx, name.c_str());
	if(!ptr)
		LG_Warn("failed to get udata value for type %s", name.c_str());
	
	_ptr = ptr;
}

#endif /* LUAGLUE_LUA_USERDATA_H_GUARD */
