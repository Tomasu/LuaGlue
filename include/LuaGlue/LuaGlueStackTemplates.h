#ifndef LUAGLUE_STACK_TEMPLATES_H_GUARD
#define LUAGLUE_STACK_TEMPLATES_H_GUARD

#include <lua.hpp>
#include <cstdint>
#include <memory.h>

#include "LuaGlue/LuaGlueTypeBase.h"
#include "LuaGlue/LuaGlueUtils.h"

class LuaGlueBase;

// FIXME: maybe add a type id to check against?
inline bool checkGlueType(LuaGlueBase *g, lua_State *s, int idx)
{
	if(LUAGLUE_TYPECHECK)
	{
		int ret = luaL_getmetafield(s, idx, LuaGlueTypeBase::METATABLE_TYPEIDINT_FIELD);
		if(!ret)
		{
			LG_Warn("failed to get metafield for obj at idx %i", idx);
			return false;
		}
		
		int id = luaL_checkint(s, -1);
		lua_pop(s, 1);
		
		auto c = (LuaGlueTypeBase *)g->lookupType((uint32_t)id);
		if(!c)
		{
			LG_Debug("failed to find type by id %i", (uint32_t)id);
		}
		else
		{
			LG_Debug("METATABLE_TYPEIDINT_FIELD: %i name: %s", id, c->name().c_str());
		}
		
		return c != nullptr;
	}
	
	// fallthrough
	return true;
}

template<typename T, class Enable = void>
struct stack;

template<class T>
struct stack<std::shared_ptr<T>> {
	static std::shared_ptr<T> get(LuaGlueBase *g, lua_State *s, int idx);
	static void put(LuaGlueBase *g, lua_State *s, std::shared_ptr<T> v);
};

template<class _Class>
class LuaGlueTypeValue;

template<class T>
struct stack<std::shared_ptr<T>&> {
	static std::shared_ptr<T> get(LuaGlueBase *g, lua_State *s, int idx);
	static void put(LuaGlueBase *g, lua_State *s, std::shared_ptr<T> v);
};

template<class T>
struct stack<LuaGlueTypeValue<T>&> {
	static T get(LuaGlueBase *g, lua_State *s, int idx);
	static void put(LuaGlueBase *g, lua_State *s, const LuaGlueTypeValue<T> &v);
};

template<int N, typename T>
class LuaGlueStaticArray;

template<size_t _N, typename T>
struct stack<T(&)[_N]> {
	static void getInPlace(LuaGlueBase *g, lua_State *s, int idx, T (&dest)[_N]);
	static T *get(LuaGlueBase *g, lua_State *s, int idx);
	static void put(LuaGlueBase *g, lua_State *s, T (&v)[_N]);
	private:
	static LuaGlueStaticArray<_N, T> *getStaticArray(LuaGlueBase *g, lua_State *s, int idx);
};

template<size_t _N, typename T>
struct stack<T[_N]> {
	static void getInPlace(LuaGlueBase *g, lua_State *s, int idx, T (&dest)[_N]);
	static T *get(LuaGlueBase *g, lua_State *s, int idx);
	static void put(LuaGlueBase *g, lua_State *s, T (&v)[_N]);
	private:
	static LuaGlueStaticArray<_N, T> *getStaticArray(LuaGlueBase *g, lua_State *s, int idx);
};

template<typename T>
struct stack<T, typename std::enable_if<std::is_integral<T>::value>::type> {
	static T get(LuaGlueBase *, lua_State *s, int idx)
	{
		T v = luaL_checkinteger(s, idx);
		LG_Debug("stack<%s>::get: %li", CxxDemangle(T), v);
		return v;
	}
	
	static void put(LuaGlueBase *, lua_State *s, T v)
	{
		LG_Debug("stack<%s>::put: %li", CxxDemangle(T), v);
		lua_pushinteger(s, v);
	}
};

template<typename T>
struct stack<T&, typename std::enable_if<std::is_integral<T>::value>::type> {
	static T get(LuaGlueBase *, lua_State *s, int idx)
	{
		T v = luaL_checkinteger(s, idx);
		LG_Debug("stack<%s>::get: %li", CxxDemangle(T), v);
		return v;
	}
	
	static void put(LuaGlueBase *, lua_State *s, T &v)
	{
		LG_Debug("stack<%s>::put: %li", CxxDemangle(T), v);
		lua_pushinteger(s, v);
	}
};

/*
template<>
struct stack<bool&> {
	static bool get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, bool &v);
};

template<>
struct stack<const bool &> {
	static bool get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, const bool &v);
};

template<>
struct stack<int&> {
	static int get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, int &v);
};

template<>
struct stack<const int&> {
	static int get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, const int &v);
};

template<>
struct stack<int> {
	static int get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, int v);
};

template<>
struct stack<unsigned int> {
	static unsigned int get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, unsigned int v);
};

template<>
struct stack<unsigned int&> {
	static unsigned int get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, unsigned int &v);
};

template<>
struct stack<const unsigned int&> {
	static unsigned int get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, const unsigned int &v);
};

template<>
struct stack<char> {
	static char get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, char v);
};

template<>
struct stack<char&> {
	static char get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, char &v);
};

template<>
struct stack<const char&> {
	static char get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, const char &v);
};
*/

