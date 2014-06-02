#ifndef LUAGLUE_ARRAY_H_GUARD
#define LUAGLUE_ARRAY_H_GUARD

#include <stdexcept>

template<int N, typename T>
class LuaGlueStaticArray
{
	public:
		LuaGlueStaticArray(T t[N]) : data(t) { }
		virtual ~LuaGlueStaticArray() { }
		
		T &operator[]( int idx )
		{
			return data[idx];
		}
		
		const T at( int idx )
		{
			if(idx < 0 || idx > N)
				throw std::out_of_range("LuaGlueStaticArray access out of range");
			
			return data[idx];
		}
		
		T *ptr() { return data; }
		
		static void glue(LuaGlueClass<LuaGlueStaticArray<N, T>> &c, std::true_type)
		{
			c.index(&LuaGlueStaticArray<N, T>::index_class);
			c.newindex(&LuaGlueStaticArray<N, T>::newindex_class);
		}
		
		static void glue(LuaGlueClass<LuaGlueStaticArray<N, T>> &c, std::false_type)
		{
			c.index(&LuaGlueStaticArray<N, T>::index_basic);
			c.newindex(&LuaGlueStaticArray<N, T>::newindex_basic);
		}
		
		static void glue(LuaGlueClass<LuaGlueStaticArray<N, T>> &c)
		{
			glue(c, std::is_class<T>());
		}
		
	private:
		T *data;
		
		void newindex_class(int k, T *value) { LG_Debug("newindex %i: %p", k, value); data[k] = *value; }
		void newindex_basic(int k, T value) { LG_Debug("newindex %i", k); data[k] = value; }
		
		T *index_class(int k) { LG_Debug("index %i!", k); return &data[k]; }
		T index_basic(int k) { LG_Debug("index %i!", k); return data[k]; }
};

#endif /* LUAGLUE_ARRAY_H_GUARD */
