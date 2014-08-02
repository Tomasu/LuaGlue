#ifndef LUAGLUE_STACKTEMPLATES_PTR_H_GUARD
#define LUAGLUE_STACKTEMPLATES_PTR_H_GUARD

template<class T>
struct stack<T *> {
	static T *get(LuaGlueBase *g, lua_State *s, int idx)
	{
		return get(g, s, idx, std::is_arithmetic<T>());
	}
	
	static T *get(LuaGlueBase *, lua_State *s, int idx, std::true_type)
	{
		T *p = (T *)lua_touserdata(s, idx);
		LG_Debug("stack::get<%s *>: lud %p", CxxDemangle(T), p);
		return p;
	}
	
	static T *get(LuaGlueBase *g, lua_State *s, int idx, std::false_type)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<%s *>: lud", CxxDemangle(T));
			// Might want to wrap this in a LuaGlueObject, but not sure we need to.
			// generally only copies of statically allocated types are done that way.
			return (T*)lua_touserdata(s, idx);
		}
		
#ifdef LUAGLUE_TYPECHECK
		if(checkGlueType(g, s, idx))
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<%s *>: mapped", CxxDemangle(T));
			LuaGlueObject<T> *p = (LuaGlueObject<T> *)lua_touserdata(s, idx);
			LG_Debug("p: %p", p);
			LuaGlueObject<T> obj = *p;
			LG_Debug("ptr: %p", obj.ptr());
			return obj.ptr();
#ifdef LUAGLUE_TYPECHECK
		}
#endif
		
		LG_Debug("stack::get<%s *>: unk", CxxDemangle(T));
		return 0;
	}
	
	static void put(LuaGlueBase *g, lua_State *s, T *v)
	{
		put(g, s, v, std::is_arithmetic<T>());
	}
	
	static void put(LuaGlueBase *, lua_State *s, T *v, std::true_type)
	{
		LG_Debug("stack::put<%s *>: lud %p", CxxDemangle(T), v);
		lua_pushlightuserdata(s, v);
	}
	
	static void put(LuaGlueBase *g, lua_State *s, T *v, std::false_type)
	{
		// first look for a class we support

		typedef typename std::remove_pointer<T>::type TC;
		LuaGlueClass<TC> *lgc = (LuaGlueClass<TC> *)g->lookupType(typeid(LuaGlueClass<TC>).name(), true);
		//printf("stack<T*>::put(T): %s %p lgc:%p\n", typeid(LuaGlueClass<T>).name(), v, lgc);
		if(lgc)
		{
			LG_Debug("stack::put<%s *>: mapped %p", CxxDemangle(T), v);
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T*>: lud!\n");
		LG_Debug("stack::put<%s *>: lud", CxxDemangle(T));
		lua_pushlightuserdata(s, v);
	}
};

template<class T>
struct stack<T *&> {
	static T *get(LuaGlueBase *g, lua_State *s, int idx)
	{
		return get(g, s, idx, std::is_arithmetic<T>());
	}
	
	static T *get(LuaGlueBase *, lua_State *s, int idx, std::true_type)
	{
		T *p = (T *)lua_touserdata(s, idx);
		LG_Debug("stack::get<%s *&>: lud %p", CxxDemangle(T), p);
		return p;
	}
	
	static T *get(LuaGlueBase *g, lua_State *s, int idx, std::false_type)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<%s *>: lud", CxxDemangle(T));
			return (T*)lua_touserdata(s, idx);
		}
		
#ifdef LUAGLUE_TYPECHECK
		if(checkGlueType(g, s, idx))
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<%s *>: mapped", CxxDemangle(T));
			LuaGlueObject<T> *p = (LuaGlueObject<T> *)lua_touserdata(s, idx);
			LG_Debug("p: %p", p);
			LuaGlueObject<T> obj = *p;
			LG_Debug("ptr: %p", obj.ptr());
			return obj.ptr();
#ifdef LUAGLUE_TYPECHECK
		}
#endif
		
		LG_Debug("stack::get<%s *>: unk", CxxDemangle(T));
		return 0;
	}
	
	static void put(LuaGlueBase *g, lua_State *s, T *v)
	{
		put(g, s, v, std::is_arithmetic<T>());
	}
	
	static void put(LuaGlueBase *, lua_State *s, T *v, std::true_type)
	{
		LG_Debug("stack::put<%s *&>: lud %p", CxxDemangle(T), v);
		lua_pushlightuserdata(s, v);
	}
	
	static void put(LuaGlueBase *g, lua_State *s, T *v, std::false_type)
	{
		// first look for a class we support

		typedef typename std::remove_pointer<T>::type TC;
		LuaGlueClass<TC> *lgc = (LuaGlueClass<TC> *)g->lookupType(typeid(LuaGlueClass<TC>).name(), true);
		//printf("stack<T*>::put(T): %s %p lgc:%p\n", typeid(LuaGlueClass<T>).name(), v, lgc);
		if(lgc)
		{
			LG_Debug("stack::put<%s *>: mapped %p", CxxDemangle(T), v);
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T*>: lud!\n");
		LG_Debug("stack::put<%s *>: lud", CxxDemangle(T));
		lua_pushlightuserdata(s, v);
	}
};


template<class T>
struct stack<T *const&> {
	static T *get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<%s *>: lud", CxxDemangle(T));
			return (T*)lua_touserdata(s, idx);
		}
		
#ifdef LUAGLUE_TYPECHECK
		if(checkGlueType(g, s, idx))
		{
#else
			(void)g;
#endif
			lua_dump_stack(s);
			LG_Debug("stack::get<%s *>:%i: %s mapped", CxxDemangle(T), idx, lua_typename(s, lua_type(s, idx)));
		
			lua_dump_stack(s);
			LuaGlueObject<T> *p = (LuaGlueObject<T> *)lua_touserdata(s, idx);
			LG_Debug("p: %p", p);
			LuaGlueObject<T> obj = *p;
			LG_Debug("ptr: %p", obj.ptr());
			return obj.ptr();
#ifdef LUAGLUE_TYPECHECK
		}
#endif
		
		LG_Debug("stack::get<%s *>: unk", CxxDemangle(T));
		return 0;
	}
	
	static void put(LuaGlueBase *g, lua_State *s, T *v)
	{
		// first look for a class we support

		typedef typename std::remove_pointer<T>::type TC;
		LuaGlueClass<TC> *lgc = (LuaGlueClass<TC> *)g->lookupType(typeid(LuaGlueClass<TC>).name(), true);
		//printf("stack<T*>::put(T): %s %p lgc:%p\n", typeid(LuaGlueClass<T>).name(), v, lgc);
		if(lgc)
		{
			LG_Debug("stack::put<%s *>: mapped %p", CxxDemangle(T), v);
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T*>: lud!\n");
		LG_Debug("stack::put<%s *>: lud", CxxDemangle(T));
		lua_pushlightuserdata(s, v);
	}
};

#endif /* LUAGLUE_STACKTEMPLATES_PTR_H_GUARD */
