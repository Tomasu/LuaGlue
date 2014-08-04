#ifndef LUAGLUE_ARRAY_H_GUARD
#define LUAGLUE_ARRAY_H_GUARD

#include <stdexcept>
#include "LuaGlue/LuaGlueTypeBase.h"
#include "LuaGlue/LuaHelpers.h"
#include "LuaGlue/LuaGlueType.h"

#include "LuaGlue/StackTemplates/SharedPtr.h"
#include "LuaGlue/StackTemplates/Integer.h"
#include "LuaGlue/StackTemplates/Numeric.h"
#include "LuaGlue/StackTemplates/String.h"
#include "LuaGlue/StackTemplates/Ptr.h"
#include "LuaGlue/StackTemplates/StaticObj.h"

template<int N, typename T>
class LuaGlueStaticArray
{
	public:
		typedef T (&ArrayType)[N];
		typedef T *PtrType;
		
		LuaGlueStaticArray(ArrayType array) : data(array)
		{
			
		}
		
		~LuaGlueStaticArray() { LG_Debug("dtor"); data = nullptr; }
		
		T& operator[] (const int idx)
		{
			LG_Debug("operator[]: %i", idx);
			if(idx < 0 || idx >= N)
				throw std::runtime_error("LuaGlueStaticArray index out of bounds");

			return data[idx];
		}
		
		T at(const int &idx)
		{
			LG_Debug("at: %i", idx);
			if(idx < 0 || idx >= N)
				throw std::runtime_error("LuaGlueStaticArray index out of bounds");

			return data[idx];
		}
		
		void set(const int &idx, T &v)
		{
			LG_Debug("set: %i", idx);
			if(idx < 0 || idx >= N)
				throw std::runtime_error("LuaGlueStaticArray index out of bounds");

			data[idx] = v;
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
		LuaGlueStaticArrayType(LuaGlueBase *b) : LuaGlueType< LuaGlueStaticArray<N,T> >(b, typeid(decltype(*this)).name()) { }
		
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
			LG_Debug("index");
			//auto obj = stack< LuaGlueStaticArray<N, T> * >::get(this->luaGlue(), s, 1);
			//auto obj = GetLuaUdata(s, 1, typeid(LuaGlueStaticArrayType<N, T>).name());
			LuaGlueTypeValue< LuaGlueStaticArray<N, T> > *obj = (LuaGlueTypeValue<LuaGlueStaticArray<N, T>> *)GetLuaUdata(s, 1, typeid(LuaGlueStaticArrayType<N, T>).name());
			int idx = stack<int>::get(this->luaGlue(), s, 2);
			
			try {
				T v = obj->ptr()->at(idx);
				stack<T>::put(this->luaGlue(), s, v);
				return 1;
			}
			catch(std::range_error &ex)
			{
				luaL_error(s, "%s", ex.what());
			}
			
			LG_Debug("index end");
			
			return 0;
		}
		
		virtual int mm_newindex(lua_State *s)
		{
			LG_Debug("newindex");
			LuaGlueTypeValue< LuaGlueStaticArray<N, T> > *obj = (LuaGlueTypeValue<LuaGlueStaticArray<N, T>> *)GetLuaUdata(s, 1, typeid(LuaGlueStaticArrayType<N, T>).name());
			//auto obj = stack< LuaGlueStaticArray<N, T> * >::get(this->luaGlue(), s, 1);
			int idx = stack<int>::get(this->luaGlue(), s, 2);
			
			try {
				T v = stack<T>::get(this->luaGlue(), s, 3);
				obj->ptr()->set(idx, v);
			}
			catch(std::range_error &ex)
			{
				luaL_error(s, "%s", ex.what());
			}

			LG_Debug("newindex end");
			return 0;
		}
};

#endif /* LUAGLUE_ARRAY_H_GUARD */
