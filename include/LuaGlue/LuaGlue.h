#ifndef LUAGLUE_H_GUARD
#define LUAGLUE_H_GUARD

#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <lua.hpp>
#include <lauxlib.h>
#include <string>
#include <map>
#include <typeinfo>
#include <memory>

#ifndef LUAGLUE_TYPECHECK
#	define LUAGLUE_TYPECHECK 0
#endif

#ifndef LG_DEBUG
#	define LG_DEBUG 0
#endif

#include "LuaGlue/LuaGlueBase.h"
#include "LuaGlue/LuaGlueStackTemplates.h"
#include "LuaGlue/LuaGlueType.h"
#include "LuaGlue/LuaGlueFunctionBase.h"
#include "LuaGlue/LuaGlueSymTab.h"

template<typename _Class>
class LuaGlueClass;
//class LuaGlueTypeBase;

template<typename _Ret, typename... _Args>
class LuaGlueFunction;

class LuaGlue : public LuaGlueBase
{
	public:
		
		LuaGlue(lua_State *s = 0) : state_(s) { }
		~LuaGlue() { LG_Debug("dtor"); if(state_) lua_close(state_); state_ = nullptr; }
		
		LuaGlue &open(lua_State *s) { state_ = s; return *this; }
		LuaGlue &open()
		{
			state_ = luaL_newstate();
			luaL_openlibs(state_);
			return *this;
		}
		
		template<typename _Type, typename std::enable_if<std::is_convertible<_Type, LuaGlueTypeBase*>::value>::type* = nullptr>
		void addType(_Type type)
		{
			typedef typename std::remove_pointer<_Type>::type SType;
			LG_Debug("addType: %s %s", type->name().c_str(), CxxDemangle(SType));
			auto sym = types.addSymbol(type->name().c_str(), type, type->typeId());
			if(state_)
			{
				type->glue(this);
				lua_pop(state_, 1);
			}
		}
		
		template<typename _Class>
		LuaGlueClass<_Class> &Class(const std::string &name)
		{
			//printf("glue.Class(\"%s\")\n", name.c_str());
			auto new_class = new LuaGlueClass<_Class>(this, name);
			auto sym = types.addSymbol(name.c_str(), new_class, new_class->typeId());
			
			return *new_class;
		}
		
		template<typename _Ret, typename... _Args>
		LuaGlue &func(const std::string &name, _Ret (*fn)(_Args...))
		{
			auto new_func = new LuaGlueFunction<_Ret, _Args...>(this, name, fn);
			functions.addSymbol(name.c_str(), new_func);
			return *this;
		}
		
		template<typename _Ret, typename... _Args>
		LuaGlue &func(const std::string &name, const std::function<_Ret(_Args...)> &fn)
		{
			auto new_func = new LuaGlueFunction<_Ret, _Args...>(this, name, fn);
			functions.addSymbol(name.c_str(), new_func);
			return *this;
		}
		
		template<typename... _Args>
		LuaGlue &func(const std::string &name, const std::function<void(_Args...)> &fn)
		{
			auto new_func = new LuaGlueFunction<void, _Args...>(this, name, fn);
			functions.addSymbol(name.c_str(), new_func);
			return *this;
		}
		
		template<typename _Ret, typename... _Args>
		_Ret invokeFunction(const std::string &name, _Args&&... args)
		{
			const unsigned int Arg_Count_ = sizeof...(_Args);
			
			lua_getglobal(state_, name.c_str());
			applyTupleLuaFunc(this, state_, std::forward<_Args>(args)...);
			int ret = lua_pcall(state_, Arg_Count_, 1, 0);
			if(ret != LUA_OK)
			{
				const char *err = luaL_checkstring(state_, -1);
				last_error = std::string(err);
				lua_pop(state_, 1);
				printf("error: %s\n", err);
				return _Ret();
			}
			_Ret r = stack<_Ret>::get(this, state_, -1);
			lua_pop(state_, 1);
			return r;
		}
		
		/*
		template<typename _Ret, typename... _Args>
		_Ret invokeFunction(const std::string &ns_name, const std::string &name, _Args... args)
		{
			const unsigned int Arg_Count_ = sizeof...(_Args);
			
			lua_getglobal(state_, ns_name.c_str());
			lua_pushstring(state_, name.c_str());
			lua_rawget(state_, -2);
			lua_remove(state_, -2);
			
			LG_Debug("before applyTupleLuaFunc");
			applyTupleLuaFunc(this, state_, args...);

			LG_Debug("before pcall");
			lua_pcall(state_, Arg_Count_, 1, 0);
			return stack<_Ret>::get(this, state_, -1);
		}*/
		
