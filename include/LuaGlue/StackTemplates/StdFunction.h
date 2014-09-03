#ifndef LUAGLUE_STACKTEMPLATES_STDFUNCTION_H_GUARD
#define LUAGLUE_STACKTEMPLATES_STDFUNCTION_H_GUARD

// FIXME: LuaGlueLuaFuncRef should turn into a LuaGlueType subclass
#include "LuaGlue/LuaGlueLuaFuncRef.h"
#include "LuaGlue/LuaGlueStdFuncWrapper.h"

	template<typename _Ret, typename... _Args>
	inline std::function<_Ret(_Args...)> stack<std::function<_Ret(_Args...)>&>::get(LuaGlueBase *b, lua_State *s, int idx)
	{
		luaL_checktype(s, idx, LUA_TFUNCTION); // must be a function
		
		const char *funcname = lua_getupvalue(s, idx, 1);
		if(funcname) { /* !funcname means no upvalues */
			// lua_getupvalue pushes the upvalue value onto the stack
			auto wrapper = (FuncWrapperType *)lua_touserdata(s, -1);
			if(wrapper)
			{
				FuncWrapperType fw = *wrapper;
				delete wrapper;
				
				lua_pop(s, 1); // remove upvalue value
				return fw;
			}
		}
		
		// got a lua function, create reference to it.
		return LuaGlueLuaFuncRef<_Ret, _Args...>(b, idx);
	}
	
	template<typename _Ret, typename... _Args>
	inline void stack<std::function<_Ret(_Args...)>&>::put(LuaGlueBase *b, lua_State *, std::function<_Ret(_Args...)> _f)
	{
		auto func = new LuaGlueStdFuncWrapper<_Ret, _Args...>(b, _f);
		func->glue(b);
	}

	template<typename _Ret, typename... _Args>
	inline std::function<_Ret(_Args...)> stack<const std::function<_Ret(_Args...)>&>::get(LuaGlueBase *b, lua_State *s, int idx)
	{
		luaL_checktype(s, idx, LUA_TFUNCTION); // must be a function
		
		const char *funcname = lua_getupvalue(s, idx, 1);
		if(funcname) { /* !funcname means no upvalues */
			// lua_getupvalue pushes the upvalue value onto the stack
			auto wrapper = (FuncWrapperType *)lua_touserdata(s, -1);
			if(wrapper)
			{
				FuncWrapperType fw = *wrapper;
				delete wrapper;
				
				lua_pop(s, 1); // remove upvalue value
				return fw;
			}
		}
		
		// got a lua function, create reference to it.
		return LuaGlueLuaFuncRef<_Ret, _Args...>(b, idx);
	}
	
	template<typename _Ret, typename... _Args>
	inline void stack<const std::function<_Ret(_Args...)>&>::put(LuaGlueBase *b, lua_State *s, std::function<_Ret(_Args...)> _f)
	{
		auto func = new LuaGlueStdFuncWrapper<_Ret, _Args...>(b, _f);
		func.glue(b);
	}

	template<typename... _Args>
	inline std::function<void(_Args...)> stack<std::function<void(_Args...)>&>::get(LuaGlueBase *b, lua_State *s, int idx)
	{
		luaL_checktype(s, idx, LUA_TFUNCTION); // must be a function
		
		const char *funcname = lua_getupvalue(s, idx, 1);
		if(funcname) { /* !funcname means no upvalues */
			// lua_getupvalue pushes the upvalue value onto the stack
			auto wrapper = (FuncWrapperType *)lua_touserdata(s, -1);
			if(wrapper)
			{
				FuncWrapperType fw = *wrapper;
				delete wrapper;
				
				lua_pop(s, 1); // remove upvalue value
				return fw;
			}
			
			lua_pop(s, 1); // remove upvalue value
		}
		
		// got a lua function, create reference to it.
		return LuaGlueLuaFuncRef<void, _Args...>(b, idx);
	}
	
	template<typename... _Args>
	inline void stack<std::function<void(_Args...)>&>::put(LuaGlueBase *b, lua_State *s, std::function<void(_Args...)> _f)
	{
		auto func = new LuaGlueStdFuncWrapper<void, _Args...>(b, _f);
		func.glue(b);
	}

	template<typename... _Args>
	inline std::function<void(_Args...)> stack<const std::function<void(_Args...)>&>::get(LuaGlueBase *b, lua_State *s, int idx)
	{
		luaL_checktype(s, idx, LUA_TFUNCTION); // must be a function
	
		const char *funcname = lua_getupvalue(s, idx, 1);
		if(funcname) { /* !funcname means no upvalues */
			// lua_getupvalue pushes the upvalue value onto the stack
			auto wrapper = (FuncWrapperType *)lua_touserdata(s, -1);
			if(wrapper)
			{
				FuncWrapperType fw = *wrapper;
				delete wrapper;
				
				lua_pop(s, 1); // remove upvalue value

				return fw;
			}
			
			lua_pop(s, 1); // remove upvalue value
		}

		// got a lua function, create reference to it.
		return LuaGlueLuaFuncRef<void, _Args...>(b, idx);
	}
	
	template<typename... _Args>
	inline void stack<const std::function<void(_Args...)>&>::put(LuaGlueBase *b, lua_State *s, const std::function<void(_Args...)> &_f)
	{
		auto func = new LuaGlueStdFuncWrapper<void, _Args...>(b, _f);
		func.glue(b);
	}

// end std::function stack handlers

#endif /* LUAGLUE_STACKTEMPLATES_STDFUNCTION_H_GUARD */
