#ifndef LUAGLUE_APPLY_TUPLE_STDFUNC_H_GUARD
#define LUAGLUE_APPLY_TUPLE_STDFUNC_H_GUARD

/**
 * std::function Tuple Argument Unpacking
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @tparam N Number of tuple arguments to unroll
 *
 * @ingroup g_util_tuple
 */
template < int N >
struct apply_std_func
{
	template < typename R, typename... ArgsF, typename... ArgsT, typename... Args >
	static R applyTuple(	LuaGlueBase *g, lua_State *s, std::function<R(ArgsF...)> f,
									const std::tuple<ArgsT...>& t,
									Args... args )
	{
		const static int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_std_func<N-1>::applyTuple( g, s, f, std::forward<decltype(t)>(t), stack<ltype>::get(g, s, -(argCount-N+1)), args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * std::function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <>
struct apply_std_func<0>
{
	template < typename R, typename... ArgsF, typename... ArgsT, typename... Args >
	static R applyTuple(	LuaGlueBase *, lua_State *, std::function<R(ArgsF...)> f,
									const std::tuple<ArgsT...>& /* t */,
									Args... args )
	{
		return f( args... );
	}
};

//-----------------------------------------------------------------------------

/**
 * std::function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename R, typename... ArgsF, typename... ArgsT >
R applyTuple( LuaGlueBase *g, lua_State *s, std::function<R(ArgsF...)> f,
                 const std::tuple<ArgsT...> & t )
{
	return apply_std_func<sizeof...(ArgsT)>::applyTuple( g, s, f, std::forward<decltype(t)>(t) );
}


#endif /* LUAGLUE_APPLY_TUPLE_STDFUNC_H_GUARD */