		template<typename... _Args>
		void invokeVoidFunction(const std::string &name, _Args&&... args)
		{
			const unsigned int Arg_Count_ = sizeof...(_Args);
			
			LG_Debug("invoke: %s", name.c_str());
			
			lua_getglobal(state_, name.c_str());
			LG_Debug("before applyTupleLuaFunc");
			lua_dump_stack(state_);
			applyTupleLuaFunc(this, state_, std::forward<_Args>(args)...);
			lua_dump_stack(state_);
			LG_Debug("before pcall");
			
			int ret = lua_pcall(state_, Arg_Count_, 0, 0);
			if(ret != LUA_OK)
			{
				const char *err = luaL_checkstring(state_, -1);
				last_error = std::string(err);
				lua_pop(state_, 1);
				printf("error: %s\n", err);
				LG_Debug("after pcall");
				return;
			}
			
			LG_Debug("after pcall");
			//lua_dump_stack(state_);
		}
		
		/*
		template<typename... _Args>
		void invokeVoidFunction(const std::string &ns_name, const std::string &name, _Args&&... args)
		{
			const unsigned int Arg_Count_ = sizeof...(_Args);
			
			lua_getglobal(state_, ns_name.c_str());
			lua_pushstring(state_, name.c_str());
			lua_rawget(state_, -2);
			lua_remove(state_, -2);
			
			applyTupleLuaFunc(this, state_, std::forward<_Args>(args)...);
			int ret = lua_pcall(state_, Arg_Count_, 0, 0);
			if(ret != LUA_OK)
			{
				const char *err = luaL_checkstring(state_, -1);
				printf("error: %s\n", err);
			}
		}*/
		
		bool globalExists(const char *name)
		{
			lua_getglobal(state_, name);
			int type = lua_type(state_, -1);
			lua_pop(state_, 1);
			return type != LUA_TNIL;
		}
		
		template<typename _Type>
		void setGlobal(const char *name, _Type v)
		{
			stack<_Type>::put(this, state_, v);
			lua_setglobal(state_, name);
		}
		
		template<typename _Type>
		_Type getGlobal(const char *name);
		
		lua_State *state() { return state_; }
		
		bool glue()
		{
			//printf("LuaGlue.glue()\n");
			for(auto &c: types)
			{
				if(!c.ptr->glue(this))
					return false;
			}
			
			for(auto &c: functions)
			{
				if(!c.ptr->glue(this))
					return false;
			}
			
			lua_settop(state_, 0);
			return true;
		}
		
		bool doFile(const std::string &path)
		{
			bool success = !luaL_dofile(state_, path.c_str());
			if(!success)
			{
				const char *err = luaL_checkstring(state_, -1);
				last_error = std::string(err);
				lua_pop(state_, 1);
			}
			else
			{
				last_error = std::string("success");
			}
			
			return success;
		}
		
		// loads script into light sandbox of sorts.
		// calls to functions must be prefixed with ns_name.
		bool doFile(const std::string &ns_name, const std::string &path)
		{
			LG_Debug("load script %s: %s", ns_name.c_str(), path.c_str());
			
			int ret = luaL_loadfile(state_, path.c_str());
			if(ret != LUA_OK) // 1
			{
				const char *errstr = ret != 7 ? luaL_checklstring(state_, -1, 0) : "file not found?";
				printf("failed to load %s: err:%i %s\n", ns_name.c_str(), ret, errstr);
				last_error = std::string(errstr);
				lua_pop(state_, 1);
				return false;
			}
			
			LG_Debug("new table");
			lua_newtable(state_); // script env  // 21
			lua_setglobal(state_, ns_name.c_str()); // set global name for new table // 1

			lua_getglobal(state_, ns_name.c_str()); // 21
			lua_newtable(state_); // metatable // 321
			lua_getglobal(state_, "_G"); // 4321
			lua_setfield(state_, -2, "__index"); // set __index in metatable to _G // 321
			lua_setmetatable(state_, -2); // set metatable for script env // 21

			LG_Debug("set env upvalue");
			lua_setupvalue(state_, -2, 1); // set env for state // 21
			
			LG_Debug("run script %s\n", ns_name.c_str());
			
			ret = lua_pcall(state_, 0, LUA_MULTRET, 0);
			if(ret != LUA_OK) // run script // 1
			{
				const char *errstr = lua_tostring(state_, -1);
				if(!errstr)
				{
					errstr = "unknown error";
				}
				
				printf("failed to run script %s: err:%i %s\n", ns_name.c_str(), ret, errstr);
				last_error = std::string(errstr);
				lua_pop(state_, 1);
				return false;
			}

			LG_Debug("end");
			return true;
		}

		
		bool doString(const std::string &script)
		{
			bool success = !luaL_dostring(state_, script.c_str());
			if(!success)
			{
				const char *err = luaL_checkstring(state_, -1);
				last_error = std::string(err);
				lua_pop(state_, 1);
			}
			else
			{
				last_error = std::string("success");
			}
			
			return success;
		}
		
