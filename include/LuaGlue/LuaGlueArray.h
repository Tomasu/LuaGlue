#ifndef LUAGLUE_ARRAY_H_GUARD
#define LUAGLUE_ARRAY_H_GUARD

#include <stdexcept>
#include "LuaGlue/LuaGlueTypeBase.h"
#include "LuaGlue/LuaHelpers.h"

template<int N, typename T>
class LuaGlueStaticArray
{
	public:
		typedef T (&ArrayType)[N];
		typedef T *PtrType;
		
		LuaGlueStaticArray(ArrayType array) : data(array)
		{
			
		}
		
		T& operator[] (const int idx)
		{
#ifdef LUAGLUE_TYPECHECK
			if(idx < 0 || idx >= N)
				throw std::runtime_error("LuaGlueStaticArray index out of bounds");
#endif /* LUAGLUE_TYPECHECK */
			return data[idx];
		}
		
	private:
		PtrType data;
};

template<int N, typename T>
class LuaGlueStaticArrayType : public LuaGlueTypeBase
{
	public:
		virtual ~LuaGlueStaticArrayType() { }
		
	protected:
		virtual bool glue_instance_properties(LuaGlueBase *g)
		{
			// TODO: make sure this field name is consistent with lua naming.
			LuaHelpers::setField(g, "length", N);
			return true;
		}
		
		virtual int mm_index(lua_State *s)
		{
			LuaGlueStaticArray<N, T> *obj = stack< LuaGlueStaticArray<N, T> * >::get(g, s, 1);
			int idx = stack<int>::get(g, s, 2);
			
			try {
				T v = obj[idx];
				stack<T>::put(g, s, obj[idx]);
				return 1;
			}
			catch(std::range_error &ex)
			{
				luaL_error(s, "%s", ex.what());
			}
			
			return 0;
		}
		
		virtual int mm_newindex(lua_State *s)
		{
			LuaGlueStaticArray<N, T> *obj = stack< LuaGlueStaticArray<N, T> * >::get(g, s, 1);
			int idx = stack<int>::get(g, s, 2);
			
			try {
				T v = stack<T>::get(g, s, 3);
				obj[idx] = v;
			}
			catch(std::range_error &ex)
			{
				luaL_error(s, "%s", ex.what());
			}
			
			return 0;
		}
};

#endif /* LUAGLUE_ARRAY_H_GUARD */
