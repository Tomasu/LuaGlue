#ifndef LUAGLUE_APPLYTUPLE_H_GUARD
#define LUAGLUE_APPLYTUPLE_H_GUARD

#include <cstdint>
#include <tuple>
#include <lua.hpp>
#include <typeinfo>

class LuaGlue;

template<class T>
LuaGlueClass<T> *getGlueClass(LuaGlue &g, lua_State *s, unsigned int idx)
{
	int ret = luaL_getmetafield(s, idx, LuaGlueClass<T>::METATABLE_CLASSIDX_FIELD);
	if(!ret)
	{
		printf("getGlueClassPtr: typeid:%s\n", typeid(LuaGlueClass<T>).name());
		printf("getGlueClassPtr: failed to get metafield for obj at idx %i\n", idx);
		return 0;
	}
	
	int id = luaL_checkint(s, -1);
	lua_pop(s, 1);
	
	//printf("getGlueClass: METATABLE_CLASSIDX_FIELD: %i\n", id);
	return (LuaGlueClass<T> *)g.lookupClass(id);
}


template<class T>
struct stack {
	static T get(LuaGlue &g, lua_State *s, unsigned int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			printf("stack<T>::get: lud!\n");
			return *(T*)lua_touserdata(s, idx);
		}
		
#ifdef LUAGLUE_TYPECHECK
		LuaGlueClass<T> *lgc = getGlueClass<T>(g, s, idx);
		if(lgc)
		{
#else
			(void)g;
#endif
			return **(T **)lua_touserdata(s, idx);
#ifdef LUAGLUE_TYPECHECK
		}
#endif