		LuaGlueTypeBase *lookupType(const char *name, bool internal_name = false)
		{
			return types.lookup(name, internal_name);
		}
		
		bool typeExists(const char *name, bool internal_name = false)
		{
			return types.exists(name, internal_name);
		}
		
		//LuaGlueClassBase *lookupClass(const std::string &name);
		LuaGlueTypeBase *lookupType(uint32_t idx)
		{
			return types.lookup(idx);
		}
		
		template<class _Class>
		LuaGlueClass<_Class> *getClass(const char *name)
		{
			return (LuaGlueClass<_Class> *)types.lookup(name, false);
		}
		
		LuaGlueTypeBase *getType(const char *name)
		{
			return types.lookup(name, false);
		}
		
		LuaGlueSymTab<LuaGlueTypeBase *> &getSymTab() { return types; }
		
		const std::string &lastError() { return last_error; }
		
		// actually a private method. no touch
		void setLastError(const std::string &err) { last_error = err; }
		
	private:
		lua_State *state_;
		LuaGlueSymTab<LuaGlueTypeBase *> types;
		LuaGlueSymTab<LuaGlueFunctionBase *> functions;
		
		std::string last_error;
};

template<>
inline int LuaGlue::getGlobal<int>(const char *name)
{
	lua_getglobal(state_, name);
	int ret = luaL_checkint(state_, -1);
	lua_pop(state_, 1);
	return ret;
}

template<>
inline float LuaGlue::getGlobal<float>(const char *name)
{
	lua_getglobal(state_, name);
	float ret = luaL_checknumber(state_, -1);
	lua_pop(state_, 1);
	return ret;
}

template<>
inline double LuaGlue::getGlobal<double>(const char *name)
{
	lua_getglobal(state_, name);
	double ret = luaL_checknumber(state_, -1);
	lua_pop(state_, 1);
	return ret;
}

template<>
inline char LuaGlue::getGlobal<char>(const char *name)
{
	lua_getglobal(state_, name);
	char ret = luaL_checkint(state_, -1);
	lua_pop(state_, 1);
	return ret;
}

template<>
inline const char *LuaGlue::getGlobal<const char *>(const char *name)
{
	lua_getglobal(state_, name);
	const char *ret = luaL_checkstring(state_, -1);
	lua_pop(state_, 1);
	return ret;
}

template<>
inline const std::string LuaGlue::getGlobal<const std::string>(const char *name)
{
	lua_getglobal(state_, name);
	std::string ret = luaL_checkstring(state_, -1);
	lua_pop(state_, 1);
	return ret;
}

template<class _Class>
inline _Class LuaGlue::getGlobal(const char *name)
{
	lua_getglobal(state_, name);
	
	auto ret = stack<_Class>::get(this, state_, -1);
	lua_pop(state_, 1);
	return ret;
}

#include "LuaGlue/LuaGlueClass.h"
#include "LuaGlue/LuaGlueConstant.h"
#include "LuaGlue/LuaGlueCtorMethod.h"
#include "LuaGlue/LuaGlueDtorMethod.h"
#include "LuaGlue/LuaGlueMethod.h"
#include "LuaGlue/LuaGlueFunction.h"
#include "LuaGlue/LuaGlueStaticMethod.h"
#include "LuaGlue/LuaGlueIndexMethod.h"
#include "LuaGlue/LuaGlueNewIndexMethod.h"
#include "LuaGlue/LuaGlueProperty.h"

#include "LuaGlue/StackTemplates/Array.h"
#include "LuaGlue/StackTemplates/Numeric.h"
#include "LuaGlue/StackTemplates/Ptr.h"
#include "LuaGlue/StackTemplates/SharedPtr.h"
#include "LuaGlue/StackTemplates/StaticObj.h"
#include "LuaGlue/StackTemplates/StdFunction.h"
#include "LuaGlue/StackTemplates/String.h"
#include "LuaGlue/StackTemplates/TypeValue.h"
#include "LuaGlue/StackTemplates/LuaValue.h"

#endif /* LUAGLUE_H_GUARD */
