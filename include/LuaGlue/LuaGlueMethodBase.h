#ifndef LUAGLUE_METHOD_BASE_H_GUARD
#define LUAGLUE_METHOD_BASE_H_GUARD

class LuaGlue;

class LuaGlueMethodImplBase
{
	public:
		LuaGlueMethodImplBase() { }
		virtual ~LuaGlueMethodImplBase() { }
		virtual bool glue(LuaGlue *luaGlue) = 0;
};

#endif /* LUAGLUE_METHOD_BASE_H_GUARD */
