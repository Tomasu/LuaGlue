#ifndef LUAGLUE_STACKTEMPLATES_TYPEVALUE_H_GUARD
#define LUAGLUE_STACKTEMPLATES_TYPEVALUE_H_GUARD

template<typename _Class>
class LuaGlueClass;

	template<typename T>
	inline T stack<LuaGlueTypeValue<T>&>::get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<LuaGlueTypeValue<%s>>: lud", CxxDemangle(T));
			return *(LuaGlueTypeValue<T> *)lua_touserdata(s, idx);
		}
		
		//printf("stack<shared_ptr<T>>::get: name:%s\n", typeid(T).name());

		if(checkGlueType(g, s, idx))
		{
			LG_Debug("stack::get<LuaGlueTypeValue<%s>>: mapped", CxxDemangle(T));
			return **(LuaGlueTypeValue<T> *)lua_touserdata(s, idx);
		}

		LG_Debug("stack::get<LuaGlueTypeValue<%s>>: unk", CxxDemangle(T));
		return T(); // TODO: is this a valid thing? I can't imagine this is a good thing.
	}
	
	template<typename T>
	inline void stack<LuaGlueTypeValue<T>&>::put(LuaGlueBase *g, lua_State *s, const LuaGlueTypeValue<T> &v)
	{
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupType(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			LG_Debug("stack::put<LuaGlueTypeValue<%s>>: mapped", CxxDemangle(T));
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		LG_Debug("stack::put<LuaGlueTypeValue<%s>>: lud", CxxDemangle(T));
		LuaGlueTypeValue<T> *obj = new LuaGlueTypeValue<T>(v, nullptr, true);
		lua_pushlightuserdata(s, obj);
	}

#endif /* LUAGLUE_STACKTEMPLATES_TYPEVALUE_H_GUARD */
