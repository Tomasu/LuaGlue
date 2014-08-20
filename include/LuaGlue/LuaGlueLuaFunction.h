#ifndef LUAGLUE_LUA_FUNCTION_H_GUARD
#define LUAGLUE_LUA_FUNCTION_H_GUARD

class LuaGlueLuaFunction
{
	public:
		LuaGlueLuaFunction() : _ref(LUA_NOREF), _b(nullptr), _s(nullptr) { }
		virtual ~LuaGlueLuaFunction()
		{
			if(_ref >= 0)
				luaL_unref(_s, LUA_REGISTRYINDEX, _ref);
			
			_b = nullptr;
			_s = nullptr;
			_ref = LUA_NOREF;
		}
		
		void load(LuaGlueBase *b, lua_State *s, int idx)
		{
			_s = s;
			_b = b;
			lua_pushvalue(s, idx); // copy item in stack at idx to top of stack
			_ref = luaL_ref(s, LUA_REGISTRYINDEX); // gets ref, and pops top of stack
		}
		
		template<typename _Ret, typename... _Args>
		_Ret invoke(_Args... args)
		{
			lua_rawgeti(_s, LUA_REGISTRYINDEX, _ref);
			applyTupleLuaFunc(_b, _s, args...);
			int ret = lua_pcall(_s, sizeof...(_Args), 1, 0);
			if(ret != LUA_OK)
			{
				const char *err = luaL_checkstring(_s, -1);
				LG_Error("%s\n", err);
				return _Ret();
			}
			
			return stack<_Ret>::get(_b, _s, -1);
		}
		
		template <typename _Ret, typename... _Args>
		_Ret operator() (_Args... args)
		{
			return invoke(std::forward<_Args...>(args...));
		}
		
		template <typename... _Args>
		void invokeMulti(_Args... args)
		{
			lua_rawgeti(_s, LUA_REGISTRYINDEX, _ref);
			applyTupleLuaFunc(_b, _s, args...);
			int ret = lua_pcall(_s, sizeof...(_Args), LUA_MULTRET, 0);
			if(ret != LUA_OK)
			{
				const char *err = luaL_checkstring(_s, -1);
				LG_Error("%s\n", err);
			}
		}
		
		template <typename... _Args>
		void operator() (_Args... args)
		{
			invokeMulti(std::forward<_Args...>(args...));
		}
		
		void put()
		{
			lua_rawgeti(_s, LUA_REGISTRYINDEX, _ref);
		}
		
	private:
		int _ref;
		LuaGlueBase *_b;
		lua_State *_s;
};

// TODO: maybe add type for this? not sure it's needed yet...
//class LuaGlueLuaFunctionType : public LuaGlueType< LuaGlueLuaFunction >
//{
//	
//};

#endif /* LUAGLUE_LUA_FUNCTION_H_GUARD */
