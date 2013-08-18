#ifndef LUAGLUE_CLASS_H_GUARD
#define LUAGLUE_CLASS_H_GUARD

#include <vector>
#include <string>
#include <map>

#include "LuaGlue/LuaGlueClassBase.h"
#include "LuaGlue/LuaGlueConstant.h"
#include "LuaGlue/LuaGlueMethodBase.h"
#include "LuaGlue/LuaGluePropertyBase.h"
#include "LuaGlue/LuaGlueSymTab.h"

class LuaGlue;

template<typename _Class, typename... _Args>
class LuaGlueCtorMethod;

template<typename _Class>
class LuaGlueDtorMethod;

template<typename _Ret, typename _Class, typename... _Args>
class LuaGlueStaticMethod;

template<typename _Class, typename... _Args>
class LuaGlueStaticMethod<void, _Class, _Args...>;

template<typename _Ret, typename _Class, typename... _Args>
class LuaGlueMethod;

template<typename _Class, typename... _Args>
class LuaGlueMethod<void, _Class, _Args...>;

template<typename _Value, typename _Class, typename _Key>
class LuaGlueIndexMethod;

template<typename _Value, typename _Class, typename _Key>
class LuaGlueNewIndexMethod;

#include "LuaGlueProperty.h"

// TODO: look into associating classes and methods with an index into
//  a lookup table rather than with a lightuserdata to the class itself..
// maybe an unordered_map of typeid(TC).hash_code() for classes ?
template<typename _Class>
class LuaGlueClass : public LuaGlueClassBase
{
	public:
		typedef _Class ClassType;
		
		LuaGlueClass(LuaGlue *luaGlue, const std::string &name) : luaGlue_(luaGlue), name_(name)
		{ }
		
		~LuaGlueClass() { }
		
		const std::string &name() { return name_; }
		
		template<typename _Ret, typename... _Args>
		_Ret invokeLuaMethod(const std::string &name, _Class *obj, _Args... args)
		{
			// TODO: maybe add LuaGlueObject wrapper, so we can create a single userdata
			//  and just pass it around.
		}
		
		LuaGlueClass<_Class> &pushInstance(_Class *obj)
		{
			return pushInstance(luaGlue->state(), obj);
		}
		
		LuaGlueClass<_Class> &pushInstance(lua_State *state, _Class *obj)
		{
			_Class **udata = (_Class **)lua_newuserdata(state, sizeof(_Class *));
			*udata = obj;
			
			luaL_getmetatable(state, name_.c_str());
			lua_setmetatable(state, -2);
			
			return *this;
		}
		
		template<typename... _Args>
		LuaGlueClass<_Class> &ctor(const std::string &name)
		{
			auto impl = new LuaGlueCtorMethod<_Class, _Args...>(this, name.c_str());
			static_methods[name] = impl;
			
			return *this;
		}
		
		LuaGlueClass<_Class> &dtor(void (_Class::*fn)())
		{
			auto impl = new LuaGlueDtorMethod<_Class>(this, "__gc", std::forward<decltype(fn)>(fn));
			meta_methods["__gc"] = impl;
			
			return *this;
		}
		
		template<typename _Value, typename _Key>
		LuaGlueClass<_Class> &index(_Value (_Class::*fn)(_Key))
		{
			auto impl = new LuaGlueIndexMethod<_Value, _Class, _Key>(this, "m__index", std::forward<decltype(fn)>(fn));
			meta_methods["m__index"] = impl;
			
			return *this;
		}
		
		template<typename _Value, typename _Key>
		LuaGlueClass<_Class> &newindex(void (_Class::*fn)(_Key, _Value))
		{
			auto impl = new LuaGlueNewIndexMethod<_Value, _Class, _Key>(this, "m__newindex", std::forward<decltype(fn)>(fn));
			meta_methods["m__newindex"] = impl;
			
			return *this;
		}
		
		template<typename _Type>
		LuaGlueClass<_Class> &property(const std::string &name, _Type _Class::*prop)
		{
			auto impl = new LuaGlueProperty<_Type, _Class>(this, name, prop);
			properties_[name] = impl;
			
			return *this;
		}
		
