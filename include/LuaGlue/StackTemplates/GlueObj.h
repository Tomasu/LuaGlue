#ifndef LUAGLUE_STACKTEMPLATES_GLUEOBJ_H_GUARD
#define LUAGLUE_STACKTEMPLATES_GLUEOBJ_H_GUARD

template<class T>
struct stack<LuaGlueObject<T>&> {
	static T get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<LuaGlueObject<%s>>: lud", CxxDemangle(T));
			return *(LuaGlueObject<T> *)lua_touserdata(s, idx);
		}
		
		//printf("stack<shared_ptr<T>>::get: name:%s\n", typeid(T).name());

		if(checkGlueType(g, s, idx))
		{
			LG_Debug("stack::get<LuaGlueObject<%s>>: mapped", CxxDemangle(T));
			return **(LuaGlueObject<T> *)lua_touserdata(s, idx);
		}

		LG_Debug("stack::get<LuaGlueObject<%s>>: unk", CxxDemangle(T));
		return T(); // TODO: is this a valid thing? I can't imagine this is a good thing.
	}
	
	static void put(LuaGlueBase *g, lua_State *s, const LuaGlueObject<T> &v)
	{
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupType(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			LG_Debug("stack::put<LuaGlueObject<%s>>: mapped", CxxDemangle(T));
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		LG_Debug("stack::put<LuaGlueObject<%s>>: lud", CxxDemangle(T));
		LuaGlueObject<T> *obj = new LuaGlueObject<T>(v, nullptr, true);
		lua_pushlightuserdata(s, obj);
	}
};

#endif /* LUAGLUE_STACKTEMPLATES_GLUEOBJ_H_GUARD */
