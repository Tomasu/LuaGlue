#ifndef LUAGLUE_CLASS_H_GUARD
#define LUAGLUE_CLASS_H_GUARD

#include <cassert>
#include <vector>
#include <string>
#include <map>
#include <new>

#include "LuaGlue/LuaGlueTypeValue.h"
#include "LuaGlue/LuaGlueConstant.h"
#include "LuaGlue/LuaGlueMethodBase.h"
#include "LuaGlue/LuaGluePropertyBase.h"
#include "LuaGlue/LuaGlueSymTab.h"

#include "LuaGlue/LuaGlueUtils.h"
#include "LuaGlue/LuaGlueDebug.h"

#include "LuaGlue/LuaGlueBase.h"

template<typename _Class, typename... _Args>
class LuaGlueCtorMethod;

template<typename _Class>
class LuaGlueDtorMethod;

template<typename _Ret, typename _Class, typename... _Args>
class LuaGlueStaticMethod;

template<typename _Class, typename... _Args>
class LuaGlueStaticMethod<void, _Class, _Args...>;

template<typename _Ret, typename _Class, typename... _Args>
class LuaGlueConstMethod;

template<typename _Class, typename... _Args>
class LuaGlueConstMethod<void, _Class, _Args...>;

template<typename _Ret, typename _Class, typename... _Args>
class LuaGlueMethod;

template<typename _Class, typename... _Args>
class LuaGlueMethod<void, _Class, _Args...>;

template<typename _Value, typename _Class, typename _Key>
class LuaGlueIndexMethod;

template<typename _Value, typename _Class, typename _Key>
class LuaGlueNewIndexMethod;

template<typename _Type, typename _Class>
class LuaGlueDirectProperty;

template<int _N, typename _Type, typename _Class>
class LuaGlueDirectPropertyArray;

template<typename _Type, typename _Class>
class LuaGlueDirectProperty<std::shared_ptr<_Type>, _Class>;

template<typename _Type, typename _Class>
class LuaGlueProperty;

//#include "LuaGlue/LuaGlueProperty.h"

#include "LuaGlue/LuaGlueType.h"

#include "LuaGlue/LuaGlueArray.h"

template<typename _Class>
class LuaGlueClass : public LuaGlueType<_Class>
{
	public:
		typedef _Class ClassType;
		
		LuaGlueClass(LuaGlueBase *luaGlue, const std::string &name) : LuaGlueType<_Class>(luaGlue, name) { }
		
		virtual ~LuaGlueClass() { LG_Debug("dtor"); }
		
		template<typename _Ret, typename... _Args>
		_Ret invokeMethod(const std::string &name, _Class *obj, _Args... args)
		{
			const unsigned int Arg_Count_ = sizeof...(_Args);
			
			// note, there is no direct support for overridden methods. each method registered with LuaGlue
			//  must have a different "name" when registered.
			
			LuaGlueBase *g = this->luaGlue();
			
			this->pushInstance(g->state(), obj);
			//printf("attempting to get method %s::%s\n", name().c_str(), name.c_str());
			lua_getfield(g->state(), -1, name.c_str());
			//lua_dump_stack(g->state());
		
			lua_pushvalue(g->state(), -2);
			applyTupleLuaFunc(g, g->state(), args...);
			
			//using Alias=char[];
			//Alias{ (returnValue(*g, g->state(), args), void(), '\0')... };
			
			lua_pcall(g->state(), Arg_Count_+1, 1, 0);
			lua_remove(g->state(), -2);
			return stack<_Ret>::get(g, g->state(), -1);
		}
		
