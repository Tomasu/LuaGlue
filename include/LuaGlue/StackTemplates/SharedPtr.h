#ifndef LUAGLUE_STACKTEMPLATES_SHAREDPTR_H_GUARD
#define LUAGLUE_STACKTEMPLATES_SHAREDPTR_H_GUARD

template<class T>
struct stack<std::shared_ptr<T>> {
	typedef typename std::shared_ptr<T> SType;
	
	static SType get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{ 
			LG_Debug("stack::get<LuaGlueObject<std::shared_ptr<%s>>>: lud", CxxDemangle(T));
			LuaGlueObject<SType> ptr = *(LuaGlueObject<SType> *)lua_touserdata(s, idx);
			return *ptr;
		}
		
		//printf("stack<shared_ptr<T>>::get: name:%s\n", typeid(T).name());
#ifdef LUAGLUE_TYPECHECK
		LuaGlueTypeBase *lgc = checkGlueType(g, s, idx);
		if(lgc)
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<LuaGlueObject<std::shared_ptr<%s>>>: mapped", CxxDemangle(T));
			LuaGlueObject<SType> obj = *(LuaGlueObject<SType> *)lua_touserdata(s, idx);
			return *obj;

#ifdef LUAGLUE_TYPECHECK
		}
#endif

		LG_Debug("stack::get<LuaGlueObject<std::shared_ptr<%s>>>: unk", CxxDemangle(T));
		return SType(); // TODO: is this a valid thing? I can't imagine this is a good thing.
	}
	
	static void put(LuaGlueBase *g, lua_State *s, SType v)
	{
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupType(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			LG_Debug("stack::put<LuaGlueObject<std::shared_ptr<%s>>>: mapped", CxxDemangle(T));
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		LG_Debug("stack::put<LuaGlueObject<std::shared_ptr<%s>>>: lud", CxxDemangle(T));
		SType *ptr = new SType(v);
		LuaGlueObject<SType> *obj = new LuaGlueObject<SType>(ptr, nullptr, true);
		lua_pushlightuserdata(s, obj);
	}
};

template<class T>
struct stack<std::shared_ptr<T>&> {
	typedef typename std::shared_ptr<T> SType;
	
	static SType get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<LuaGlueObject<std::shared_ptr<%s>>>: lud", CxxDemangle(T));
			LuaGlueObject<SType> ptr = *(LuaGlueObject<SType> *)lua_touserdata(s, idx);
			return *ptr;
		}
		
		//printf("stack<shared_ptr<T>>::get: name:%s\n", typeid(T).name());
#ifdef LUAGLUE_TYPECHECK
		LuaGlueTypeBase *lgc = checkGlueType(g, s, idx);
		if(lgc)
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<LuaGlueObject<std::shared_ptr<%s>>>: mapped", CxxDemangle(T));
			LuaGlueObject<SType> obj = *(LuaGlueObject<SType> *)lua_touserdata(s, idx);
			return *obj;

#ifdef LUAGLUE_TYPECHECK
		}
#endif

		LG_Debug("stack::get<LuaGlueObject<std::shared_ptr<%s>>>: unk", CxxDemangle(T));
		return SType(); // TODO: is this a valid thing? I can't imagine this is a good thing.
	}
	
	static void put(LuaGlueBase *g, lua_State *s, SType v)
	{
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupType(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			LG_Debug("stack::put<LuaGlueObject<std::shared_ptr<%s>>>: mapped", CxxDemangle(T));
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		LG_Debug("stack::put<LuaGlueObject<std::shared_ptr<%s>>>: lud", CxxDemangle(T));
		SType *ptr = new SType(v);
		LuaGlueObject<SType> *obj = new LuaGlueObject<SType>(ptr, nullptr, true);
		lua_pushlightuserdata(s, obj);
	}
};

#endif /* LUAGLUE_STACKTEMPLATES_SHAREDPTR_H_GUARD */
