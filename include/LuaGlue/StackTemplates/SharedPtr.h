#ifndef LUAGLUE_STACKTEMPLATES_SHAREDPTR_H_GUARD
#define LUAGLUE_STACKTEMPLATES_SHAREDPTR_H_GUARD

#include "LuaGlue/LuaGlueTypeValue.h"
#include "LuaGlue/LuaGlueClass.h"
	
	template<class T>
	std::shared_ptr<T> stack<std::shared_ptr<T>>::get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{ 
			LG_Debug("stack::get<LuaGlueTypeValue<std::shared_ptr<%s>>>: lud", CxxDemangle(T));
			LuaGlueTypeValueBase *b = (LuaGlueTypeValueBase *)lua_touserdata(s, idx);
			return *(b->ptr<std::shared_ptr<T>>());
		}
		
		//printf("stack<shared_ptr<T>>::get: name:%s\n", typeid(T).name());

		if(checkGlueType(g, s, idx))
		{
			LG_Debug("stack::get<LuaGlueTypeValue<std::shared_ptr<%s>>>: mapped", CxxDemangle(T));
			LuaGlueTypeValueBase *b = (LuaGlueTypeValueBase *)lua_touserdata(s, idx);
			return *(b->ptr<std::shared_ptr<T>>());
		}

		LG_Debug("stack::get<LuaGlueTypeValue<std::shared_ptr<%s>>>: unk", CxxDemangle(T));
		return std::shared_ptr<T>(); // TODO: is this a valid thing? I can't imagine this is a good thing.
	}

	template<typename T>
	void stack<std::shared_ptr<T>>::put(LuaGlueBase *g, lua_State *s, std::shared_ptr<T> v)
	{
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupType(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			LG_Debug("stack::put<LuaGlueTypeValue<std::shared_ptr<%s>>>: mapped", CxxDemangle(T));
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		LG_Debug("stack::put<LuaGlueTypeValue<std::shared_ptr<%s>>>: lud", CxxDemangle(T));
		std::shared_ptr<T> *ptr = new std::shared_ptr<T>(v);
		LuaGlueTypeValue<std::shared_ptr<T>> *obj = new LuaGlueTypeValue<std::shared_ptr<T>>(ptr, nullptr, true);
		lua_pushlightuserdata(s, obj);
	}
	
	template<class T>
	std::shared_ptr<T> stack<std::shared_ptr<T>&>::get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<LuaGlueTypeValue<std::shared_ptr<%s>>>: lud", CxxDemangle(T));
			LuaGlueTypeValue<std::shared_ptr<T>> ptr = *(LuaGlueTypeValue<std::shared_ptr<T>> *)lua_touserdata(s, idx);
			return *ptr;
		}
		
		//printf("stack<shared_ptr<T>>::get: name:%s\n", typeid(T).name());

		if(checkGlueType(g, s, idx))
		{
			LG_Debug("stack::get<LuaGlueTypeValue<std::shared_ptr<%s>>>: mapped", CxxDemangle(T));
			LuaGlueTypeValue<std::shared_ptr<T>> obj = *(LuaGlueTypeValue<std::shared_ptr<T>> *)lua_touserdata(s, idx);
			return *obj;
		}

		LG_Debug("stack::get<LuaGlueTypeValue<std::shared_ptr<%s>>>: unk", CxxDemangle(T));
		return std::shared_ptr<T>(); // TODO: is this a valid thing? I can't imagine this is a good thing.
	}
	
	template<class T>
	void stack<std::shared_ptr<T>&>::put(LuaGlueBase *g, lua_State *s, std::shared_ptr<T> v)
	{
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupType(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			LG_Debug("stack::put<LuaGlueTypeValue<std::shared_ptr<%s>>>: mapped", CxxDemangle(T));
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		LG_Debug("stack::put<LuaGlueTypeValue<std::shared_ptr<%s>>>: lud", CxxDemangle(T));
		std::shared_ptr<T> *ptr = new std::shared_ptr<T>(v);
		LuaGlueTypeValue<std::shared_ptr<T>> *obj = new LuaGlueTypeValue<std::shared_ptr<T>>(ptr, nullptr, true);
		lua_pushlightuserdata(s, obj);
	}

#endif /* LUAGLUE_STACKTEMPLATES_SHAREDPTR_H_GUARD */