		template<typename... _Args>
		void invokeVoidMethod(const std::string &n, _Class *obj, _Args... args)
		{
			const unsigned int Arg_Count_ = sizeof...(_Args);
			
			// note, there is no direct support for overridden methods. each method registered with LuaGlue
			//  must have a different "name" when registered.
			
			LuaGlueBase *g = this->luaGlue();
			
			this->pushInstance(g->state(), obj);
			//printf("attempting to get method %s::%s\n", name().c_str(), n.c_str());
			lua_getfield(g->state(), -1, n.c_str());
			//lua_dump_stack(g->state());
		
			lua_pushvalue(g->state(), -2);
			
			applyTupleLuaFunc(g, g->state(), args...);
			
			//using Alias=char[];
			//Alias{ (returnValue(*g, g->state(), args), void(), '\0')... };
			
			lua_pcall(g->state(), Arg_Count_+1, 0, 0);
			//lua_pop(g->state(), 1);
		}
		
		template<typename _Ret>
		_Ret getProperty(const std::string &n, _Class *obj)
		{
			LuaGlueBase *g = this->luaGlue();
			
			this->pushInstance(g->state(), obj);
			lua_getfield(g->state(), -1, n.c_str());
			
			_Ret ret = getPropertyImpl<_Ret>(std::is_class<typename std::remove_pointer<_Ret>::type>());
			lua_pop(g->state(), 1);
			
			return ret;
		}
		
		// basic types
		template<typename _Ret>
		_Ret getPropertyImpl(std::false_type)
		{
			LuaGlueBase *g = this->luaGlue();
			
			_Ret ret = stack<_Ret>::get(g, g->state(), -1);
			return ret;
		}
		
		// objects
		template<class _Ret>
		_Ret getPropertyImpl(std::true_type)
		{
			_Ret ret = getPropertyImplObject<_Ret>(std::is_pointer<_Ret>());
			return ret;
		}
		
		// pointers to objects
		template<class _Ret>
		_Ret getPropertyImplObject(std::true_type)
		{
			LuaGlueBase *g = this->luaGlue();
			
			_Ret ptr = stack<_Ret>::get(g, g->state(), -1);
			return ptr;
		}
		
		// static objects
		template<class _Ret>
		_Ret getPropertyImplObject(std::false_type)
		{
			LuaGlueBase *g = this->luaGlue();
			
			_Ret obj = stack<_Ret>::get(g, g->state(), -1);
			return obj;
		}
		
		template<typename _Type>
		void setProperty(const std::string &n, _Class *obj, _Type v)
		{
			LuaGlueBase *g = this->luaGlue();
			
			this->pushInstance(g->state(), obj);
			stack<_Type>::put(g, g->state(), v);
			lua_setfield(g->state(), -2, n.c_str());
			lua_pop(g->state(), 1);
		}
		
		/*LuaGlueClass<_Class> &pushInstance(_Class *obj)
		{
			LuaGlueBase *g = this->luaGlue();
			
			return pushInstance(g->state(), obj);
		}*/
		
		/*
		LuaGlueClass<_Class> &pushInstance(lua_State *state, _Class *obj, bool owner = false)
		{
			assert(obj != nullptr);
			
			LG_Debug("%s pushInstance", CxxDemangle(_Class));
			LuaGlueTypeValue<_Class> *udata = (LuaGlueTypeValue<_Class> *)lua_newuserdata(state, sizeof(LuaGlueTypeValue<_Class>));
			new (udata) LuaGlueTypeValue<_Class>(obj, this, owner); // placement new to initialize object
			
			luaL_getmetatable(state, this->name().c_str());
			lua_setmetatable(state, -2);
			
			return *this;
		}*/
		