		printf("stack::get<T>: failed to get a class instance for lua stack value at idx: %i\n", idx);
		return T();
	}
	
	static void put(LuaGlue &g, lua_State *s, T v)
	{
		//printf("stack<T>::put(T)\n");
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g.lookupClass(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			lgc->pushInstance(s, new T(v));
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T>: lud!\n");
		lua_pushlightuserdata(s, new T(v));
	}
	
	// for putting static types
	static void put(LuaGlue &g, lua_State *s, T *v)
	{
		//printf("stack<T>::put(T*)\n");
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g.lookupClass(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T>: lud!\n");
		lua_pushlightuserdata(s, v);
	}
};

template<class T>
struct stack<std::shared_ptr<T>> {
	static std::shared_ptr<T> get(LuaGlue &g, lua_State *s, unsigned int idx)
	{
		if(lua_islightuserdata(s, idx))
		{
			printf("stack<shared_ptr<T>>::get: lud!\n");
			std::shared_ptr<T> **ptr_ptr = (std::shared_ptr<T> **)lua_touserdata(s, idx);
			std::shared_ptr<T> ptr = **ptr_ptr;
			return ptr;
		}
		
		//printf("stack<shared_ptr<T>>::get: name:%s\n", typeid(T).name());
#ifdef LUAGLUE_TYPECHECK
		LuaGlueClass<T> *lgc = getGlueClass<T>(g, s, idx);
		if(lgc)
		{
#else
			(void)g;
#endif
			std::shared_ptr<T> **ptr_ptr = (std::shared_ptr<T> **)lua_touserdata(s, idx);
			std::shared_ptr<T> ptr = **ptr_ptr;
			return ptr;

#ifdef LUAGLUE_TYPECHECK
		}
#endif

		printf("stack::get<T>: failed to get a class instance for lua stack value at idx: %i\n", idx);
		return 0; // TODO: is this a valid thing? I can't imagine this is a good thing.
	}
	
	static void put(LuaGlue &g, lua_State *s, std::shared_ptr<T> v)
	{
		//printf("stack<T>::put(T)\n");
		LuaGlueClass<T> *lgc = (LuaGlueClass<T> *)g.lookupClass(typeid(LuaGlueClass<T>).name(), true);
		if(lgc)
		{
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T>: lud!\n");
		std::shared_ptr<T> *ptr = new std::shared_ptr<T>(v);
		lua_pushlightuserdata(s, ptr);
	}
};

template<>
struct stack<int> {
	static int get(LuaGlue &, lua_State *s, unsigned int idx)
	{
		return luaL_checkint(s, idx);
	}
	
	static void put(LuaGlue &, lua_State *s, int v)
	{
		lua_pushinteger(s, v);
	}
};

template<>
struct stack<float> {
	static float get(LuaGlue &, lua_State *s, unsigned int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	static void put(LuaGlue &, lua_State *s, float v)
	{
		lua_pushnumber(s, v);
	}
};

template<>
struct stack<double> {
	static double get(LuaGlue &, lua_State *s, unsigned int idx)
	{
		return luaL_checknumber(s, idx);
	}
	
	static void put(LuaGlue &, lua_State *s, double v)
	{
		lua_pushnumber(s, v);
	}
};

template<>
struct stack<const char *> {
	static const char *get(LuaGlue &, lua_State *s, unsigned int idx)
	{
		return luaL_checkstring(s, (int)idx);
	}
	
	static void put(LuaGlue &, lua_State *s, const char *v)
	{
		lua_pushstring(s, v);
	}
};

template<class T>
struct stack<T *> {
	static T *get(LuaGlue &g, lua_State *s, unsigned int idx)
	{
		//printf("stack<T*>::get: idx:%i\n", idx);
		if(lua_islightuserdata(s, idx))
		{
			//printf("stack<T*>::get: lud!\n");
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
			T *v = (T *)lua_touserdata(s, idx);
			return v;
#ifdef LUAGLUE_TYPECHECK
		}
#endif
		
		printf("stack::get<T*>: failed to get a class instance for lua stack value at idx: %i\n", idx);
		return 0;
	}
	
	static void put(LuaGlue &g, lua_State *s, T *v)
	{
		//printf("stack<T>::put(T*) begin!\n");
		// first look for a class we support
		
		typedef typename std::remove_pointer<T>::type TC;
		LuaGlueClass<TC> *lgc = (LuaGlueClass<TC> *)g.lookupClass(typeid(LuaGlueClass<TC>).name(), true);
		//printf("stack<T*>::put(T): %s %p lgc:%p\n", typeid(LuaGlueClass<T>).name(), v, lgc);
		if(lgc)
		{
			lgc->pushInstance(s, v);
			return;
		}
		
		// otherwise push onto stack as light user data
		//printf("stack::put<T*>: lud!\n");
		lua_pushlightuserdata(s, v);
	}
};

// original apply tuple code:
// http://stackoverflow.com/questions/687490/how-do-i-expand-a-tuple-into-variadic-template-functions-arguments


//-----------------------------------------------------------------------------

/**
 * Object Function Tuple Argument Unpacking
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @tparam N Number of tuple arguments to unroll
 *
 * @ingroup g_util_tuple
 */
template < uint32_t N >
struct apply_obj_func
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlue &g, lua_State *s, T* pObj,
                          R (T::*f)( ArgsF... ),
                          const std::tuple<ArgsT...> &t,
                          Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_obj_func<N-1>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t), stack<ltype>::get(g, s, -(argCount-N+1)), args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * Object Function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <>
struct apply_obj_func<0>
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlue &, lua_State *, T* pObj,
                          R (T::*f)( ArgsF... ),
                          const std::tuple<ArgsT...> &/* t */,
                          Args... args )
	{
		return (pObj->*f)( args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * Object Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename T, typename R, typename... ArgsF, typename... ArgsT >
R applyTuple(LuaGlue &g, lua_State *s, T* pObj,
                 R (T::*f)( ArgsF... ),
                 const std::tuple<ArgsT...> &t )
{
	return apply_obj_func<sizeof...(ArgsT)>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t) );
}

//-----------------------------------------------------------------------------

/**
 * Static Function Tuple Argument Unpacking
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @tparam N Number of tuple arguments to unroll
 *
 * @ingroup g_util_tuple
 */
template < uint32_t N >
struct apply_func
{
	template < typename R, typename... ArgsF, typename... ArgsT, typename... Args >
	static R applyTuple(	LuaGlue &g, lua_State *s, R (*f)( ArgsF... ),
									const std::tuple<ArgsT...>& t,
									Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_func<N-1>::applyTuple( g, s, f, std::forward<decltype(t)>(t), stack<ltype>::get(g, s, -(argCount-N+1)), args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * Static Function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <>
struct apply_func<0>
{
	template < typename R, typename... ArgsF, typename... ArgsT, typename... Args >
	static R applyTuple(	LuaGlue &, lua_State *, R (*f)( ArgsF... ),
									const std::tuple<ArgsT...>& /* t */,
									Args... args )
	{
		return f( args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * Static Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename R, typename... ArgsF, typename... ArgsT >
R applyTuple( LuaGlue &g, lua_State *s, R (*f)(ArgsF...),
                 const std::tuple<ArgsT...> & t )
{
	return apply_func<sizeof...(ArgsT)>::applyTuple( g, s, f, std::forward<decltype(t)>(t) );
}

//-----------------------------------------------------------------------------

// object constructor tuple unpack

/**
 * Ctor Function Tuple Argument Unpacking
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @tparam N Number of tuple arguments to unroll
 *
 * @ingroup g_util_tuple
 */
template <class C, uint32_t N >
struct apply_ctor_func
{
	template < typename... ArgsT, typename... Args >
	static C *applyTuple(	LuaGlue &g, lua_State *s, const std::tuple<ArgsT...>& t,
								Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_ctor_func<C, N-1>::applyTuple( g, s, std::forward<decltype(t)>(t), stack<ltype>::get(g, s, -(argCount-N+1)), args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * ctor Function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <class C>
struct apply_ctor_func<C, 0>
{
	template < typename... ArgsT, typename... Args >
	static C *applyTuple(	LuaGlue &, lua_State *, const std::tuple<ArgsT...>& /* t */,
								Args... args )
	{
		return new C( args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * ctor Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename C, typename... ArgsT >
C *applyTuple( LuaGlue &g, lua_State *s, const std::tuple<ArgsT...> & t )
{
	return apply_ctor_func<C, sizeof...(ArgsT)>::applyTuple( g, s, std::forward<decltype(t)>(t) );
}


// lua function tuple unpack

/**
 * Lua Function Tuple Argument Unpacking
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @tparam N Number of tuple arguments to unroll
 *
 * @ingroup g_util_tuple
 */
template < uint32_t N >
struct apply_lua_func
{
	template < typename... ArgsT, typename... Args >
	static void applyTuple(	LuaGlue &g, lua_State *s, const std::tuple<ArgsT...>& t,
								Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		const static unsigned int argIdx = (argCount-N);
		
		typedef typename std::remove_reference<decltype(std::get<argIdx>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		stack<ltype>::put(g, s, std::get<argIdx>(t));
		
		apply_lua_func<N-1>::applyTuple( g, s, std::forward<decltype(t)>(t), std::get<argIdx>(t), args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * Lua Function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <>
struct apply_lua_func<0>
{
	template < typename... ArgsT, typename... Args >
	static void applyTuple(	LuaGlue &, lua_State *, const std::tuple<ArgsT...>& /* t */,
								Args... /*args*/ )
	{
		// nada
	}
};

//-----------------------------------------------------------------------------

/**
 * Lua Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename... Args >
void applyTuple( LuaGlue &g, lua_State *s, Args... args )
{
	std::tuple<Args...> t(args...);
	apply_lua_func<sizeof...(Args)>::applyTuple( g, s, std::forward<decltype(t)>(t) );
}


#endif /* LUAGLUE_APPLYTUPLE_H_GUARD */
