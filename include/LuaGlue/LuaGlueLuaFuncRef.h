#ifndef LUAGLUE_LUAFUNCREF_H_GUARD
#define LUAGLUE_LUAFUNCREF_H_GUARD

//#warning "turn into LuaGlueType subclass..."

#include <lua.hpp>
#include <lauxlib.h>

#include "LuaGlue/LuaGlueDebug.h"
#include "LuaGlue/LuaGlueStackTemplates.h"

class LuaGlueBase;

template<typename _Ret, typename... _Args>
class LuaGlueLuaFuncRef
{
	const static unsigned int Arg_Count_ = sizeof...(_Args);
	
	public:
		LuaGlueLuaFuncRef(LuaGlueBase *g_, std::string &name) : g(g_)
		{
			lua_getglobal(g->state(), name.c_str());
			storeRef();
		}
		
		LuaGlueLuaFuncRef(LuaGlueBase *g_, int idx) : g(g_)
		{
			lua_pushvalue(g->state(), idx);
			storeRef();
		}
		
		// create a new reference to stored function
		LuaGlueLuaFuncRef(const LuaGlueLuaFuncRef<_Ret, _Args...> &o) : g(o.g)
		{
			lua_rawgeti(g->state(), LUA_REGISTRYINDEX, o.lua_ref);
			storeRef();
		}
		
		~LuaGlueLuaFuncRef()
		{
			luaL_unref(g->state(), LUA_REGISTRYINDEX, lua_ref);
		}
		
		// create a new reference to stored function
		LuaGlueLuaFuncRef &operator=( const LuaGlueLuaFuncRef &o )
		{
			g = o.g;
			lua_rawgeti(g->state(), LUA_REGISTRYINDEX, o.lua_ref);
			storeRef();
		}
		
		_Ret operator()(_Args... args)
		{
			return invoke(args...);
		}
	
		_Ret invoke(_Args... args)
		{
			return invokeImpl(std::is_void<_Ret>(), args...);
		}
		
	private:
		void storeRef()
		{
			LG_TypeCheck(g, "LuaGlueLuaFuncRef", LUA_TFUNCTION, -1);
			lua_ref = luaL_ref(g->state(), LUA_REGISTRYINDEX);
			LG_Debug("new ref: %i", lua_ref);
		}
		
		_Ret invokeImpl(std::false_type, _Args... args)
		{
			lua_rawgeti(g->state(), LUA_REGISTRYINDEX, lua_ref);
			applyTupleLuaFunc(g, g->state(), args...);
			lua_call(g->state(), Arg_Count_, 1);
 
			return stack<_Ret>::get(g, g->state(), -1);
		}
		
		void invokeImpl(std::true_type, _Args... args)
		{
			lua_rawgeti(g->state(), LUA_REGISTRYINDEX, lua_ref);
			applyTupleLuaFunc(g, g->state(), args...);
			lua_call(g->state(), Arg_Count_, 0);
		}
		
		LuaGlueBase *g;
		int lua_ref;
};

#endif /* LUAGLUE_LUAFUNCREF_H_GUARD */