#ifndef LUAGLUE_STACK_TEMPLATES_H_GUARD
#define LUAGLUE_STACK_TEMPLATES_H_GUARD

#include <lua.hpp>
#include <cstdint>

class LuaGlueBase;

template<class T>
LuaGlueClass<T> *getGlueClass(LuaGlueBase *g, lua_State *s, int idx)
{
	int ret = luaL_getmetafield(s, idx, LuaGlueClass<T>::METATABLE_CLASSIDX_FIELD);
	if(!ret)
	{
		LG_Warn("typeid:%s", typeid(LuaGlueClass<T>).name());
		LG_Warn("failed to get metafield for obj at idx %i", idx);
		return 0;
	}
	
	int id = luaL_checkint(s, -1);
	lua_pop(s, 1);
	
	auto c = (LuaGlueClass<T> *)g->lookupClass((uint32_t)id);
	LG_Debug("getGlueClass: METATABLE_CLASSIDX_FIELD: %i name: %s", id, c->name().c_str());
	return c;
}

template<typename T>
struct stack;


/*
template<class T>
struct stack<std::shared_ptr<T>> {
	static std::shared_ptr<T> get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			//printf("stack<shared_ptr<T>>::get: lud!\n");
			LG_Debug("stack::get<shared_ptr<%s>>: lud", typeid(T).name());
			return **(LuaGlueObject<std::shared_ptr<T>> *)lua_touserdata(s, idx);
		}
		
		//printf("stack<shared_ptr<T>>::get: name:%s\n", typeid(T).name());
#ifdef LUAGLUE_TYPECHECK
		LuaGlueClass<T> *lgc = getGlueClass<T>(g, s, idx);
		if(lgc)
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<shared_ptr<%s>>: mapped", typeid(T).name());
			return **(LuaGlueObject<std::shared_ptr<T>> *)lua_touserdata(s, idx);

#ifdef LUAGLUE_TYPECHECK
		}
#endif

		LG_Debug("stack::get<%s>: unk", typeid(T).name());
		//printf("stack::get<shared_ptr<T>>: failed to get a class instance for lua stack value at idx: %i\n", idx);
		return 0; // TODO: is this a valid thing? I can't imagine this is a good thing.
	}
	
	static void put(LuaGlueBase *g, lua_State *s, std::shared_ptr<T> v)
	{
		//printf("stack<T>::put(T)\n");
		
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupClass(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			//printf("stack<shared_ptr<T>>::put: name:%s\n", typeid(T).name());
			LG_Debug("stack::put<shared_ptr<%s>>: mapped", typeid(T).name());
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T>: lud!\n");
		LG_Debug("stack::put<shared_ptr<%s>>: lud", typeid(T).name());
		std::shared_ptr<T> *ptr = new std::shared_ptr<T>(v);
		LuaGlueObject<std::shared_ptr<T>> *obj = new LuaGlueObject<std::shared_ptr<T>>(ptr, nullptr, true);
		lua_pushlightuserdata(s, obj);
	}
	
	static void put(LuaGlueBase *g, lua_State *s, std::shared_ptr<T> *v)
	{
		//printf("stack<T>::put(T)\n");
		
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupClass(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			//printf("stack<shared_ptr<T>>::put: name:%s\n", typeid(T).name());
			LG_Debug("stack::put<shared_ptr<%s>>: mapped", typeid(T).name());
			lgc->pushInstance(s, *v);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T>: lud!\n");
		LG_Debug("stack::put<shared_ptr<%s>>: lud", typeid(T).name());
		std::shared_ptr<T> *ptr = new std::shared_ptr<T>(*v);
		LuaGlueObject<std::shared_ptr<T>> *obj = new LuaGlueObject<std::shared_ptr<T>>(ptr, nullptr, true);
		lua_pushlightuserdata(s, obj);
	}
};
*/

template<class T>
struct stack<LuaGlueObject<T>&> {
	static T get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<LuaGlueObject<%s>>: lud", typeid(T).name());
			return *(LuaGlueObject<T> *)lua_touserdata(s, idx);
		}
		
		//printf("stack<shared_ptr<T>>::get: name:%s\n", typeid(T).name());
#ifdef LUAGLUE_TYPECHECK
		LuaGlueClass<T> *lgc = getGlueClass<T>(g, s, idx);
		if(lgc)
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<LuaGlueObject<%s>>: mapped", typeid(T).name());
			return **(LuaGlueObject<T> *)lua_touserdata(s, idx);

#ifdef LUAGLUE_TYPECHECK
		}