template<>
struct stack<float> {
	static float get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, float v);
};

template<>
struct stack<float&> {
	static float get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, float &v);
};

template<>
struct stack<const float&> {
	static float get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, const float &v);
};

template<>
struct stack<double> {
	static double get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, double v);
};

template<>
struct stack<double&> {
	static double get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, double &v);
};

template<>
struct stack<const double&> {
	static double get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, const double &v);
};

template<class T>
struct stack<T *> {
	static T *get(LuaGlueBase *g, lua_State *s, int idx);
	static T *get(LuaGlueBase *, lua_State *s, int idx, std::true_type);
	static T *get(LuaGlueBase *g, lua_State *s, int idx, std::false_type);
	static void put(LuaGlueBase *g, lua_State *s, T *v);
	static void put(LuaGlueBase *, lua_State *s, T *v, std::true_type);
	static void put(LuaGlueBase *g, lua_State *s, T *v, std::false_type);
};

template<>
struct stack<void *> {
	static void *get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, void *v);
};

template<class T>
struct stack<T *&> {
	static T *get(LuaGlueBase *g, lua_State *s, int idx);
	static T *get(LuaGlueBase *, lua_State *s, int idx, std::true_type);
	static T *get(LuaGlueBase *g, lua_State *s, int idx, std::false_type);
	static void put(LuaGlueBase *g, lua_State *s, T *v);
	static void put(LuaGlueBase *, lua_State *s, T *v, std::true_type);
	static void put(LuaGlueBase *g, lua_State *s, T *v, std::false_type);
};

template<class T>
struct stack<T *const&> {
	static T *get(LuaGlueBase *g, lua_State *s, int idx);
	static void put(LuaGlueBase *g, lua_State *s, T *v);
};

template<typename _Ret, typename... _Args>
class LuaGlueStdFuncWrapper;

#include <functional>

template<typename _Ret, typename... _Args>
struct stack<std::function<_Ret(_Args...)>&> {
	typedef LuaGlueStdFuncWrapper<_Ret, _Args...> FuncWrapperType;
	
	static std::function<_Ret(_Args...)> get(LuaGlueBase *b, lua_State *s, int idx);
	static void put(LuaGlueBase *b, lua_State *, std::function<_Ret(_Args...)> _f);
};

template<typename _Ret, typename... _Args>
struct stack<const std::function<_Ret(_Args...)>&> {
	typedef LuaGlueStdFuncWrapper<_Ret, _Args...> FuncWrapperType;
	
	static std::function<_Ret(_Args...)> get(LuaGlueBase *b, lua_State *s, int idx);
	static void put(LuaGlueBase *b, lua_State *s, std::function<_Ret(_Args...)> _f);
};

template<typename... _Args>
struct stack<std::function<void(_Args...)>&> {
	typedef LuaGlueStdFuncWrapper<void, _Args...> FuncWrapperType;
	
	static std::function<void(_Args...)> get(LuaGlueBase *b, lua_State *s, int idx);
	static void put(LuaGlueBase *b, lua_State *s, std::function<void(_Args...)> _f);
};

template<typename... _Args>
struct stack<const std::function<void(_Args...)>&> {
	typedef LuaGlueStdFuncWrapper<void, _Args...> FuncWrapperType;
	
	static std::function<void(_Args...)> get(LuaGlueBase *b, lua_State *s, int idx);
	static void put(LuaGlueBase *b, lua_State *s, const std::function<void(_Args...)> &_f);
};

template<>
struct stack<const char *&> {
	static const char *get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, const char *v);
};

template<>
struct stack<const char * const &> {
	static const char *get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, const char *v);
};

template<>
struct stack<const char *> {
	static const char *get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, const char *v);
};

template<size_t N>
struct stack<const char (&)[N]> {
	static const char *get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, const char *v);
};

template<>
struct stack<std::string> {
	static std::string get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, std::string v);
	static void put(LuaGlueBase *, lua_State *s, std::string *v);
};

template<>
struct stack<std::string &> {
	static std::string get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, std::string v);
	static void put(LuaGlueBase *, lua_State *s, std::string *v);
};

template<>
struct stack<const std::string &> {
	static std::string get(LuaGlueBase *, lua_State *s, int idx);
	static void put(LuaGlueBase *, lua_State *s, std::string v);
	static void put(LuaGlueBase *, lua_State *s, std::string *v);
};

//#include "LuaGlue/LuaGlueLuaValue.h"
class LuaGlueLuaValue;
template<>
struct stack<LuaGlueLuaValue> {
	static LuaGlueLuaValue get(LuaGlueBase *b, lua_State *s, int idx);
	static void put(LuaGlueBase *b, lua_State *s, LuaGlueLuaValue& v);
};

template<>
struct stack<const LuaGlueLuaValue&> {
	static const LuaGlueLuaValue get(LuaGlueBase *b, lua_State *s, int idx);
	static void put(LuaGlueBase *b, lua_State *s, LuaGlueLuaValue& v);
};

#include "LuaGlue/LuaGlueUtils.h"

template<class T>
class LuaGlueClass;

#include "LuaGlue/StackTemplates/StaticObj.h"

#endif /* LUAGLUE_STACK_TEMPLATES_H_GUARD */



