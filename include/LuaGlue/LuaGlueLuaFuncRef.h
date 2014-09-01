#ifndef LUAGLUE_LUAFUNCREF_H_GUARD
#define LUAGLUE_LUAFUNCREF_H_GUARD

//#warning "turn into LuaGlueType subclass..."

//#error "use LuaGlueLuaFunction instead"

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
		
		_Ret invoke()
		{
			
		}
	private:
		void storeRef()
		{
			LG_TypeCheck(g, "LuaGlueLuaFuncRef", LUA_TFUNCTION, -1);
			lua_ref = luaL_ref(g->state(), LUA_REGISTRYINDEX);
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

template<typename _Ret, typename... _Args>
class LuaGlueLuaFunctionType : public LuaGlueType< LuaGlueLuaFuncRef<_Ret, _Args...> >
{
	typedef LuaGlueLuaFuncRef<_Ret, _Args...> ValueType;
	
	public:
		virtual ~LuaGlueLuaFunctionType() { }
		LuaGlueLuaFunctionType(LuaGlueBase *b) : LuaGlueType< ValueType >(b, typeid(decltype(*this)).name()) { }

		virtual std::string toString()
		{
			std::string ret;
			
			LuaGlueBase *g = this->luaGlue();
			lua_State *state = g->state();
			
			int type = lua_type(state, 1);
			if(type == LUA_TUSERDATA)
			{
				ValueType *sa = (ValueType *)lua_touserdata(state, 1);
				
				char buff[2048];
				sprintf(buff, "%s(%p)", this->name().c_str(), sa);
				
				ret = buff;
			}
			else if(type == LUA_TNIL)
			{
				LG_Debug("nil!");
				ret = "nil";
			}
			else
			{
				LG_Debug("type: %s", lua_typename(state, type));
				ret = lua_tostring(state, 1);
			}
			
			return ret;
		}
		
		virtual lua_Integer toInteger()
		{
			return 0;
		}
		
		virtual lua_Number toNumber()
		{
			return 0.0;
		}
		
	protected:
		virtual int mm_call(lua_State *s)
		{
			LuaGlueTypeValue< ValueType > *obj = (LuaGlueTypeValue< ValueType > *)GetLuaUdata(s, 1, typeid(ValueType).name());
			obj->invoke();
			return 1;
		}
	
};

#endif /* LUAGLUE_LUAFUNCREF_H_GUARD */