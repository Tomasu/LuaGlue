#ifndef LUAGLUE_ARRAY_H_GUARD
#define LUAGLUE_ARRAY_H_GUARD

#include <stdexcept>
#include "LuaGlue/LuaGlueTypeBase.h"
#include "LuaGlue/LuaHelpers.h"
#include "LuaGlue/LuaGlueType.h"

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
		
		PtrType ptr() { return data; }
		
	private:
		PtrType data;
};

template<int N, typename T>
class LuaGlueStaticArrayType : public LuaGlueType< LuaGlueStaticArray<N, T> >
{
	public:
		virtual ~LuaGlueStaticArrayType() { }
		LuaGlueStaticArrayType(LuaGlueBase *b, const std::string &n) : LuaGlueType< LuaGlueStaticArray<N,T> >(b, n) { }
		
		virtual std::string toString()
		{
			std::string ret;
			
			LuaGlueBase *g = this->luaGlue();
			lua_State *state = g->state();
			
			int type = lua_type(state, 1);
			if(type == LUA_TUSERDATA)
			{
				LuaGlueStaticArray<N, T> *sa = (LuaGlueStaticArray<N, T> *)lua_touserdata(state, 1);
				
				char buff[2048];
				sprintf(buff, "%s(%p)", this->name().c_str(), sa);
				
				ret = buff;
			}
			else if(type == LUA_TNIL)
			{
				LG_Debug("nil!");
				ret = "nil";
			}
			else
			{
				LG_Debug("type: %s", lua_typename(state, type));
				ret = lua_tostring(state, 1);
			}
			
			return ret;
		}
		
		virtual lua_Integer toInteger()
		{
			return 0;
		}
		
		virtual lua_Number toNumber()
		{
			return 0.0;
		}
		
	protected:
		virtual bool glue_instance_properties(LuaGlueBase *g)
		{
			// TODO: make sure this field name is consistent with lua naming.
			LuaHelpers::setField(g, "length", N);
			return true;
		}
		
		virtual int mm_index(lua_State *s)
		{
			auto obj = stack< LuaGlueStaticArray<N, T> * >::get(this->luaGlue(), s, 1);
			int idx = stack<int>::get(this->luaGlue(), s, 2);
			
			try {
				T v = (*obj)[idx];
				stack<T>::put(this->luaGlue(), s, v);
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
			auto obj = stack< LuaGlueStaticArray<N, T> * >::get(this->luaGlue(), s, 1);
			int idx = stack<int>::get(this->luaGlue(), s, 2);
			
			try {
				T v = stack<T>::get(this->luaGlue(), s, 3);
				(*obj)[idx] = v;
			}
			catch(std::range_error &ex)
			{
				luaL_error(s, "%s", ex.what());
			}
			
			return 0;
		}
};

#endif /* LUAGLUE_ARRAY_H_GUARD */