		/*
		LuaGlueClass<_Class> &pushInstance(lua_State *state, const LuaGlueTypeValue<_Class> &obj)
		{
			//assert(obj.ptr() != nullptr);
			
			LG_Debug("LuaGlueTypeValue<%s> pushInstance", CxxDemangle(_Class));
			LuaGlueTypeValue<_Class> *udata = (LuaGlueTypeValue<_Class> *)lua_newuserdata(state, sizeof(LuaGlueTypeValue<_Class>));
			new (udata) LuaGlueTypeValue<_Class>(obj); // placement new to initialize object
			
			luaL_getmetatable(state, this->name().c_str());
			lua_setmetatable(state, -2);
			
			return *this;
		}
		
		LuaGlueClass<_Class> &pushInstance(lua_State *state, std::shared_ptr<_Class> const & obj)
		{
			//assert(obj.get() != nullptr);
		
			LG_Debug("shared_ptr<%s> pushInstance", CxxDemangle(_Class));
			std::shared_ptr<_Class> *ptr_ptr = new std::shared_ptr<_Class>(obj);
			LuaGlueTypeValue<std::shared_ptr<_Class>> *udata = (LuaGlueTypeValue<std::shared_ptr<_Class>> *)lua_newuserdata(state, sizeof(LuaGlueTypeValue<std::shared_ptr<_Class>>));
			new (udata) LuaGlueTypeValue<std::shared_ptr<_Class>>(ptr_ptr, this, true); // placement new to initialize object
			
			luaL_getmetatable(state, this->name().c_str());
			lua_setmetatable(state, -2);
			
			return *this;
		}
		
		LuaGlueClass<_Class> &pushInstance(lua_State *state, const LuaGlueTypeValue<std::shared_ptr<_Class>> &obj)
		{
			assert(obj.ptr() != nullptr);
			
			LG_Debug("LuaGlueTypeValue<std::shared_ptr<%s>> pushInstance", CxxDemangle(_Class));
			LuaGlueTypeValue<std::shared_ptr<_Class>> *udata = (LuaGlueTypeValue<std::shared_ptr<_Class>> *)lua_newuserdata(state, sizeof(LuaGlueTypeValue<std::shared_ptr<_Class>>));
			new (udata) LuaGlueTypeValue<std::shared_ptr<_Class>>(obj); // placement new to initialize object
			
			luaL_getmetatable(state, this->name().c_str());
			lua_setmetatable(state, -2);
			
			return *this;
		}*/
		
