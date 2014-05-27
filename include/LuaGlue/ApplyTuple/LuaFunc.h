#ifndef LUAGLUE_APPLYTUPLE_LUAFUNC_H_GUARD
#define LUAGLUE_APPLYTUPLE_LUAFUNC_H_GUARD

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
template < int N >
struct apply_lua_func
{
	template < typename... ArgsT, typename... Args >
	static void applyTuple(	LuaGlueBase *g, lua_State *s, const std::tuple<ArgsT...>& t,
								Args&&... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		const static unsigned int argIdx = (argCount-N);
		
		//typedef typename std::remove_reference<decltype(std::get<argIdx>(t))>::type ltype_const;
		//typedef typename std::remove_const<ltype_const>::type ltype;
		typedef typename std::remove_const<decltype(std::get<argIdx>(t))>::type ltype;
		//typedef decltype(std::get<argIdx>(t)) AType;
		stack<ltype>::put(g, s, std::forward<ltype>(std::get<argIdx>(t)));
		
		apply_lua_func<N-1>::applyTuple( g, s, std::forward<decltype(t)>(t), std::forward<ltype>(std::get<argIdx>(t)), std::forward<Args>(args)... );
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
	static void applyTuple(	LuaGlueBase *, lua_State *, const std::tuple<ArgsT...>& /* t */,
								Args&&... /*args*/ )
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
void applyTupleLuaFunc( LuaGlueBase *g, lua_State *s, Args&&... args )
{
	std::tuple<Args&&...> t(std::forward<Args>(args)...);
	apply_lua_func<sizeof...(Args)>::applyTuple( g, s, std::forward<decltype(t)>(t) );
}

#endif /* LUAGLUE_APPLYTUPLE_LUAFUNC_H_GUARD */
