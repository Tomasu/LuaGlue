#ifndef LUAGLUE_STACKTEMPLATES_LUAVALUE_H_GUARD
#define LUAGLUE_STACKTEMPLATES_LUAVALUE_H_GUARD

#include "LuaGlue/LuaGlueLuaValue.h"
	LuaGlueLuaValue stack<LuaGlueLuaValue>::get(LuaGlueBase *b, lua_State *s, int idx)
	{
		LuaGlueLuaValue object;
		object.load(b, s, idx);
		return object;
	}

	void stack<LuaGlueLuaValue>::put(LuaGlueBase *b, lua_State *s, LuaGlueLuaValue& v)
	{
		v.put(b, s);
	}

	const LuaGlueLuaValue stack<const LuaGlueLuaValue &>::get(LuaGlueBase *b, lua_State *s, int idx)
	{
		LuaGlueLuaValue object;
		object.load(b, s, idx);
		return object;
	}

	void stack<const LuaGlueLuaValue &>::put(LuaGlueBase *b, lua_State *s, LuaGlueLuaValue& v)
	{
		v.put(b, s);
	}

#endif /* LUAGLUE_STACKTEMPLATES_LUAVALUE_H_GUARD */