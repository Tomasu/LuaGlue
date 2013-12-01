#ifndef LUAGLUE_CLASS_BASE_H
#define LUAGLUE_CLASS_BASE_H

class LuaGlue;

class LuaGlueClassBase
{
	public:
		virtual bool glue(LuaGlue *) = 0;
		virtual ~LuaGlueClassBase() { }
};

#endif /* LUAGLUE_CLASS_BASE_H */