#endif

		LG_Debug("stack::get<LuaGlueObject<%s>>: unk", typeid(T).name());
		return T(); // TODO: is this a valid thing? I can't imagine this is a good thing.
	}
	
	static void put(LuaGlueBase *g, lua_State *s, const LuaGlueObject<T> &v)
	{
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupClass(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			LG_Debug("stack::put<LuaGlueObject<%s>>: mapped", typeid(T).name());
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		LG_Debug("stack::put<LuaGlueObject<%s>>: lud", typeid(T).name());
		LuaGlueObject<T> *obj = new LuaGlueObject<T>(v);
		lua_pushlightuserdata(s, obj);
	}
};

template<typename _Ret, typename... _Args>
struct stack<std::function<_Ret(_Args...)>> {
	static std::function<_Ret(_Args...)> get(LuaGlueBase *b, lua_State *s, int idx)
	{
		luaL_checktype(s, idx, LUA_TFUNCTION); // must be a function
		
		return LuaGlueLuaFuncRef<_Ret, _Args...>(b, idx);
	}
	
	static void put(LuaGlueBase *b, lua_State *s, std::function<_Ret(_Args...)> _f)
	{
		// TODO: see if we need a wrapper class for these so weird LUA_TFUNCTION call chains don't get setup
		//        when passing std::functions back and forth
		
		auto func = [b,_f](lua_State *_s) -> int
		{
			static const unsigned int Arg_Count_ = sizeof...(_Args);
			std::tuple<_Args...> t;
			
			_Ret ret = applyTuple(b, _s, _f, t);
			if(Arg_Count_) lua_pop(_s, (int)Arg_Count_);
			stack<_Ret>::put(b, _s, ret);
			return 1;
		};
		
		lua_pushcfunction(s, func);
	}
};

template<typename... _Args>
struct stack<std::function<void(_Args...)>> {
	static std::function<void(_Args...)> get(LuaGlueBase *b, lua_State *s, int idx)
	{
		luaL_checktype(s, idx, LUA_TFUNCTION); // must be a function
		
		auto v = LuaGlueLuaFuncRef<void, _Args...>(b, idx);
		return v;
	}
	
	static void put(LuaGlueBase *b, lua_State *s, std::function<void(_Args...)> _f)
	{
		// TODO: see if we need a wrapper class for these so weird LUA_TFUNCTION call chains don't get setup
		//        when passing std::functions back and forth
		
		auto func = [b,_f](lua_State *_s)
		{
			static const unsigned int Arg_Count_ = sizeof...(_Args);
			std::tuple<_Args...> t;
			
			applyTuple(b, _s, _f, t);
			if(Arg_Count_) lua_pop(_s, (int)Arg_Count_);
			return 0;
		};
		
		lua_pushcfunction(s, func);
	}
};

template<>
struct stack<int&> {
	static int get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkint(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, int &v)
	{
		lua_pushinteger(s, v);
	}
};

template<>
struct stack<const int&> {
	static int get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkint(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, int &v)
	{
		lua_pushinteger(s, v);
	}
};

template<>
struct stack<int> {
	static int get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkint(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, int &v)
	{
		lua_pushinteger(s, v);
	}
};

template<size_t _N, typename T>
struct stack<T(&)[_N]> {
	typedef LuaGlueStaticArray<_N, T> ArrayType;
	typedef T(&AT)[_N];
	
	static void getInPlace(LuaGlueBase *g, lua_State *s, int idx, T (&dest)[_N])
	{
		ArrayType *sa = getStaticArray(g, s, idx);
		if(sa)
		{
			memcpy(dest, sa->ptr(), sizeof(T) * _N);
		}
	}
	
	static T *get(LuaGlueBase *g, lua_State *s, int idx)
	{
		ArrayType *sa = getStaticArray(g, s, idx);
		if(sa)
			return sa->ptr();
		
		return 0;
	}
	
	static void put(LuaGlueBase *g, lua_State *s, T (&v)[_N])
	{
		LuaGlueClass<ArrayType> *lgc = (LuaGlueClass<ArrayType> *)g->lookupClass(typeid(LuaGlueClass<ArrayType>).name(), true);
		//printf("stack<T*>::put(T): %s %p lgc:%p\n", typeid(LuaGlueClass<T>).name(), v, lgc);
		if(lgc)
		{
			LG_Debug("stack::put<%s>: mapped %p", typeid(ArrayType).name(), v);
			ArrayType *sa = new ArrayType(v);
			lgc->pushInstance(s, sa);
			return;
		}
		
		lua_pushnil(s);
	}
	
	private:
		static ArrayType *getStaticArray(LuaGlueBase *g, lua_State *s, int idx)
		{
#ifdef LUAGLUE_TYPECHECK
			LuaGlueClass<ArrayType> *lgc = getGlueClass<ArrayType>(g, s, idx);
			if(lgc)
			{
#else
				(void)g;
#endif
				LG_Debug("stack::get<%s>: mapped", typeid(ArrayType).name());
				LuaGlueObject<ArrayType> *p = (LuaGlueObject<ArrayType> *)lua_touserdata(s, idx);
				LG_Debug("p: %p", p);
				LuaGlueObject<ArrayType> obj = *p;
				LG_Debug("ptr: %p", obj.ptr());
				
				return obj.ptr();
#ifdef LUAGLUE_TYPECHECK
			}
#endif
			
			LG_Debug("stack::get<%s>: unk", typeid(ArrayType).name());
			return 0;
		}
};

