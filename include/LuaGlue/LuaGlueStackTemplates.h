#ifndef LUAGLUE_STACK_TEMPLATES_H_GUARD
#define LUAGLUE_STACK_TEMPLATES_H_GUARD

#include <lua.hpp>
#include <cstdint>
#include <memory.h>

#include "LuaGlue/LuaGlueUtils.h"

class LuaGlueBase;

template<class T>
LuaGlueClass<T> *getGlueClass(LuaGlueBase *g, lua_State *s, int idx)
{
	int ret = luaL_getmetafield(s, idx, LuaGlueClass<T>::METATABLE_CLASSIDX_FIELD);
	if(!ret)
	{
		LG_Warn("typeid:%s", CxxDemangle(LuaGlueClass<T>));
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
#ifdef LUAGLUE_TYPECHECK
		LuaGlueClass<T> *lgc = getGlueClass<T>(g, s, idx);
		if(lgc)
		{
#else
			(void)g;
#endif
			LG_Debug("stack::get<LuaGlueObject<%s>>: mapped", CxxDemangle(T));
			return **(LuaGlueObject<T> *)lua_touserdata(s, idx);

#ifdef LUAGLUE_TYPECHECK
		}
#endif

		LG_Debug("stack::get<LuaGlueObject<%s>>: unk", CxxDemangle(T));
		return T(); // TODO: is this a valid thing? I can't imagine this is a good thing.
	}
	
	static void put(LuaGlueBase *g, lua_State *s, const LuaGlueObject<T> &v)
	{
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupClass(typeid(LuaGlueClass<T>).name(), true);
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
		LuaGlueClass<T> *lgc = getGlueClass<T>(g, s, idx);
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
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupClass(typeid(LuaGlueClass<T>).name(), true);
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
		LuaGlueClass<T> *lgc = getGlueClass<T>(g, s, idx);
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
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g->lookupClass(typeid(LuaGlueClass<T>).name(), true);
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

template<typename _Ret, typename... _Args>
struct stack<std::function<_Ret(_Args...)>&> {
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

template<typename _Ret, typename... _Args>
struct stack<const std::function<_Ret(_Args...)>&> {
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
struct stack<std::function<void(_Args...)>&> {
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

template<typename... _Args>
struct stack<const std::function<void(_Args...)>&> {
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
			LG_Debug("stack::put<%s>: mapped %p", CxxDemangle(ArrayType), v);
			ArrayType *sa = new ArrayType(v);
			lgc->pushInstance(s, sa, true);
			return;
		}
	
		LG_Debug("stack::put<%s>: lud", CxxDemangle(ArrayType));
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
				LG_Debug("stack::get<%s>: mapped", CxxDemangle(ArrayType));
				LuaGlueObject<ArrayType> *p = (LuaGlueObject<ArrayType> *)lua_touserdata(s, idx);
				LG_Debug("p: %p", p);
				LuaGlueObject<ArrayType> obj = *p;
				LG_Debug("ptr: %p", obj.ptr());
				
				return obj.ptr();
#ifdef LUAGLUE_TYPECHECK
			}
#endif
			
			LG_Debug("stack::get<%s>: unk", CxxDemangle(ArrayType));
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
	
	static void put(LuaGlueBase *g, lua_State *s, T v[_N])
	{
		LuaGlueClass<ArrayType> *lgc = (LuaGlueClass<ArrayType> *)g->lookupClass(typeid(LuaGlueClass<ArrayType>).name(), true);
		//printf("stack<T*>::put(T): %s %p lgc:%p\n", typeid(LuaGlueClass<T>).name(), v, lgc);
		if(lgc)
		{
			LG_Debug("stack::put<%s>: mapped %p", CxxDemangle(ArrayType), v);
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
				LG_Debug("stack::get<%s>: mapped", CxxDemangle(ArrayType));
				LuaGlueObject<ArrayType> *p = (LuaGlueObject<ArrayType> *)lua_touserdata(s, idx);
				LG_Debug("p: %p", p);
				LuaGlueObject<ArrayType> obj = *p;
				LG_Debug("ptr: %p", obj.ptr());
				
				return obj.ptr();
#ifdef LUAGLUE_TYPECHECK
			}
#endif
			
			LG_Debug("stack::get<%s>: unk", CxxDemangle(ArrayType));
			return 0;
		}
};

template<>
struct stack<int&> {
	static int get(LuaGlueBase *, lua_State *s, int idx)
	{
		int v = luaL_checkint(s, idx);
		LG_Debug("stack::get<int&>:%i: %i", idx, v);
		return v;
	}
	
	static void put(LuaGlueBase *, lua_State *s, int &v)
	{
		LG_Debug("stack::put<int&>: %i", v);
		lua_pushinteger(s, v);
	}
};

template<>
struct stack<const int&> {
	static int get(LuaGlueBase *, lua_State *s, int idx)
	{
		int v = luaL_checkint(s, idx);
		LG_Debug("stack::get<const int&>:%i: %i", idx, v);
		return v;
	}
	
	static void put(LuaGlueBase *, lua_State *s, int &v)
	{
		LG_Debug("stack::put<const int&>: %i", v);
		lua_pushinteger(s, v);
	}
};

template<>
struct stack<int> {
	static int get(LuaGlueBase *, lua_State *s, int idx)
	{
		int v = luaL_checkint(s, idx);
		LG_Debug("stack::get<int>:%i: %i", idx, v);
		return v;
	}
	
	static void put(LuaGlueBase *, lua_State *s, int &v)
	{
		LG_Debug("stack::put<int>: %i", v);
		lua_pushinteger(s, v);
	}
};

/*
template<>
struct stack<int*&> {
	static int *get(LuaGlueBase *, lua_State *s, int idx)
	{
		LG_Debug("stack::get<int *&>:%i", idx);
		return (int *)luaL_checkinteger(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, int *v)
	{
		lua_pushinteger(s, (lua_Integer)v);
	}
};

template<>
struct stack<int*> {
	static int *get(LuaGlueBase *, lua_State *s, int idx)
	{
		LG_Debug("stack::get<int *>:%i", idx);
		return (int *)luaL_checkinteger(s, idx);
	}
	
	static void put(LuaGlueBase *, lua_State *s, int *v)
	{
		lua_pushinteger(s, (lua_Integer)v);
	}
};*/

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
struct stack<unsigned int&> {
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
struct stack<const unsigned int&> {
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
struct stack<float&> {
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
struct stack<const float&> {
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
struct stack<double&> {
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
struct stack<const double&> {
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
struct stack<bool&> {
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
struct stack<const bool&> {
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
struct stack<char&> {
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
struct stack<const char&> {
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
struct stack<const char *&> {
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
struct stack<const char * const &> {
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

template<>
struct stack<std::string &> {
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

template<>
struct stack<const std::string&> {
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
		typedef typename std::remove_pointer<T>::type TC;
		LuaGlueClass<TC> *lgc = getGlueClass<TC>(g, s, idx);
		if(lgc)
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
		LuaGlueClass<TC> *lgc = (LuaGlueClass<TC> *)g->lookupClass(typeid(LuaGlueClass<TC>).name(), true);
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
		typedef typename std::remove_pointer<T>::type TC;
		LuaGlueClass<TC> *lgc = getGlueClass<TC>(g, s, idx);
		if(lgc)
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
		LuaGlueClass<TC> *lgc = (LuaGlueClass<TC> *)g->lookupClass(typeid(LuaGlueClass<TC>).name(), true);
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
		typedef typename std::remove_pointer<T>::type TC;
		LuaGlueClass<TC> *lgc = getGlueClass<TC>(g, s, idx);
		if(lgc)
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
		LuaGlueClass<TC> *lgc = (LuaGlueClass<TC> *)g->lookupClass(typeid(LuaGlueClass<TC>).name(), true);
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

// FIXME: static objects need fixed again.
// new LuaGlueObject stuff needs a way to "own" a pointer, and know how to delete it.
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
		LuaGlueClass<RRT> *lgc = getGlueClass<RRT>(g, s, idx);
		if(lgc)
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
		LuaGlueClass<RRT> *lgc = (LuaGlueClass<RRT> *)g->lookupClass(typeid(LuaGlueClass<RRT>).name(), true);
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
		LuaGlueClass<RRT> *lgc = (LuaGlueClass<RRT> *)g->lookupClass(typeid(LuaGlueClass<T>).name(), true);
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

#endif /* LUAGLUE_STACK_TEMPLATES_H_GUARD */