		template<typename... _Args>
		LuaGlueClass<_Class> &ctor(const std::string &name)
		{
			//printf("ctor(%s)\n", name.c_str());
			auto impl = new LuaGlueCtorMethod<_Class, _Args...>(this, name.c_str());
			static_methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		LuaGlueClass<_Class> &dtor(void (_Class::*fn)())
		{
			//printf("dtor()\n");
			auto impl = new LuaGlueDtorMethod<_Class>(this, "m__gc", std::forward<decltype(fn)>(fn));
			meta_methods.addSymbol("m__gc", impl);
			
			return *this;
		}
		
		template<typename _Value, typename _Key>
		LuaGlueClass<_Class> &index(_Value (_Class::*fn)(_Key))
		{
			LG_Debug("%s %s::index(%s)", CxxDemangle(_Value), CxxDemangle(_Class), CxxDemangle(_Key));
			auto impl = new LuaGlueIndexMethod<_Value, _Class, _Key>(this, "m__index", std::forward<decltype(fn)>(fn));
			meta_methods.addSymbol("m__index", impl);
			
			return *this;
		}
		
		template<typename _Value, typename _Key>
		LuaGlueClass<_Class> &newindex(void (_Class::*fn)(_Key, _Value))
		{
			//printf("newindex()\n");
			auto impl = new LuaGlueNewIndexMethod<_Value, _Class, _Key>(this, "m__newindex", std::forward<decltype(fn)>(fn));
			meta_methods.addSymbol("m__newindex", impl);
			
			return *this;
		}
		
		template<typename _Type>
		LuaGlueClass<_Class> &property(const std::string &name, _Type _Class::*prop)
		{
			//printf("property(%s)\n", name.c_str());
			auto impl = new LuaGlueDirectProperty<_Type, _Class>(this, name, prop);
			properties_.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		template<typename _Type>
		LuaGlueClass<_Class> &prop(const std::string &name, _Type _Class::*prop)
		{
			//printf("prop(%s)\n", name.c_str());
			auto impl = new LuaGlueDirectProperty<_Type, _Class>(this, name, prop);
			properties_.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		template<int _N, typename _Type>
		LuaGlueClass<_Class> &property(const std::string &name, _Type (_Class::*prop)[_N])
		{
			LG_Debug("prop array!\n");
			auto impl = new LuaGlueDirectPropertyArray<_N, _Type, _Class>(this, name, prop);
			
			LuaGlueBase *g = this->luaGlue();
			
			// check to see if the required LuaGlueArray specialization is registered
			// if not, register it.
			if(!g->typeExists(typeid(LuaGlueStaticArrayType<_N, _Type>).name(), true))
			{
				// FIXME: calling glue() here ourselves works for now, but later
				//  if/when namespaces are added, or more complex registration is done, this may break.
				//  however, not calling it here would mean it may not ever get registered in some cases.
				auto at = new LuaGlueStaticArrayType<_N, _Type>(g);
				((LuaGlue*)g)->addType(at);
				
				LG_Debug("register %s", impl->LUAGLUE_CLASS_NAME);
			}
			properties_.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		template<typename _Type>
		LuaGlueClass<_Class> &property(const std::string &name, _Type (_Class::*getter)(), void (_Class::*setter)(_Type))
		{
			//printf("property(%s)\n", name.c_str());
			auto impl = new LuaGlueProperty<_Type, _Class>(this, name, getter, setter);
			properties_.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		template<typename _Type>
		LuaGlueClass<_Class> &property(const std::string &name, _Type (_Class::*getter)() const, void (_Class::*setter)(_Type))
		{
			//printf("property(%s)\n", name.c_str());
			auto impl = new LuaGlueProperty<_Type, _Class>(this, name, getter, setter);
			properties_.addSymbol(name.c_str(), impl);
			
			return *this;
		}

		template<typename _Type>
		LuaGlueClass<_Class> &property(const std::string &name, _Type (_Class::*getter)())
		{
			//printf("property(%s)\n", name.c_str());
			auto impl = new LuaGlueProperty<_Type, _Class>(this, name, getter);
			properties_.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		template<typename _Type>
		LuaGlueClass<_Class> &prop(const std::string &name, _Type (_Class::*getter)(), void (_Class::*setter)(_Type))
		{
			//printf("property(%s)\n", name.c_str());
			auto impl = new LuaGlueProperty<_Type, _Class>(this, name, getter, setter);
			properties_.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		// begin instance methods
		
		template<typename _Ret, typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, _Ret (_Class::*fn)(_Args...))
		{
			//printf("method(%s)\n", name.c_str());
			auto impl = new LuaGlueMethod<_Ret, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}

		template<typename _Ret, typename _BaseClass, typename... _Args>
		typename std::enable_if<std::is_convertible<_Class, _BaseClass>::value, LuaGlueClass<_Class>&>::type
		method(const std::string &name, _Ret (_BaseClass::*fn)(_Args...))
		{
			//printf("method(%s)\n", name.c_str());
			auto impl = new LuaGlueMethod<_Ret, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		

		template<typename _Ret, typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, _Ret (_Class::*fn)(_Args...) const)
		{
			//printf("method(%s)\n", name.c_str());
			auto impl = new LuaGlueConstMethod<_Ret, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		template<typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, void (_Class::*fn)(_Args...))
		{
			//printf("method(%s)\n", name.c_str());
			auto impl = new LuaGlueMethod<void, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		template<typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, void (_Class::*fn)(_Args...) const)
		{
			//printf("method(%s)\n", name.c_str());
			auto impl = new LuaGlueConstMethod<void, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		// end instance methods
		
		// begin decorator methods.

		template<typename _Ret, typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, _Ret (*fn)(_Class*, _Args...))
		{
			//printf("decorator method(%s)\n", name.c_str());
			auto impl = new LuaGlueStaticMethod<_Ret, _Class, _Class*, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}

		template<typename _Ret, typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, _Ret (*fn)(std::shared_ptr<_Class>, _Args...))
		{
			//printf("decorator method(%s)\n", name.c_str());
			auto impl = new LuaGlueStaticMethod<_Ret, _Class, std::shared_ptr<_Class>, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}

		// end decorator methods
		
		
		// static/class method
		
		template<typename _Ret, typename... _Args>
		LuaGlueClass<_Class> &method(const std::string &name, _Ret (*fn)(_Args...))
		{
			//printf("method(%s)\n", name.c_str());
			auto impl = new LuaGlueStaticMethod<_Ret, _Class, _Args...>(this, name, std::forward<decltype(fn)>(fn));
			static_methods.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		bool hasMethod(const std::string& methodName)
		{
			auto state = this->luaGlue()->state();

			lua_getglobal(state, this->name().c_str());
			if (!lua_istable(state, -1))
			{
				lua_pop(state, 1);

				return false;
			}

			lua_getfield(state, -1, methodName.c_str());

			bool exists = lua_isfunction(state, -1);

			lua_pop(state, 2);

			return exists;
		}
		
		template<typename T>
		LuaGlueClass<_Class> &constant(const std::string &name, T v)
		{
			//printf("constant(%s)\n", name.c_str());
			auto impl = new LuaGlueConstant(name, v);
			constants_.addSymbol(name.c_str(), impl);
			
			return *this;
		}
		
		LuaGlueClass<_Class> &constants(const std::vector<LuaGlueConstant> &c)
		{
			for(unsigned int i = 0; i < c.size(); i++)
			{
				//printf("constant(%s)\n", c[i].name().c_str());
				auto impl = new LuaGlueConstant(c[i]);
				constants_.addSymbol(impl->name().c_str(), impl);
			}
			
			return *this;
		}
		
		LuaGlue &end() { return *(LuaGlue*)this->luaGlue(); }
		//LuaGlueBase *luaGlue() { return luaGlue(); }
		
		virtual bool glue_type_methods(LuaGlueBase *g)
		{
			for(auto &method: static_methods)
			{
				if(!method.ptr->glue(g))
					return false;
			}
			
			return true;
		}
		
		virtual bool glue_type_properties(LuaGlueBase *g)
		{
			for(auto &prop: constants_)
			{
				if(!prop.ptr->glue(g))
					return false;
			}
			
			return true;
		}
		
		virtual bool glue_instance_methods(LuaGlueBase *g)
		{
			for(auto &method: methods)
			{
				if(!method.ptr->glue(g))
					return false;
			}
			
			return true;
		}
		
		virtual bool glue_instance_properties(LuaGlueBase *g)
		{
			for(auto &prop: properties_)
			{
				if(!prop.ptr->glue(g))
					return false;
			}
			
			return true;
		}
		
		virtual bool glue_meta_methods(LuaGlueBase *g)
		{
			for(auto &mmethod: meta_methods)
			{
				if(!mmethod.ptr->glue(g))
					return false;
			}
			return true;
		}
		
		virtual lua_Integer toInteger()
		{
			// FIXME: allow overriding this?
			return 0;
		}
		
		virtual lua_Number toNumber()
		{
			// FIXME: allow overriding this?
			// possibly via a new metamethod hook, like index/newindex, and gc use?
			return 0;
		}
		
		virtual std::string toString()
		{
			std::string ret;
			
			LuaGlueBase *g = this->luaGlue();
			
			lua_State *state = g->state();
			
			int type = lua_type(state, 1);
			if(type == LUA_TUSERDATA)
			{
				LuaGlueTypeValueBase *lg_obj = (LuaGlueTypeValueBase *)lua_touserdata(state, 1);
				_Class *obj = nullptr;
				
				if(lg_obj->isSharedPtr())
				{
					auto o = CastLuaGlueTypeValueShared(_Class, lg_obj);
					obj = o->ptr();
				}
				else
				{
					auto o = CastLuaGlueTypeValue(_Class, lg_obj);
					obj = o->ptr();
				}
				
				char buff[2048];
				sprintf(buff, "%s(%p)", this->name().c_str(), obj);
				
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
		
	private:
		void impl_dtor(std::shared_ptr<_Class> *)
		{
			LG_Debug("class impl shared dtor!");
		}
		
		// LuaGlueTypeValueImpl dtor?
		void impl_dtor(_Class *)
		{
			// ???
			LG_Debug("class impl dtor!");
			//printf("LuaGlueClass<%s>::_impl_dtor\n", name().c_str());
			//delete p;
		}
		
		virtual int mm_index(lua_State *state)
		{
			//printf("index!\n");
			
			int type = lua_type(state, 2);
			if(type == LUA_TSTRING)
			{
				const char *key = lua_tostring(state, 2);
				
				lua_getmetatable(state, 1);
				lua_pushvalue(state, 2); // push key
				
				lua_rawget(state, -2); // get function
				lua_remove(state, -2); // remove metatable
					
				if(properties_.exists(key))
				{
					//printf("prop!\n");
					lua_pushvalue(state, 1); // push args
					lua_pushvalue(state, 2);
					lua_pcall(state, 2, 1, 0); // call function
				}
				
			}
			else if(type == LUA_TNUMBER)
			{
				lua_dump_stack(state);
				LG_Debug("array type?");
				lua_getmetatable(state, 1);
				lua_pushstring(state, "m__index");
				
				lua_rawget(state, -2); // get m__index method from metatable
				lua_remove(state, -2); // remove metatable
				
				if(lua_isfunction(state, -1)) { // if m__index is a function, call it
					lua_pushvalue(state, 1); // copy 1st and 2nd stack elements
					lua_pushvalue(state, 2);
					
					LG_Debug("before pcall");
					lua_pcall(state, 2, 1, 0); // removes the argument copies
				// should always be a function.. might want to put some debug/trace messages to make sure
					
					//lua_dump_stack(state);
				}
				else
				{
					LG_Debug("not a function :( %s", lua_typename(state, -1));
				}
			}
			else
			{
				printf("index: unsupported type: %i\n", type);
			}
			
			return 1;
		}
		
		virtual int mm_newindex(lua_State *state)
		{
			int type = lua_type(state, 2);
			if(type == LUA_TSTRING)
			{
				const char *key = lua_tostring(state, 2);

				lua_getmetatable(state, 1); // get metatable
				
				if(properties_.exists(key))
				{
					lua_pushvalue(state, 2); // push key
					lua_rawget(state, -2); // get field
					lua_remove(state, -2); // remove metatable
				
					lua_pushvalue(state, 1); // push self
					lua_pushvalue(state, 2); // push key
					lua_pushvalue(state, 3); // push value
				
					lua_pcall(state, 3, 0, 0); // call function from field above
					
					//lua_dump_stack(state);
				}
				else
				{
					lua_pushvalue(state, 2); // push key
					lua_pushvalue(state, 3); // push value
					
					//lua_dump_stack(state);
					lua_rawset(state, -3);
					//lua_dump_stack(state);
				}
				
				lua_pop(state, 1);
			}
			else if(type == LUA_TNUMBER)
			{
				lua_getmetatable(state, 1);
				
				lua_pushstring(state, "m__newindex");
				lua_rawget(state, -2); // get method
				lua_remove(state, -2); // remove metatable
				
				if(lua_isfunction(state, -1)) {
					lua_pushvalue(state, 1); // push self
					lua_pushvalue(state, 2); // push idx
					lua_pushvalue(state, 3); // push value
					lua_pcall(state, 3, 0, 0);
					//lua_dump_stack(state);
				}
			}
			else
			{
				printf("newindex: unsupported type: %i\n", type);
			}
			
			//printf("newindex end!\n");
			
			return 0;
		}
		
	private:
		LuaGlueSymTab<LuaGlueConstant *> constants_;
		LuaGlueSymTab<LuaGlueMethodBase *> methods;
		LuaGlueSymTab<LuaGlueMethodBase *> static_methods;
		LuaGlueSymTab<LuaGlueMethodBase *> meta_methods;
		LuaGlueSymTab<LuaGluePropertyBase *> properties_;
		
		
};



#endif /* LUAGLUE_CLASS_H_GUARD */