template<size_t _N, typename T>
struct stack<T[_N]> {
	typedef LuaGlueStaticArray<_N, T> ArrayType;
	typedef T(&AT)[_N];
	
	static void getInPlace(LuaGlueBase *g, lua_State *s, int idx, T (&dest)[_N])
	{
		ArrayType *sa = getStaticArray(g, s, idx);
		if(sa)
		{
			memcpy(dest, sa->ptr(), sizeof(T) * _N);
		}
	}
	
	static T *get(LuaGlueBase *g, lua_State *s, int idx)
	{
		ArrayType *sa = getStaticArray(g, s, idx);
		if(sa)
			return sa->ptr();
		
		return 0;
	}
	
	static void put(LuaGlueBase *g, lua_State *s, T (&v)[_N])
	{
		LuaGlueClass<ArrayType> *lgc = (LuaGlueClass<ArrayType> *)g->lookupClass(typeid(LuaGlueClass<ArrayType>).name(), true);
		//printf("stack<T*>::put(T): %s %p lgc:%p\n", typeid(LuaGlueClass<T>).name(), v, lgc);
		if(lgc)
		{
			LG_Debug("stack::put<%s>: mapped %p", typeid(ArrayType).name(), v);
			ArrayType *sa = new ArrayType(v);
			lgc->pushInstance(s, sa);
			return;
		}
		
		lua_pushnil(s);
	}
	
	private:
		static ArrayType *getStaticArray(LuaGlueBase *g, lua_State *s, int idx)
		{
#ifdef LUAGLUE_TYPECHECK
			LuaGlueClass<ArrayType> *lgc = getGlueClass<ArrayType>(g, s, idx);
			if(lgc)
			{
#else
				(void)g;
#endif
				LG_Debug("stack::get<%s>: mapped", typeid(ArrayType).name());
				LuaGlueObject<ArrayType> *p = (LuaGlueObject<ArrayType> *)lua_touserdata(s, idx);
				LG_Debug("p: %p", p);
				LuaGlueObject<ArrayType> obj = *p;
				LG_Debug("ptr: %p", obj.ptr());
				
				return obj.ptr();
#ifdef LUAGLUE_TYPECHECK
			}
#endif
			
			LG_Debug("stack::get<%s>: unk", typeid(ArrayType).name());
			return 0;
		}
};

template<>
struct stack<unsigned int> {
	static unsigned int get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkinteger(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, unsigned int v)
	{
		lua_pushinteger(s, v);
	}
};

template<>
struct stack<float> {
	static float get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, float v)
	{
		lua_pushnumber(s, v);
	}
};

template<>
struct stack<double> {
	static double get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, double v)
	{
		lua_pushnumber(s, v);
	}
};

template<>
struct stack<bool> {
	static bool get(LuaGlueBase *, lua_State *s, int idx)
	{
		return lua_toboolean(s, idx) ? true : false;
	}
	
	static void put(LuaGlueBase *, lua_State *s, bool v)
	{
		lua_pushboolean(s, v);
	}
};

template<>
struct stack<char> {
	static char get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkinteger(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, char v)
	{
		lua_pushinteger(s, v);
	}
};

template<>
struct stack<const char *> {
	static const char *get(LuaGlueBase *, lua_State *s, int idx)
	{
		return luaL_checkstring(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}
};

template<>
struct stack<std::string> {
	static std::string get(LuaGlueBase *, lua_State *s, int idx)
	{
		const char *str = luaL_checkstring(s, idx);
		LG_Debug("str: %s", str);
		return str;
	}
	
	static void put(LuaGlueBase *, lua_State *s, std::string v)
	{
		lua_pushstring(s, v.c_str());
	}
	
	static void put(LuaGlueBase *, lua_State *s, std::string *v)
	{
		LG_Debug("str: %s", v->c_str());
		lua_pushstring(s, v->c_str());
	}
};

template<class T>
struct stack<T *> {
	static T *get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<%s *>: lud", typeid(T).name());
			return (T*)lua_touserdata(s, idx);
		}
		
#ifdef LUAGLUE_TYPECHECK
		typedef typename std::remove_pointer<T>::type TC;
		LuaGlueClass<TC> *lgc = getGlueClass<TC>(g, s, idx);
		if(lgc)
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<%s *>: mapped", typeid(T).name());
			LuaGlueObject<T> *p = (LuaGlueObject<T> *)lua_touserdata(s, idx);
			LG_Debug("p: %p", p);
			LuaGlueObject<T> obj = *p;
			LG_Debug("ptr: %p", obj.ptr());
			return obj.ptr();
#ifdef LUAGLUE_TYPECHECK
		}
