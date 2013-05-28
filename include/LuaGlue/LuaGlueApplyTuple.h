#ifndef LUAGLUE_APPLYTUPLE_H_GUARD
#define LUAGLUE_APPLYTUPLE_H_GUARD

#include <cstdint>
#include <tuple>
#include <lua.hpp>
#include <typeinfo>

class LuaGlue;

template<typename T>
T getValue(LuaGlue &, lua_State *, unsigned int);

template<>
int getValue<int>(LuaGlue &, lua_State *state, unsigned int idx)
{
	return luaL_checkint(state, idx);
}

template<>
double getValue<double>(LuaGlue &, lua_State *state, unsigned int idx)
{
	return luaL_checknumber(state, idx);
}

template<>
const char *getValue<const char *>(LuaGlue &, lua_State *state, unsigned int idx)
{
	return luaL_checkstring(state, idx);
}

template<class T>
T getValue(LuaGlue &g, lua_State *state, unsigned int idx)
{
	typedef typename std::remove_pointer<T>::type TC;
	
	if(lua_islightuserdata(state, idx))
		return (T)lua_touserdata(state, idx);
	
	for(auto &c: g.getClasses())
	{
		LuaGlueClass<TC> *lgc = dynamic_cast<LuaGlueClass<TC> *>(c.second);
		if(lgc)
		{
			T v = *(T *)luaL_checkudata(state, idx, lgc->name().c_str());
			if(v)
			{
				printf("getValue: found the right class! v=%s lgc=%s c=%s\n", typeid(v).name(), typeid(lgc).name(), typeid(c.second).name());
				return v;
			}
			else
			{
				printf("getValue: found the right class! v=%s lgc=%s c=%s\n", typeid(v).name(), typeid(lgc).name(), typeid(c.second).name());
				printf("except that the udata was null??\n");
			}
		}
		else
		{
			printf("getValue: found the wrong class! c=%s\n", typeid(c.second).name());
		}
	}
	
	printf("getValue: failed to get a class instance for lua stack value at idx: %i\n", idx);
	return 0;
}

template<typename T>
void returnValue(LuaGlue &, lua_State *, T);

template<>
void returnValue(LuaGlue &, lua_State *state, int v)
{
	lua_pushinteger(state, v);
}

template<>
void returnValue(LuaGlue &, lua_State *state, double v)
{
	lua_pushnumber(state, v);
}

template<>
void returnValue(LuaGlue &, lua_State *state, const char *v)
{
	lua_pushstring(state, v);
}

template<class T>
void returnValue(LuaGlue &g, lua_State *state, T *v)
{
	// first look for a class we support
	for(auto &c: g.getClasses())
	{
		LuaGlueClass<T> *lgc = dynamic_cast<LuaGlueClass<T> *>(c.second);
		if(lgc)
		{
			printf("returnValue: found the right class! v=%s lgc=%s c=%s\n", typeid(v).name(), typeid(lgc).name(), typeid(c.second).name());
			lgc->pushInstance(v);
			return;
		}
		else
		{
			printf("returnValue: found the wrong class! v=%s c=%s\n", typeid(v).name(), typeid(c.second).name());
		}
	}
	
	// otherwise push onto stack as light user data
	lua_pushlightuserdata(state, v);
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
  static R applyTuple(LuaGlue &g, lua_State *state, T* pObj,
                          R (T::*f)( ArgsF... ),
                          const std::tuple<ArgsT...> &t,
                          Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_obj_func<N-1>::applyTuple(g, state, pObj, f, std::forward<decltype(t)>(t), getValue<ltype>(g, state, -(argCount-N+1)), args... );
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
R applyTuple(LuaGlue &g, lua_State *state, T* pObj,
                 R (T::*f)( ArgsF... ),
                 const std::tuple<ArgsT...> &t )
{
	return apply_obj_func<sizeof...(ArgsT)>::applyTuple(g, state, pObj, f, std::forward<decltype(t)>(t) );
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
	static R applyTuple(	LuaGlue &g, lua_State *state, R (*f)( ArgsF... ),
									const std::tuple<ArgsT...>& t,
									Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_func<N-1>::applyTuple( g, state, f, std::forward<decltype(t)>(t), getValue<ltype>(g, state, -(argCount-N+1)), args... );
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
R applyTuple( LuaGlue &g, lua_State *state, R (*f)(ArgsF...),
                 const std::tuple<ArgsT...> & t )
{
	return apply_func<sizeof...(ArgsT)>::applyTuple( g, state, f, std::forward<decltype(t)>(t) );
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
	static C *applyTuple(	LuaGlue &g, lua_State *state, const std::tuple<ArgsT...>& t,
								Args... args )
	{
		const static unsigned int argCount = sizeof...(ArgsT);
		typedef typename std::remove_reference<decltype(std::get<N-1>(t))>::type ltype_const;
		typedef typename std::remove_const<ltype_const>::type ltype;
		return apply_ctor_func<C, N-1>::applyTuple( g, state, std::forward<decltype(t)>(t), getValue<ltype>(g, state, -(argCount-N+1)), args... );
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
C *applyTuple( LuaGlue &g, lua_State *state, const std::tuple<ArgsT...> & t )
{
	return apply_ctor_func<C, sizeof...(ArgsT)>::applyTuple( g, state, std::forward<decltype(t)>(t) );
}

#endif /* LUAGLUE_APPLYTUPLE_H_GUARD */
