#ifndef LUAPLUGIN_BASE_H_GUARD
#define LUAPLUGIN_BASE_H_GUARD

class LuaGlueBase;

class LuaPluginBase
{
	public:
		LuaPluginBase() { }
		virtual ~LuaPluginBase() { }
		virtual bool bind(LuaGlueBase *) = 0;
};

typedef LuaPluginBase *(*LuaPluginCreateFunction)(LuaGlueBase *);
typedef void (*LuaPluginDestroyFunction)(LuaGlue*, LuaPluginBase*);

#endif /* LUAPLUGIN_BASE_H_GUARD */
