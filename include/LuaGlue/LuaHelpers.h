#ifndef LUAGLUE_LUAHELPERS_H_GUARD
#define LUAGLUE_LUAHELPERS_H_GUARD

#include "LuaGlue/LuaGlueBase.h"
#include "LuaGlue/LuaGlueStackTemplates.h"

class LuaHelpers
{
	public:
		template<class _Class>
		static bool glueFunction(LuaGlueBase *g, const std::string &name, _Class *ctx, int (*fn)(lua_State *), int idx = -2)
		{
			lua_pushlightuserdata(g->state(), ctx);
			lua_pushcclosure(g->state(), fn, 1);
			lua_setfield(g->state(), idx, name.c_str());
			return true;
		}
		
		template<typename _Value>
		static bool setField(LuaGlueBase *g, const std::string &name, _Value v, int idx = -2)
		{
			stack<_Value>::put(g, g->state(), v);
			lua_setfield(g->state(), idx, name.c_str());
			return true;
		}
};

#endif /* LUAGLUE_LUAHELPERS_H_GUARD */
