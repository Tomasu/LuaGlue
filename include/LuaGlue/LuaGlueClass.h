#ifndef LUAGLUE_CLASS_H_GUARD
#define LUAGLUE_CLASS_H_GUARD

#include <cassert>
#include <vector>
#include <string>
#include <map>
#include <new>

#include "LuaGlue/LuaGlueObject.h"
#include "LuaGlue/LuaGlueClassBase.h"
#include "LuaGlue/LuaGlueConstant.h"
#include "LuaGlue/LuaGlueMethodBase.h"
#include "LuaGlue/LuaGluePropertyBase.h"
#include "LuaGlue/LuaGlueSymTab.h"

#include "LuaGlue/LuaGlueUtils.h"
#include "LuaGlue/LuaGlueDebug.h"

#include "LuaGlue/LuaGlueBase.h"
#include "LuaGlue/LuaGlueArray.h"

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

#include "LuaGlue/LuaGlueProperty.h"

template<class _Class>
class LuaGlueObject;

template<typename _Class>
class LuaGlueClass : public LuaGlueClassBase
{
	public:
		
		
		typedef _Class ClassType;
		
		LuaGlueClass(LuaGlueBase *luaGlue, const std::string &name) : luaGlue_(luaGlue), name_(name), lg_typeid_(0) 
		{ }
		
		~LuaGlueClass()
		{ 	}
		
		const std::string &name() { return name_; }

		void setLGTypeID(LUA_UNSIGNED lgt) { lg_typeid_ = lgt; }
		
		template<typename _Ret, typename... _Args>
		_Ret invokeMethod(const std::string &name, _Class *obj, _Args... args)
		{
			const unsigned int Arg_Count_ = sizeof...(_Args);
			
			// note, there is no direct support for overridden methods. each method registered with LuaGlue
			//  must have a different "name" when registered.
			
			pushInstance(luaGlue_->state(), obj);
			//printf("attempting to get method %s::%s\n", name_.c_str(), name.c_str());
			lua_getfield(luaGlue_->state(), -1, name.c_str());
			//lua_dump_stack(luaGlue_->state());
		
			lua_pushvalue(luaGlue_->state(), -2);
			applyTupleLuaFunc(luaGlue_, luaGlue_->state(), args...);
			
			//using Alias=char[];
			//Alias{ (returnValue(*luaGlue_, luaGlue_->state(), args), void(), '\0')... };
			
			lua_pcall(luaGlue_->state(), Arg_Count_+1, 1, 0);
			lua_remove(luaGlue_->state(), -2);
			return stack<_Ret>::get(luaGlue_, luaGlue_->state(), -1);
		}
		
		template<typename... _Args>
		void invokeVoidMethod(const std::string &name, _Class *obj, _Args... args)
		{
			const unsigned int Arg_Count_ = sizeof...(_Args);
			
			// note, there is no direct support for overridden methods. each method registered with LuaGlue
			//  must have a different "name" when registered.
			
			pushInstance(luaGlue_->state(), obj);
			//printf("attempting to get method %s::%s\n", name_.c_str(), name.c_str());
			lua_getfield(luaGlue_->state(), -1, name.c_str());
			//lua_dump_stack(luaGlue_->state());
		
			lua_pushvalue(luaGlue_->state(), -2);
			
			applyTupleLuaFunc(luaGlue_, luaGlue_->state(), args...);
			
			//using Alias=char[];
			//Alias{ (returnValue(*luaGlue_, luaGlue_->state(), args), void(), '\0')... };
			
			lua_pcall(luaGlue_->state(), Arg_Count_+1, 0, 0);
			//lua_pop(luaGlue_->state(), 1);
		}
		
		template<typename _Ret>
		_Ret getProperty(const std::string &name, _Class *obj)
		{
			pushInstance(luaGlue_->state(), obj);
			lua_getfield(luaGlue_->state(), -1, name.c_str());
			
			_Ret ret = getPropertyImpl<_Ret>(std::is_class<typename std::remove_pointer<_Ret>::type>());
			lua_pop(luaGlue_->state(), 1);
			
			return ret;
		}
		
