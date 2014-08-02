#ifndef LUAGLUE_STACK_TEMPLATES_H_GUARD
#define LUAGLUE_STACK_TEMPLATES_H_GUARD

#include <lua.hpp>
#include <cstdint>
#include <memory.h>

#include "LuaGlue/LuaGlueTypeBase.h"

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
		LG_Debug("checkGlueType: METATABLE_TYPEIDINT_FIELD: %i name: %s", id, c->name().c_str());
		return c != nullptr;
	}
	
	// fallthrough
	return true;
}

template<typename T>
struct stack;

#include "LuaGlue/LuaGlueUtils.h"

#endif /* LUAGLUE_STACK_TEMPLATES_H_GUARD */

#include "LuaGlue/StackTemplates/GlueObj.h"
#include "LuaGlue/StackTemplates/SharedPtr.h"
#include "LuaGlue/StackTemplates/Array.h"
#include "LuaGlue/StackTemplates/Integer.h"
#include "LuaGlue/StackTemplates/Numeric.h"
#include "LuaGlue/StackTemplates/String.h"
#include "LuaGlue/StackTemplates/Ptr.h"
#include "LuaGlue/StackTemplates/StaticObj.h"

