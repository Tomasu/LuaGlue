#ifndef LUAGLUE_METHOD_H_GUARD
#define LUAGLUE_METHOD_H_GUARD

#include <lua.hpp>
#include <string>
#include <tuple>
#include <utility>

#include "LuaGlue/LuaGlueMethodBase.h"
#include "LuaGlue/LuaGlueApplyTuple.h"

class LuaGlue;

template<typename _Class>
class LuaGlueClass;

template<typename _Ret, typename _Class, typename... _Args>
class LuaGlueMethod : public LuaGlueMethodBase
{
	
	public:
		typedef _Class ClassType;
		typedef _Ret ReturnType;
		//typedef _Ret (_Class::*MethodType)( _Args... );
		
		LuaGlueMethod(LuaGlueClass<_Class> *luaClass, const std::string &name, _Ret (_Class::*fn)(_Args...)) : glueClass(luaClass), name_(name), fn(std::forward< _Ret (_Class::*)(_Args...)>(fn))
		{ }
		
		~LuaGlueMethod() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlue *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
	private:
		LuaGlueClass<_Class> *glueClass;
		std::string name_;
		_Ret (_Class::*fn)(_Args...);
		std::tuple<_Args...> args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
		
	public:
		int invoke(lua_State *state)
		{
			ClassType *obj = (ClassType *)luaL_checkudata(state, 1, glueClass->name().c_str());
			ReturnType ret = applyTuple(state, (_Class *)obj, fn, args);
			lua_pop(state, Arg_Count_);
			
			returnValue(state, ret);
			return 1;
		}
		
	private:
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueMethod<_Ret, _Class, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

template<typename _Class, typename... _Args>
class LuaGlueMethod<void, _Class, _Args...> : public LuaGlueMethodBase
{
	
	public:
		typedef _Class ClassType;
		
		LuaGlueMethod(LuaGlueClass<_Class> *luaClass, const std::string &name, void (_Class::*fn)(_Args...)) : glueClass(luaClass), name_(name), fn(std::forward< void (_Class::*)(_Args...)>(fn))
		{ }
		
		~LuaGlueMethod() {}
		
		std::string name() { return name_; }
		
		bool glue(LuaGlue *luaGlue)
		{
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_call_func, 1);
			lua_setfield(luaGlue->state(), -2, name_.c_str());
			return true;
		}
		
	private:
		LuaGlueClass<_Class> *glueClass;
		std::string name_;
		void (_Class::*fn)(_Args...);
		std::tuple<_Args...> args;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
	
	public:
		int invoke(lua_State *state)
		{
			ClassType *obj = (ClassType *)luaL_checkudata(state, 1, glueClass->name().c_str());
			applyTuple(state, (_Class *)obj, fn, args);
			lua_pop(state, Arg_Count_);
			return 0;
		}
		
	private:
		static int lua_call_func(lua_State *state)
		{
			auto mimp = (LuaGlueMethod<void, _Class, _Args...> *)lua_touserdata(state, lua_upvalueindex(1));
			return mimp->invoke(state);
		}
};

#endif /* LUAGLUE_METHOD_H_GUARD */
