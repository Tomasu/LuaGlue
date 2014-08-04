#ifndef LUAGLUE_STACKTEMPLATES_ARRAY_H_GUARD
#define LUAGLUE_STACKTEMPLATES_ARRAY_H_GUARD

// FIXME: turn array into LuaGlueType subclass
//#include "LuaGlue/LuaGlueArray.h"

#include "LuaGlue/LuaGlueUtils.h"

template<int N, typename T>
class LuaGlueStaticArray;

template<int N, typename T>
class LuaGlueStaticArrayType;

	template<size_t _N, typename T>
	void stack<T(&)[_N]>::getInPlace(LuaGlueBase *g, lua_State *s, int idx, T (&dest)[_N])
	{
		LuaGlueStaticArray<_N, T> *sa = getStaticArray(g, s, idx);
		if(sa)
		{
			memcpy(dest, sa->ptr(), sizeof(T) * _N);
		}
	}
	
	template<size_t _N, typename T>
	T *stack<T(&)[_N]>::get(LuaGlueBase *g, lua_State *s, int idx)
	{
		LuaGlueStaticArray<_N, T> *sa = getStaticArray(g, s, idx);
		if(sa)
			return sa->ptr();
		
		return 0;
	}
	
	template<size_t _N, typename T>
	void stack<T(&)[_N]>::put(LuaGlueBase *g, lua_State *s, T (&v)[_N])
	{
		typedef LuaGlueStaticArray<_N, T> ArrayType;
		
		LuaGlueStaticArrayType<_N, T> *lgc = (LuaGlueStaticArrayType<_N, T> *)g->lookupType(typeid(LuaGlueStaticArrayType<_N, T>).name(), true);
		//printf("stack<T*>::put(T): %s %p lgc:%p\n", typeid(LuaGlueClass<T>).name(), v, lgc);
		if(!lgc)
		{
			lgc = new LuaGlueStaticArrayType<_N, T>(g);
			((LuaGlue*)g)->addType(lgc);
		}
		

		LG_Debug("stack::put<%s>: mapped %p", CxxDemangle(ArrayType), v);
		ArrayType *sa = new ArrayType(v);
		lgc->pushInstance(s, sa, true);
	}
	
	template<size_t _N, typename T>
	LuaGlueStaticArray<_N, T> *stack<T(&)[_N]>::getStaticArray(LuaGlueBase *g, lua_State *s, int idx)
	{
		typedef LuaGlueStaticArray<_N, T> ArrayType;
		
		if(checkGlueType(g, s, idx))
		{
			LG_Debug("stack::get<%s>: mapped", CxxDemangle(ArrayType));
			LuaGlueTypeValue<ArrayType> *p = (LuaGlueTypeValue<ArrayType> *)lua_touserdata(s, idx);
			LG_Debug("ptr: %p", p->ptr());
			
			return p->ptr();
		}
		
		LG_Debug("stack::get<%s>: unk", CxxDemangle(ArrayType));
		return 0;
	}

	
	template<size_t _N, typename T>
	void stack<T[_N]>::getInPlace(LuaGlueBase *g, lua_State *s, int idx, T (&dest)[_N])
	{
		LuaGlueStaticArray<_N, T> *sa = getStaticArray(g, s, idx);
		if(sa)
		{
			memcpy(dest, sa->ptr(), sizeof(T) * _N);
		}
	}
	
	template<size_t _N, typename T>
	T *stack<T[_N]>::get(LuaGlueBase *g, lua_State *s, int idx)
	{
		LuaGlueStaticArray<_N, T> *sa = getStaticArray(g, s, idx);
		if(sa)
			return sa->ptr();
		
		return 0;
	}
	
	template<size_t _N, typename T>
	void stack<T[_N]>::put(LuaGlueBase *g, lua_State *s, T (&v)[_N])
	{
		typedef LuaGlueStaticArray<_N, T> ArrayType;
		
		auto lgt = (LuaGlueStaticArrayType<_N, T>*)g->lookupType(typeid(LuaGlueStaticArrayType<_N, T>).name(), true);
		//printf("stack<T*>::put(T): %s %p lgc:%p\n", typeid(LuaGlueClass<T>).name(), v, lgc);
		if(!lgt)
		{
			lgt = new LuaGlueStaticArrayType<_N, T>(g);
			((LuaGlue*)g)->addType(lgt);
		}
		
		LG_Debug("stack::put<%s>: mapped %p", CxxDemangle(ArrayType), v);
		ArrayType *sa = new ArrayType(v);
		lgt->pushInstance(s, sa, true);
	}
	
	template<size_t _N, typename T>
	LuaGlueStaticArray<_N, T> *stack<T[_N]>::getStaticArray(LuaGlueBase *g, lua_State *s, int idx)
	{
		typedef LuaGlueStaticArray<_N, T> ArrayType;
		
		if(checkGlueType(g, s, idx))
		{
			LG_Debug("stack::get<%s>: mapped", CxxDemangle(ArrayType));
			LuaGlueTypeValue<ArrayType> *p = (LuaGlueTypeValue<ArrayType> *)lua_touserdata(s, idx);
			LG_Debug("ptr: %p", p->ptr());
			
			return p->ptr();
		}

		LG_Debug("stack::get<%s>: unk", CxxDemangle(ArrayType));
		return 0;
	}

#endif /* LUAGLUE_STACKTEMPLATES_ARRAY_H_GUARD */
