#ifndef LUAGLUE_APPLYTUPLE_H_GUARD
#define LUAGLUE_APPLYTUPLE_H_GUARD

#include <cstdint>
#include <tuple>
#include <lua.hpp>

template<typename T>
T getValue(lua_State *state, unsigned int);

template<>
int getValue<int>(lua_State *state, unsigned int idx)
{
	return luaL_checkint(state, idx);
}

template<>
double getValue<double>(lua_State *state, unsigned int idx)
{
	return luaL_checknumber(state, idx);
}

template<>
const char *getValue<const char *>(lua_State *state, unsigned int idx)
{
	return luaL_checkstring(state, idx);
}


template<typename T>
void returnValue(lua_State *, T);

template<>
void returnValue(lua_State *state, int v)
{
	lua_pushinteger(state, v);
}

template<>
void returnValue(lua_State *state, double v)
{
	lua_pushnumber(state, v);
}

template<>
void returnValue(lua_State *state, const char *v)
{
	lua_pushstring(state, v);
}

// original apply tuple code:
// http://stackoverflow.com/questions/687490/how-do-i-expand-a-tuple-into-variadic-template-functions-arguments
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
  static R applyTuple(lua_State *state, T* pObj,
                          R (T::*f)( ArgsF... ),
                          const std::tuple<ArgsT...> &t,
                          Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_obj_func<N-1>::applyTuple(state, pObj, f, std::forward<const std::tuple<ArgsT...>>(t), getValue<ltype>(state, -(argCount-N+1)), args... );
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
  static R applyTuple(lua_State *, T* pObj,
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
R applyTuple(lua_State *state, T* pObj,
                 R (T::*f)( ArgsF... ),
                 const std::tuple<ArgsT...> &t )
{
	return apply_obj_func<sizeof...(ArgsT)>::applyTuple(state, pObj, f, std::forward<const std::tuple<ArgsT...>>(t) );
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
template < uint N >
struct apply_func
{
	template < typename R, typename... ArgsF, typename... ArgsT, typename... Args >
	static R applyTuple(	lua_State *state, R (*f)( ArgsF... ),
									const std::tuple<ArgsT...>& t,
									Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_func<N-1>::applyTuple( state, f, std::forward<const std::tuple<ArgsT...>>(t), getValue<ltype>(state, -(argCount-N+1)), args... );
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
	static R applyTuple(	lua_State *, R (*f)( ArgsF... ),
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
R applyTuple( lua_State *state, R (*f)(ArgsF...),
                 const std::tuple<ArgsT...> & t )
{
	return apply_func<sizeof...(ArgsT)>::applyTuple( state, f, std::forward<const std::tuple<ArgsT...>>(t) );
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
template <class C, uint N >
struct apply_ctor_func
{
	template < typename... ArgsT, typename... Args >
	static C *applyTuple(	lua_State *state, const std::tuple<ArgsT...>& t,
								Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_ctor_func<C, N-1>::applyTuple( state, std::forward<const std::tuple<ArgsT...>>(t), getValue<ltype>(state, -(argCount-N+1)), args... );
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
	static C *applyTuple(	lua_State *, const std::tuple<ArgsT...>& /* t */,
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
C *applyTuple( lua_State *state, const std::tuple<ArgsT...> & t )
{
	return apply_ctor_func<C, sizeof...(ArgsT)>::applyTuple( state, std::forward<const std::tuple<ArgsT...>>(t) );
}

#endif /* LUAGLUE_APPLYTUPLE_H_GUARD */