		//template<typename _Type, typename E = void>
		//LuaGlueClass<_Class> &prop(const std::string &name, _Type _Class::*prop);
		
		template<typename _Type>
		LuaGlueClass<_Class> &prop(const std::string &name, _Type _Class::*prop)
		{
			auto impl = new LuaGlueProperty<_Type, _Class>(this, name, prop);
			properties_[name] = impl;
			
			return *this;
		}
		
		template<typename _Ret, typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, _Ret (_Class::*fn)(_Args...))
		{
			auto impl = new LuaGlueMethod<_Ret, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			methods[name] = impl;
			
			return *this;
		}
		
		template<typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, void (_Class::*fn)(_Args...))
		{
			auto impl = new LuaGlueMethod<void, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			methods[name] = impl;
			
			return *this;
		}
		
		template<typename _Ret, typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, _Ret (*fn)(_Args...))
		{
			auto impl = new LuaGlueStaticMethod<_Ret, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			static_methods[name] = impl;
			
			return *this;
		}
		
		template<typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, void (*fn)(_Args...))
		{
			auto impl = new LuaGlueStaticMethod<void, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			static_methods[name] = impl;
			
			return *this;
		}
		
		template<typename T>
		LuaGlueClass<_Class> &constant(const std::string &name, T v)
		{
			auto impl = new LuaGlueConstant(name, v);
			constants_[name] = impl;
			
			return *this;
		}
		
		LuaGlueClass<_Class> &constants(const std::vector<LuaGlueConstant> &c)
		{
			for(unsigned int i = 0; i < c.size(); i++)
			{
				auto impl = new LuaGlueConstant(c[i]);
				constants_[impl->name()] = impl;
			}
			
			return *this;
		}
		
		LuaGlue &end() { return *luaGlue_; }
		LuaGlue &luaGlue() { return *luaGlue_; }
		
		bool glue(LuaGlue *luaGlue)
		{
			lua_createtable(luaGlue->state(), 0, 0);
			//int lib_id = lua_gettop(luaGlue->state());
			lua_pushvalue(luaGlue->state(), -1);
			lua_setglobal(luaGlue->state(), name_.c_str());
			
			for(auto &method: static_methods)
			{
				//printf("Glue static method: %s::%s\n", name_.c_str(), method.first.c_str());
				if(!method.ptr->glue(luaGlue))
					return false;
			}
			
			for(auto &constant: constants_)
			{
				//printf("Glue constant: %s::%s\n", name_.c_str(), constant.first.c_str());
				if(!constant.ptr->glue(luaGlue))
					return false;
			}
			
			//printf("Glue Class: %s\n", name_.c_str());
			luaL_newmetatable(luaGlue->state(), name_.c_str());
			int meta_id = lua_gettop(luaGlue->state());
			
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_index, 1);
			lua_setfield(luaGlue->state(), meta_id, "__index");
			
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_newindex, 1);
			lua_setfield(luaGlue->state(), meta_id, "__newindex");
			
			lua_pushvalue(luaGlue->state(), -1);
			lua_setfield(luaGlue->state(), meta_id, "__metatable");
			
			for(auto &method: methods)
			{
				//printf("Glue method: %s::%s\n", name_.c_str(), method.first.c_str());
				if(!method.ptr->glue(luaGlue))
					return false;
			}
			
			for(auto &method: meta_methods)
			{
				//printf("Glue method: %s::%s\n", name_.c_str(), method.first.c_str());
				if(!method.ptr->glue(luaGlue))
					return false;
			}
			
			for(auto &prop: properties_)
			{
				//printf("prop: %s: %p\n", prop.name, prop.ptr); 
				if(!prop.ptr->glue(luaGlue))
					return false;
			}
			
			lua_pushvalue(luaGlue->state(), -1);
			lua_setmetatable(luaGlue->state(), -2);
			
			lua_pop(luaGlue->state(), 2);
			//lua_stack_dump(luaGlue->state());
			//printf("done.\n");
			return true;
		}
		
	private:
		LuaGlue *luaGlue_;
		std::string name_;
		
		LuaGlueSymTab<LuaGlueConstant *> constants_;
		LuaGlueSymTab<LuaGlueMethodBase *> methods;
		LuaGlueSymTab<LuaGlueMethodBase *> static_methods;
		LuaGlueSymTab<LuaGlueMethodBase *> meta_methods;
		//std::map<std::string, LuaGluePropertyBase *> properties_;
		LuaGlueSymTab<LuaGluePropertyBase *> properties_;
		
		// symbol lookup metamethod
		// TODO: handle inheritance/multi-inheritance
		
		int index(lua_State *state)
		{
			//printf("index!\n");
			int type = lua_type(state, 2);
			if(type == LUA_TSTRING)
			{
				const char *key = lua_tostring(state, 2);
				//printf("index: got a string: %s\n", key);
				luaL_getmetatable(state, this->name().c_str());
				//lua_pushstring(state, key);
				lua_pushvalue(state, -2);
				lua_rawget(state, -2);
				lua_remove(state, -2);
				
				if(properties_.exists(key))
				{
					//printf("prop!\n");
					lua_pushvalue(state, 1);
					lua_pushvalue(state, 2);
					lua_call(state, 2, 1);
				}
				
			}
			else if(type == LUA_TNUMBER)
			{
				//int idx = lua_tointeger(state, 2);
				//printf("index: got a number: %d\n", idx);
				
				luaL_getmetatable(state, this->name().c_str());
				lua_pushstring(state, "m__index");
				lua_rawget(state, -2); // get m__index method from metatable
				lua_remove(state, -2); // remove metatable
				lua_pushvalue(state, 1); // copy 1st and 2nd stack elements
				lua_pushvalue(state, 2);
				if(lua_isfunction(state, -3)) // if m__index is a function, call it
					lua_call(state, 2, 1); // removes the argument copies
				// should always be a function.. might want to put some debug/trace messages to make sure
			}
			else
			{
				printf("index: unsupported type: %i\n", type);
			}
			
			return 1;
		}
		
		int newindex(lua_State *state)
		{
			//printf("newindex begin!\n");
			int type = lua_type(state, 2);
			if(type == LUA_TSTRING)
			{
				const char *key = lua_tostring(state, 2);
				//printf("newindex: got a string: %s\n", key);
				luaL_getmetatable(state, this->name().c_str());
				//lua_pushstring(state, key);
				lua_pushvalue(state, 2);
				lua_rawget(state, -2);
				lua_remove(state, -2);
				
				if(properties_.exists(key))
				{
					lua_pushvalue(state, 1); // push self
					lua_pushvalue(state, 2); // push key
					lua_pushvalue(state, 3); // push value
				
					lua_call(state, 3, 0);
				}
			}
			else if(type == LUA_TNUMBER)
			{
				//int idx = lua_tointeger(state, 2);
				//printf("newindex: got a number: %d == %s\n", idx, lua_tostring(state, 3));
				
				luaL_getmetatable(state, this->name().c_str());
				lua_pushstring(state, "m__newindex");
				lua_rawget(state, -2);
				lua_remove(state, -2); // remove metatable
				
				lua_pushvalue(state, 1); // push self
				lua_pushvalue(state, 2); // push key
				lua_pushvalue(state, 3); // push value
				if(lua_isfunction(state, -4))
					lua_call(state, 3, 0);
			}
			else
			{
				printf("newindex: unsupported type: %i\n", type);
			}
			
			//printf("newindex end!\n");
			
			return 0;
		}
		
		static int lua_index(lua_State *state)
		{
			auto cimp = (LuaGlueClass<_Class> *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->index(state);
		}
		
		static int lua_newindex(lua_State *state)
		{
			auto cimp = (LuaGlueClass<_Class> *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->newindex(state);
		}
};

#endif /* LUAGLUE_CLASS_H_GUARD */
