#ifndef LUAGLUE_STDFUNC_WRAPPER_H_GUARD
#define LUAGLUE_STDFUNC_WRAPPER_H_GUARD

class LuaGlueBase;

#include "LuaGlue/LuaGlueStackTemplates.h"
#include "LuaGlue/ApplyTuple/StdFunc.h"

template<typename _Ret, typename... _Args>
class LuaGlueStdFuncWrapper
{
	private:
		typedef std::tuple<typename std::remove_const<typename std::remove_reference<_Args>::type>::type...> ArgsTuple;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
		
	public:
		typedef _Ret ReturnType;
		typedef std::function<_Ret(_Args...)> FunctionType;
		
		LuaGlueStdFuncWrapper(LuaGlueBase *g, const FunctionType &f) : _g(g), _f(f)
		{
			//glue();
		}
		
		LuaGlueStdFuncWrapper(const LuaGlueStdFuncWrapper<_Ret, _Args...> &o) : _g(o._g), _f(o._f)
		{
			//glue();
		}
		
		~LuaGlueStdFuncWrapper() { }

		LuaGlueStdFuncWrapper &operator=( const LuaGlueStdFuncWrapper &o )
		{
			_g = o._g;
			_f = o._f;
			glue();
			
			return *this;
		}
		
		operator const FunctionType &()
		{
			return _f;
		}
		
		ReturnType operator()(_Args... args)
		{
			return _f(std::forward<_Args>(args)...);
		}
		
		int invoke(lua_State *s)
		{
			ReturnType ret;

			ret = applyTuple<_Ret, _Args...>(_g, s, _f, args);
			if(Arg_Count_) lua_pop(s, (int)Arg_Count_);
			stack<ReturnType>::put(_g, s, ret);
			
			return 1;
		}
		
		void glue(LuaGlueBase *g)
		{
			lua_pushlightuserdata(g->state(), this);
			lua_pushcclosure(g->state(), &invoke_cb, 1);
		}
		
	private:
		static int invoke_cb(lua_State *s)
		{
			auto imp = (LuaGlueStdFuncWrapper<_Ret, _Args...> *)lua_touserdata(s, lua_upvalueindex(1));
			return imp->invoke(s);
		}
		
		LuaGlueBase *_g;
		std::function<_Ret(_Args...)> _f;
		ArgsTuple args;
};

template<typename... _Args>
class LuaGlueStdFuncWrapper<void, _Args...>
{
	private:
		typedef std::tuple<typename std::remove_const<typename std::remove_reference<_Args>::type>::type...> ArgsTuple;
		static const unsigned int Arg_Count_ = sizeof...(_Args);
		
	public:
		typedef std::function<void(_Args...)> FunctionType;
		
		LuaGlueStdFuncWrapper(LuaGlueBase *g, const FunctionType &f) : _g(g), _f(f)
		{
		}
		
		LuaGlueStdFuncWrapper(const LuaGlueStdFuncWrapper<void, _Args...> &o) : _g(o._g), _f(o._f)
		{
		}
		
		~LuaGlueStdFuncWrapper() { }

		LuaGlueStdFuncWrapper &operator=( const LuaGlueStdFuncWrapper &o )
		{
			_g = o._g;
			_f = o._f;
			glue(_g);
			
			return *this;
		}
		
		operator const FunctionType &()
		{
			return _f;
		}
		
		void operator()(_Args... args)
		{
			_f(std::forward<_Args>(args)...);
		}
		
		int invoke(lua_State *s)
		{
			applyTuple<void, _Args...>(_g, s, _f, args);
			if(Arg_Count_) lua_pop(s, (int)Arg_Count_);
			
			return 0;
		}
		
		void glue(LuaGlueBase *g)
		{
			lua_pushlightuserdata(g->state(), this);
			lua_pushcclosure(g->state(), &invoke_cb, 1);
		}
		
	private:
		static int invoke_cb(lua_State *s)
		{
			auto imp = (LuaGlueStdFuncWrapper<void, _Args...> *)lua_touserdata(s, lua_upvalueindex(1));
			return imp->invoke(s);
		}
		
		LuaGlueBase *_g;
		std::function<void(_Args...)> _f;
		ArgsTuple args;
};

#endif /* LUAGLUE_STDFUNC_WRAPPER_H_GUARD */
