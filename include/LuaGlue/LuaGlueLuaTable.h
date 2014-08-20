#ifndef LUAGLUE_LUA_TABLE_H_GUARD
#define LUAGLUE_LUA_TABLE_H_GUARD

class LuaGlueLuaTable
{
	public:
		LuaGlueLuaTable() : _ref(LUA_NOREF) { }
		virtual ~LuaGlueLuaTable() { }
		
		void load(LuaGlueBase *b, lua_State *s, int idx);
		
		template <typename... Args>
		void set(std::string id, Args... args)  { _stridx_cache[id] = LuaGlueLuaValue(args...); }

		template <typename... Args>
		void set(int32_t id, Args... args)      { _intidx_cache[id] = LuaGlueLuaValue(args...); }
		
		void put()
		{
			lua_rawgeti(_s, LUA_REGISTRYINDEX, _ref);
		}
		
		void update()
		{
			put();
			
			scan_table(-1);
		}
		
		void _dump()
		{
			put();
			lua_dump_table(_s, -1);
			lua_pop(_s, 1);
		}
		
	private:
		int _ref;
		LuaGlueBase *_b;
		lua_State *_s;
		
		std::map<lua_Integer, LuaGlueLuaValue> _intidx_cache;
		std::map<std::string, LuaGlueLuaValue> _stridx_cache;
		
		void scan_table(int idx);
};

inline void LuaGlueLuaTable::load(LuaGlueBase *b, lua_State *s, int idx)
{
	_b = b;
	_s = s;
	
	// should allow us to keep the table around as long as we have wrapped it
	// and pass around the table by reference, rather than always creating
	// and pushing a new table onto the stack
	
	lua_pushvalue(s, idx); // copy item in stack at idx to top of stack
	_ref = luaL_ref(s, LUA_REGISTRYINDEX); // gets ref, and pops top of stack
	
	scan_table(idx);
}

inline void LuaGlueLuaTable::scan_table(int idx)
{
	// scan table
	lua_pushvalue(_s, idx);
	lua_pushnil(_s);

	while(lua_next(_s, -2))
	{
		lua_pushvalue(_s, -2);

		LuaGlueLuaValue key; key.load(_b, _s, -1);
		LuaGlueLuaValue value; value.load(_b, _s, -2);

		lua_pop(_s, 2);

		if(key.isNumber())
			set(key.getInteger(), value);
		else if(key.isString())
			set(key.getString(), value);
		else
			LG_Error("unknown table key type");
	}

	lua_pop(_s, 1);
}

#endif /* LUAGLUE_LUA_TABLE_H_GUARD */
