#ifndef LUAGLUE_STACKTEMPLATES_ARRAY_H_GUARD
#define LUAGLUE_STACKTEMPLATES_ARRAY_H_GUARD

// FIXME: turn array into LuaGlueType subclass
#include "LuaGlue/LuaGlueArray.h"

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
		LuaGlueClass<ArrayType> *lgc = (LuaGlueClass<ArrayType> *)g->lookupType(typeid(LuaGlueClass<ArrayType>).name(), true);
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
			LuaGlueTypeBase *lgc = checkGlueType(g, s, idx);
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
		ArrayType *lgc = (ArrayType *)g->lookupType(typeid(ArrayType).name(), true);
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
			LuaGlueTypeBase *lgc = checkGlueType(g, s, idx);
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

#endif /* LUAGLUE_STACKTEMPLATES_ARRAY_H_GUARD */