		// basic types
		template<typename _Ret>
		_Ret getPropertyImpl(std::false_type)
		{
			_Ret ret = stack<_Ret>::get(luaGlue_, luaGlue_->state(), -1);
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
			_Ret ptr = stack<_Ret>::get(luaGlue_, luaGlue_->state(), -1);
			return ptr;
		}
		
		// static objects
		template<class _Ret>
		_Ret getPropertyImplObject(std::false_type)
		{
			_Ret obj = stack<_Ret>::get(luaGlue_, luaGlue_->state(), -1);
			return obj;
		}
		
		template<typename _Type>
		void setProperty(const std::string &name, _Class *obj, _Type v)
		{
			pushInstance(luaGlue_->state(), obj);
			stack<_Type>::put(luaGlue_, luaGlue_->state(), v);
			lua_setfield(luaGlue_->state(), -2, name.c_str());
			lua_pop(luaGlue_->state(), 1);
		}
		
		LuaGlueClass<_Class> &pushInstance(_Class *obj)
		{
			return pushInstance(luaGlue_->state(), obj);
		}
		
		LuaGlueClass<_Class> &pushInstance(lua_State *state, _Class *obj, bool owner = false)
		{
			assert(obj != nullptr);
			
			LG_Debug("%s pushInstance", CxxDemangle(_Class));
			LuaGlueObject<_Class> *udata = (LuaGlueObject<_Class> *)lua_newuserdata(state, sizeof(LuaGlueObject<_Class>));
			new (udata) LuaGlueObject<_Class>(obj, this, owner); // placement new to initialize object
			
			luaL_getmetatable(state, name_.c_str());
			lua_setmetatable(state, -2);
			
			return *this;
		}
		
		LuaGlueClass<_Class> &pushInstance(lua_State *state, const LuaGlueObject<_Class> &obj)
		{
			//assert(obj.ptr() != nullptr);
			
			LG_Debug("LuaGlueObject<%s> pushInstance", CxxDemangle(_Class));
			LuaGlueObject<_Class> *udata = (LuaGlueObject<_Class> *)lua_newuserdata(state, sizeof(LuaGlueObject<_Class>));
			new (udata) LuaGlueObject<_Class>(obj); // placement new to initialize object
			
			luaL_getmetatable(state, name_.c_str());
			lua_setmetatable(state, -2);
			
			return *this;
		}
		
		LuaGlueClass<_Class> &pushInstance(lua_State *state, std::shared_ptr<_Class> const & obj)
		{
			//assert(obj.get() != nullptr);
		
			LG_Debug("shared_ptr<%s> pushInstance", CxxDemangle(_Class));
			std::shared_ptr<_Class> *ptr_ptr = new std::shared_ptr<_Class>(obj);
			LuaGlueObject<std::shared_ptr<_Class>> *udata = (LuaGlueObject<std::shared_ptr<_Class>> *)lua_newuserdata(state, sizeof(LuaGlueObject<std::shared_ptr<_Class>>));
			new (udata) LuaGlueObject<std::shared_ptr<_Class>>(ptr_ptr, this, true); // placement new to initialize object
			
			luaL_getmetatable(state, name_.c_str());
			lua_setmetatable(state, -2);
			
			return *this;
		}
		
		LuaGlueClass<_Class> &pushInstance(lua_State *state, const LuaGlueObject<std::shared_ptr<_Class>> &obj)
		{
			assert(obj.ptr() != nullptr);
			
			LG_Debug("LuaGlueObject<std::shared_ptr<%s>> pushInstance", CxxDemangle(_Class));
			LuaGlueObject<std::shared_ptr<_Class>> *udata = (LuaGlueObject<std::shared_ptr<_Class>> *)lua_newuserdata(state, sizeof(LuaGlueObject<std::shared_ptr<_Class>>));
			new (udata) LuaGlueObject<std::shared_ptr<_Class>>(obj); // placement new to initialize object
			
			luaL_getmetatable(state, name_.c_str());
			lua_setmetatable(state, -2);
			
			return *this;
		}
		
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
			
