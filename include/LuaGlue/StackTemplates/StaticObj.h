#ifndef LUAGLUE_STACKTEMPLATES_STATICOBJ_H_GUARD
#define LUAGLUE_STACKTEMPLATES_STATICOBJ_H_GUARD

// FIXME: static objects need fixed again.
// FIXME: does it really?

template<class T>
struct stack {
	typedef typename std::remove_reference<T>::type RRT;
	
	static RRT get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<static %s>: lud", CxxDemangle(RRT));
			LuaGlueObject<RRT> obj = *(LuaGlueObject<RRT> *)lua_touserdata(s, idx);
			return *obj;
		}
		
#ifdef LUAGLUE_TYPECHECK
		if(checkGlueType(g, s, idx))
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<static %s>: mapped", CxxDemangle(RRT));
			LuaGlueObject<RRT> *ptr = (LuaGlueObject<RRT> *)lua_touserdata(s, idx);
			if(!ptr)
				LG_Debug("no glue object found?");
			LuaGlueObject<RRT> obj = *ptr;
			return *obj;
#ifdef LUAGLUE_TYPECHECK
		}
#endif

		LG_Debug("stack::get<static %s>: failed to get a class instance for lua stack value at idx: %i", CxxDemangle(RRT), idx);
		return RRT();
	}
	
	// think this is borked.
	static void put(LuaGlueBase *g, lua_State *s, const RRT &v)
	{
		LuaGlueClass<RRT> *lgc = (LuaGlueClass<RRT> *)g->lookupType(typeid(LuaGlueClass<RRT>).name(), true);
		if(lgc)
		{
			LG_Debug("stack::put<static1 %s>: mapped", CxxDemangle(RRT));
			lgc->pushInstance(s, new RRT(v), true); // we're owner
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<RRT>: lud!\n");
		
		LG_Debug("stack::put<static1 %s>: lud", CxxDemangle(RRT));
		LuaGlueObject<RRT> *obj = new LuaGlueObject<RRT>(new RRT(v), 0, true);
		lua_pushlightuserdata(s, obj);
	}
	
	// dumb test to see if we can match T *& types
	// even if this works, a separate stack<T> would be better
	//  or fixing the reference bit in the tuple code
	//static void put(LuaGlueBase *g, lua_State *s, T *&v)
	//{
	//	
	//}
	
	// for putting static types
	
	static void put(LuaGlueBase *g, lua_State *s, RRT *v)
	{
		//printf("stack<RRT>::put(RRT*)\n");
		LuaGlueClass<RRT> *lgc = (LuaGlueClass<RRT> *)g->lookupType(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			LG_Debug("stack::put<static2 %s>: mapped", CxxDemangle(T));
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<RRT>: lud!\n");
		LG_Debug("stack::put<static2 %s>: lud", CxxDemangle(RRT));
		LuaGlueObject<RRT> *obj = new LuaGlueObject<RRT>(new RRT(*v), 0, true);
		lua_pushlightuserdata(s, obj);
	}
};

#endif /* LUAGLUE_STACKTEMPLATES_STATICOBJ_H_GUARD */
