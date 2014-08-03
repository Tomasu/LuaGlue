#ifndef LUAGLUE_BASE_H_GUARD
#define LUAGLUE_BASE_H_GUARD

struct lua_State;
class LuaGlueTypeBase;
#include "LuaGlue/LuaGlueSymTab.h"
//template<typename T> class LuaGlueSymTab;

class LuaGlueBase
{
	public:
		virtual ~LuaGlueBase() {}
		virtual lua_State *state() = 0;
		virtual bool glue() = 0;
		virtual void addType(LuaGlueTypeBase *type) = 0;
		virtual bool typeExists(const char *, bool = false) = 0;
		virtual LuaGlueTypeBase *lookupType(const char *name, bool internal_name = false) = 0;
		virtual LuaGlueTypeBase *lookupType(uint32_t idx) = 0;
		virtual LuaGlueSymTab<LuaGlueTypeBase *> &getSymTab() = 0;
};

#endif /* LUAGLUE_BASE_H_GUARD */
