#ifndef LUAGLUE_APPLYTUPLE_OBJCTOR_H_GUARD
#define LUAGLUE_APPLYTUPLE_OBJCTOR_H_GUARD

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
template <class C, int N >
struct apply_ctor_func
{
	template < typename... ArgsT, typename... Args >
	static C *applyTuple(	LuaGlueBase *g, lua_State *s, const std::tuple<ArgsT...>& t,
								Args... args )
	{
		const static int argCount = sizeof...(ArgsT);
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
	static C *applyTuple(	LuaGlueBase *, lua_State *, const std::tuple<ArgsT...>& /* t */,
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
C *applyTuple( LuaGlueBase *g, lua_State *s, const std::tuple<ArgsT...> & t )
{
	return apply_ctor_func<C, sizeof...(ArgsT)>::applyTuple( g, s, std::forward<decltype(t)>(t) );
}

#endif /* LUAGLUE_APPLYTUPLE_OBJCTOR_H_GUARD */
