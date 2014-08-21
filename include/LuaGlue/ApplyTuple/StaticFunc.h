#ifndef LUAGLUE_APPLYTUPLE_STATICFUNC_H_GUARD
#define LUAGLUE_APPLYTUPLE_STATICFUNC_H_GUARD

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
template < int N >
struct apply_func
{
	template < typename R, typename... ArgsF, typename... ArgsT, typename... Args >
	static R applyTuple(	LuaGlueBase *g, lua_State *s, R (*f)( ArgsF ... ),
									const std::tuple<ArgsT...>& t,
									Args... args )
	{
		const static int argCount = sizeof...(ArgsT);
		//typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		//typedef typename std::remove_const<ltype_const>::type ltype;
		typedef typename std::remove_const<decltype(std::get<N-1>(t))>::type ltype;
		int idx = -(argCount-N+1); //lua_absindex(s, -(argCount-N+1));
		ltype v = stack<ltype>::get(g, s, idx);
		LG_Debug("apply_func<%i>::applyTuple arg(%i,%i): %p %s %s", N, lua_gettop(s), idx, v, CxxDemangle(ltype), lua_typename(s, lua_type(s, idx)));
		return apply_func<N-1>::applyTuple( g, s, f, std::forward<decltype(t)>(t), v, std::forward<Args>(args)... );
	}
	
	/*
	template < typename R, typename... ArgsF, typename... ArgsT, typename... Args >
	static R applyTuple(	LuaGlueBase *g, lua_State *s, R (*f)( ArgsF ... ),
									const std::tuple<ArgsT...>& t,
									decltype(std::get<N-1>(t)) argN,
									Args &&... args )
	{
		const static int argCount = sizeof...(ArgsT);
		//typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		//typedef typename std::remove_const<ltype_const>::type ltype;
		typedef typename std::remove_const<decltype(std::get<N-1>(t))>::type ltype;
		LG_Debug("get arg: %s", CxxDemangle(ltype));
		return apply_func<N-1>::applyTuple( g, s, f, std::forward<decltype(t)>(t), stack<ltype>::get(g, s, -(argCount-N+1)), std::forward<Args>(args)... );
	}
	*/
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
	static R applyTuple(	LuaGlueBase *, lua_State *, R (*f)( ArgsF ... ),
									const std::tuple<ArgsT...>& /* t */,
									Args... args )
	{
		LG_Debug("eoat");
		return f( std::forward<Args>(args)... );
	}
};

//-----------------------------------------------------------------------------

/**
 * Static Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename R, typename... ArgsF, typename... ArgsT >
R applyTupleStaticFunc( LuaGlueBase *g, lua_State *s, R (*f)(ArgsF...),
                 const std::tuple<ArgsT...> & t )
{
	LG_Debug("in StaticFunc applyTuple N=%i", sizeof...(ArgsT));
	return apply_func<sizeof...(ArgsT)>::applyTuple( g, s, f, std::forward<decltype(t)>(t) );
}

#endif /* LUAGLUE_APPLYTUPLE_STATICFUNC_H_GUARD */