#endif
		
		LG_Debug("stack::get<%s *>: unk", typeid(T).name());
		return 0;
	}
	
	static void put(LuaGlueBase *g, lua_State *s, T *v)
	{
		// first look for a class we support

		typedef typename std::remove_pointer<T>::type TC;
		LuaGlueClass<TC> *lgc = (LuaGlueClass<TC> *)g->lookupClass(typeid(LuaGlueClass<TC>).name(), true);
		//printf("stack<T*>::put(T): %s %p lgc:%p\n", typeid(LuaGlueClass<T>).name(), v, lgc);
		if(lgc)
		{
			LG_Debug("stack::put<%s *>: mapped %p", typeid(T).name(), v);
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T*>: lud!\n");
		LG_Debug("stack::put<%s *>: lud", typeid(T).name());
		lua_pushlightuserdata(s, v);
	}
};

template<class T>
struct stack<T *const&> {
	static T *get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<%s *>: lud", typeid(T).name());
			return (T*)lua_touserdata(s, idx);
		}
		
#ifdef LUAGLUE_TYPECHECK
		typedef typename std::remove_pointer<T>::type TC;
		LuaGlueClass<TC> *lgc = getGlueClass<TC>(g, s, idx);
		if(lgc)
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<%s *>: mapped", typeid(T).name());
			LuaGlueObject<T> *p = (LuaGlueObject<T> *)lua_touserdata(s, idx);
			LG_Debug("p: %p", p);
			LuaGlueObject<T> obj = *p;
			LG_Debug("ptr: %p", obj.ptr());
			return obj.ptr();
#ifdef LUAGLUE_TYPECHECK
		}
#endif
		
		LG_Debug("stack::get<%s *>: unk", typeid(T).name());
		return 0;
	}
	
	static void put(LuaGlueBase *g, lua_State *s, T *v)
	{
		// first look for a class we support

		typedef typename std::remove_pointer<T>::type TC;
		LuaGlueClass<TC> *lgc = (LuaGlueClass<TC> *)g->lookupClass(typeid(LuaGlueClass<TC>).name(), true);
		//printf("stack<T*>::put(T): %s %p lgc:%p\n", typeid(LuaGlueClass<T>).name(), v, lgc);
		if(lgc)
		{
			LG_Debug("stack::put<%s *>: mapped %p", typeid(T).name(), v);
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T*>: lud!\n");
		LG_Debug("stack::put<%s *>: lud", typeid(T).name());
		lua_pushlightuserdata(s, v);
	}
};

// FIXME: static objects need fixed again.
// new LuaGlueObject stuff needs a way to "own" a pointer, and know how to delete it.
template<class T>
struct stack {
	static T get(LuaGlueBase *g, lua_State *s, int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			LG_Debug("stack::get<static %s>: lud", typeid(T).name());
			return *(T*)lua_touserdata(s, idx);
		}
		
#ifdef LUAGLUE_TYPECHECK
		LuaGlueClass<T> *lgc = getGlueClass<T>(g, s, idx);
		if(lgc)
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<static %s>: mapped", typeid(T).name());
			LuaGlueObject<T> obj = *(LuaGlueObject<T> *)lua_touserdata(s, idx);
			return *obj;
#ifdef LUAGLUE_TYPECHECK
		}
#endif

		LG_Debug("stack::get<static %s>: failed to get a class instance for lua stack value at idx: %i", typeid(T).name(), idx);
		return T();
	}
	
	static void put(LuaGlueBase *g, lua_State *s, T v)
	{
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupClass(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			LG_Debug("stack::put<static1 %s>: mapped", typeid(T).name());
			lgc->pushInstance(s, new T(v), true);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T>: lud!\n");
		
		LG_Debug("stack::put<static1 %s>: lud", typeid(T).name());
		LuaGlueObject<T> *obj = new LuaGlueObject<T>(new T(v), 0, true);
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
	/*
	static void put(LuaGlueBase *g, lua_State *s, T *v)
	{
		//printf("stack<T>::put(T*)\n");
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupClass(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			LG_Debug("stack::put<static2 %s>: mapped", typeid(T).name());
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T>: lud!\n");
		LG_Debug("stack::put<static2 %s>: lud", typeid(T).name());
		LuaGlueObject<T> *obj = new LuaGlueObject<T>(new T(*v), 0, true);
		lua_pushlightuserdata(s, obj);
	}*/
};

#endif /* LUAGLUE_STACK_TEMPLATES_H_GUARD */
