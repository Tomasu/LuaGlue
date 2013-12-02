#ifndef LUAGLUE_NEWINDEX_METHOD_H_GUARD
#define LUAGLUE_NEWINDEX_METHOD_H_GUARD

#include <lua.hpp>
#include <string>
#include <tuple>
#include <utility>

#include "LuaGlue/LuaGlueObject.h"
#include "LuaGlue/LuaGlueApplyTuple.h"
#include "LuaGlue/LuaGlueBase.h"

template<typename _Class>
class LuaGlueClass;

template<typename _Value, typename _Class, typename _Key>
class LuaGlueNewIndexMethod : public LuaGlueMethodBase
{
	public:
		typedef _Class ClassType;
		typedef void (_Class::*MethodType)(_Key, _Value);
		
		LuaGlueNewIndexMethod(LuaGlueClass<_Class> *luaClass, const std::string &name, MethodType &&fn) : glueClass(luaClass), name_(name), fn(std::forward<MethodType>(fn))
		{ }
		
		~LuaGlueNewIndexMethod() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
	private:
		LuaGlueClass<_Class> *glueClass;
		std::string name_;
		MethodType fn;
		std::tuple<_Key, _Value> args;
		
		int invoke(lua_State *state)
		{
#ifdef LUAGLUE_TYPECHECK
			LuaGlueObject<ClassType> obj = *(LuaGlueObject<ClassType> *)luaL_checkudata(state, 1, glueClass->name().c_str());
#else
			LuaGlueObject<ClassType> obj = *(LuaGlueObject<ClassType> *)lua_touserdata(state, 1);
#endif
			if(lua_istable(state, 1))
				printf("got table as arg1\n");
			
			lua_remove(state, 1);
			
			applyTuple(glueClass->luaGlue(), state, obj, fn, args);
			lua_pop(state, 2);
			
			return 0;
		}
		
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueNewIndexMethod<_Value, _Class, _Key> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

#endif /* LUAGLUE_NEWINDEX_METHOD_H_GUARD */
