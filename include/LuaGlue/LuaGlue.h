#ifndef LUAGLUE_H_GUARD
#define LUAGLUE_H_GUARD

#include <lua.hpp>
#include <lauxlib.h>
#include <string>
#include <map>
#include <typeinfo>
#include <memory>

#include "LuaGlue/LuaGlueBase.h"
#include "LuaGlue/LuaGlueClassBase.h"
#include "LuaGlue/LuaGlueFunction.h"
#include "LuaGlue/LuaGlueLuaFuncRef.h"
#include "LuaGlue/LuaGlueSymTab.h"
#include "LuaGlue/LuaGlueUtils.h"

template<typename _Class>
class LuaGlueClass;

class LuaGlue : public LuaGlueBase
{
	public:
		
		LuaGlue(lua_State *s = 0) : state_(s) { }
		~LuaGlue() { if(state_) lua_close(state_); }
		
		LuaGlue &open(lua_State *s) { state_ = s; return *this; }
		LuaGlue &open()
		{
			state_ = luaL_newstate();
			luaL_openlibs(state_);
			return *this;
		}
		
		template<typename _Class>
		LuaGlueClass<_Class> &Class(const std::string &name)
		{
			//printf("glue.Class(\"%s\")\n", name.c_str());
			auto new_class = new LuaGlueClass<_Class>(this, name);
			auto sym = classes.addSymbol(name.c_str(), new_class);
			new_class->setLGTypeID(sym.lg_typeid);
			
			return *new_class;
		}
		
		/*template<typename _Ret, typename... _Args>
		LuaGlue &func(const std::string &name, _Ret (*fn)(_Args...))
		{
			auto new_func = new LuaGlueFunction<_Ret, _Args...>(this, name, fn);
			functions.addSymbol(name.c_str(), new_func);
			return *this;
		}*/
		
		template<typename _Ret, typename... _Args>
		LuaGlue &func(const std::string &name, _Ret (*fn)(_Args&&...))
		{
			auto new_func = new LuaGlueFunction<_Ret, _Args&&...>(this, name, fn);
			functions.addSymbol(name.c_str(), new_func);
			return *this;
		}
		
		template<typename _Ret, typename... _Args>
		_Ret invokeFunction(const std::string &name, _Args&&... args)
		{
			const unsigned int Arg_Count_ = sizeof...(_Args);
			
			lua_getglobal(state_, name.c_str());
			applyTupleLuaFunc(this, state_, std::forward<_Args>(args)...);
			lua_pcall(state_, Arg_Count_, 1, 0);
			return stack<_Ret>::get(this, state_, -1);
		}
		
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
		}
		
		template<typename... _Args>
		void invokeVoidFunction(const std::string &name, _Args&&... args)
		{
			const unsigned int Arg_Count_ = sizeof...(_Args);
			
			lua_getglobal(state_, name.c_str());
			LG_Debug("before applyTupleLuaFunc");
			//lua_dump_stack(state_);
			applyTupleLuaFunc(this, state_, std::forward<_Args>(args)...);

			LG_Debug("before pcall");
			//lua_dump_stack(state_);
			int ret = lua_pcall(state_, Arg_Count_, 0, 0);
			if(ret != LUA_OK)
			{
				const char *err = luaL_checkstring(state_, -1);
				printf("error: %s\n", err);
			}
			
			LG_Debug("after pcall");
		}
		
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
			for(auto &c: classes)
			{
				if(!c.ptr->glue(this))
					return false;
			}
			
			for(auto &c: functions)
			{
				if(!c.ptr->glue(this))
					return false;
			}
			
			return true;
		}
		
		bool doFile(const std::string &path)
		{
			bool success = !luaL_dofile(state_, path.c_str());
			if(!success)
			{
				const char *err = luaL_checkstring(state_, -1);
				last_error = std::string(err);
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
				printf("failed to run script %s: err:%i %s\n", ns_name.c_str(), ret, errstr ? errstr : "unknown error");
				last_error = std::string(errstr);
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
			}
			else
			{
				last_error = std::string("success");
			}
			
			return success;
		}
		
		LuaGlueClassBase *lookupClass(const char *name, bool internal_name = false)
		{
			return classes.lookup(name, internal_name);
		}
		
		bool classExists(const char *name, bool internal_name = false)
		{
			return classes.exists(name, internal_name);
		}
		
		//LuaGlueClassBase *lookupClass(const std::string &name);
		LuaGlueClassBase *lookupClass(uint32_t idx)
		{
			return classes.lookup(idx);
		}
		
		template<class _Class>
		LuaGlueClass<_Class> *getClass(const char *name)
		{
			return (LuaGlueClass<_Class> *)classes.lookup(name, false);
		}
		
		LuaGlueSymTab<LuaGlueClassBase *> &getSymTab() { return classes; }
		
		const std::string &lastError() { return last_error; }
	private:
		lua_State *state_;
		LuaGlueSymTab<LuaGlueClassBase *> classes;
		LuaGlueSymTab<LuaGlueFunctionBase *> functions;
		
		std::string last_error;
};

template<>
inline int LuaGlue::getGlobal<int>(const char *name)
{
	lua_getglobal(state_, name);
	return luaL_checkint(state_, -1);
}

template<>
inline float LuaGlue::getGlobal<float>(const char *name)
{
	lua_getglobal(state_, name);
	return luaL_checknumber(state_, -1);
}

template<>
inline double LuaGlue::getGlobal<double>(const char *name)
{
	lua_getglobal(state_, name);
	return luaL_checknumber(state_, -1);
}

template<>
inline char LuaGlue::getGlobal<char>(const char *name)
{
	lua_getglobal(state_, name);
	return luaL_checkint(state_, -1);
}

template<>
inline const char *LuaGlue::getGlobal<const char *>(const char *name)
{
	lua_getglobal(state_, name);
	return luaL_checkstring(state_, -1);
}

template<>
inline const std::string LuaGlue::getGlobal<const std::string>(const char *name)
{
	lua_getglobal(state_, name);
	return luaL_checkstring(state_, -1);
}

template<class _Class>
inline _Class LuaGlue::getGlobal(const char *name)
{
	lua_getglobal(state_, name);
	
	return stack<_Class>::get(this, state_, -1);
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

#endif /* LUAGLUE_H_GUARD */
