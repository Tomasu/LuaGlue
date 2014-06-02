#ifndef LUAGLUE_APPLYTUPLE_GLUEOBJFUNC_H_GUARD
#define LUAGLUE_APPLYTUPLE_GLUEOBJFUNC_H_GUARD

/**
 * LuaGlueObject Function Tuple Argument Unpacking
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
struct apply_glueobj_func
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlueBase *g, lua_State *s, LuaGlueObject<T> &pObj,
                          R (T::*f)( ArgsF... ),
                          const std::tuple<ArgsT...> &t,
                          Args&&... args )
	{
		const static int argCount = sizeof...(ArgsT);
		//typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		//typedef typename std::remove_const<ltype_const>::type ltype;
		typedef typename std::remove_const<decltype(std::get<N-1>(t))>::type ltype;
		int idx = lua_absindex(s, -(argCount-N+1));
		LG_Debug("apply_glueobj_func<%i>::applyTuple arg(%i,%i):%s", N, lua_gettop(s), idx, CxxDemangle(ltype));
		return apply_glueobj_func<N-1>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t), stack<ltype>::get(g, s, idx), std::forward<Args>(args)... );
	}
};

//-----------------------------------------------------------------------------

/**
 * LuaGlueObject Function Tuple Argument Unpacking End Point
 *
 * This recursive template unpacks the tuple parameters into
 * variadic template arguments until we reach the count of 0 where the function
 * is called with the correct parameters
 *
 * @ingroup g_util_tuple
 */
template <>
struct apply_glueobj_func<0>
{
  template < typename T, typename R, typename... ArgsF, typename... ArgsT, typename... Args >
  static R applyTuple(LuaGlueBase *, lua_State *, LuaGlueObject<T> &pObj,
                          R (T::*f)( ArgsF... ),
                          const std::tuple<ArgsT...> &/* t */,
                          Args&&... args )
	{
		LG_Debug("glueobj call!");
		return (pObj.ptr()->*f)( std::forward<Args>(args)... );
	}
};

//-----------------------------------------------------------------------------

/**
 * LuaGlueObject Function Call Forwarding Using Tuple Pack Parameters
 */
// Actual apply function
template < typename T, typename R, typename... ArgsF, typename... ArgsT >
R applyTuple(LuaGlueBase *g, lua_State *s, LuaGlueObject<T> &pObj,
                 R (T::*f)( ArgsF... ),
                 const std::tuple<ArgsT...> &t )
{
	//LG_Debug("before apply_glueobj_func<%i>::applyTuple", sizeof...(ArgsT));
	return apply_glueobj_func<sizeof...(ArgsT)>::applyTuple(g, s, pObj, f, std::forward<decltype(t)>(t) );
}

#endif /* LUAGLUE_APPLYTUPLE_GLUEOBJFUNC_H_GUARD */