			// check to see if the required LuaGlueArray specialization is registered
			// if not, register it.
			if(!luaGlue_->classExists(impl->LUAGLUE_CLASS_NAME))
			{
				// FIXME: calling glue() here ourselves works for now, but later
				//  if/when namespaces are added, or more complex registration is done, this may break.
				//  however, not calling it here would mean it may not ever get registered in some cases.
				auto &ac = ((LuaGlue*)luaGlue_)->Class< LuaGlueStaticArray<_N, _Type> >(impl->LUAGLUE_CLASS_NAME);
				
				LG_Debug("register %s", impl->LUAGLUE_CLASS_NAME);
				LuaGlueStaticArray<_N, _Type>::glue(ac);
					
				// if, and only if the state is set
				if(luaGlue_->state())
				{
					ac.glue(luaGlue_);
				}
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
			auto state = luaGlue_->state();

			lua_getglobal(state, name_.c_str());
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
		
		LuaGlue &end() { return *(LuaGlue*)luaGlue_; }
		LuaGlueBase *luaGlue() { return luaGlue_; }
		
		bool glue(LuaGlueBase *luaGlue)
		{
			lua_createtable(luaGlue->state(), 0, 0);
			//int lib_id = lua_gettop(luaGlue->state());
			
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
			
			int idx = luaGlue->getSymTab().findSym(name_.c_str()).idx;
			//printf("LuaGlueClass::glue: classidx: %i\n", idx);
			lua_pushinteger(luaGlue->state(), idx);
			lua_setfield(luaGlue->state(), meta_id, METATABLE_CLASSIDX_FIELD);
			
			//printf("LuaGlueClass::glue: intclassname: %s\n", typeid(_Class).name());
			lua_pushstring(luaGlue->state(), typeid(_Class).name());
			lua_setfield(luaGlue->state(), meta_id, METATABLE_INTCLASSNAME_FIELD);

			//printf("LuaGlueClass::glue: classname: %s\n", name_.c_str());
			lua_pushstring(luaGlue->state(), name_.c_str());
			lua_setfield(luaGlue->state(), meta_id, METATABLE_CLASSNAME_FIELD);
			
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_typeid, 1);
			lua_setfield(luaGlue->state(), meta_id, METATABLE_TYPEID_FIELD);
			
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_index, 1);
			lua_setfield(luaGlue->state(), meta_id, "__index");
			
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_newindex, 1);
			lua_setfield(luaGlue->state(), meta_id, "__newindex");
			
			lua_pushvalue(luaGlue->state(), -2);
			lua_setfield(luaGlue->state(), meta_id, "__metatable");
			
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_gc, 1);
			lua_setfield(luaGlue->state(), meta_id, "__gc");
			
			lua_pushlightuserdata(luaGlue->state(), this);
			lua_pushcclosure(luaGlue->state(), &lua_meta_concat, 1);
			lua_setfield(luaGlue->state(), meta_id, "__concat");
			
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
			
			//lua_pushvalue(luaGlue->state(), -1);
			lua_setmetatable(luaGlue->state(), -2);
			
			lua_pushvalue(luaGlue->state(), -1);
			lua_setglobal(luaGlue->state(), name_.c_str());
			
			//lua_pop(luaGlue->state(), 2);

			//printf("done.\n");
			return true;
		}
		
		void _impl_dtor(std::shared_ptr<_Class> *)
		{
			LG_Debug("impl shared dtor!");
		}
		
		// LuaGlueObjectImpl dtor?
		void _impl_dtor(_Class *)
		{
			// ???
			LG_Debug("impl dtor!");
			//printf("LuaGlueClass<%s>::_impl_dtor\n", name_.c_str());
			//delete p;
		}
		
	private:
		
		LuaGlueSymTab<LuaGlueConstant *> constants_;
		LuaGlueSymTab<LuaGlueMethodBase *> methods;
		LuaGlueSymTab<LuaGlueMethodBase *> static_methods;
		LuaGlueSymTab<LuaGlueMethodBase *> meta_methods;
		//std::map<std::string, LuaGluePropertyBase *> properties_;
		LuaGlueSymTab<LuaGluePropertyBase *> properties_;
		
		
};



#endif /* LUAGLUE_CLASS_H_GUARD */
