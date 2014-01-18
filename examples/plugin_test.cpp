#include <LuaGlue/LuaGlue.h>
#include <dlfcn.h>
#include <errno.h>
#include <string.h>

#include "LuaPluginBase.h"

int main(int, char **)
{
	LuaGlue *g = nullptr;
	void *mod = nullptr;
	
	g = new LuaGlue;
	
	mod = dlopen("./libplugin.so", RTLD_LAZY);
	if(!mod)
	{
		printf("failed to load libplugin.so: %s\n", dlerror());
		return -1;
	}

	auto create_fn = (LuaPluginCreateFunction)dlsym(mod, "CreatePlugin");
	auto destroy_fn = (LuaPluginDestroyFunction)dlsym(mod, "DestroyPlugin");
	
	LuaPluginBase *plugin = create_fn(g);
	if(!plugin)
	{
		printf("failed to create plugin\n");
		dlclose(mod);
		delete g;
		return -1;
	}
	
	if(!plugin->bind(g))
	{
		printf("failed to bind plugin\n");
		destroy_fn(g, plugin);
		dlclose(mod);
		delete g;
		return -1;
	}

	g->open().glue();
	g->setGlobal("plugin", (LuaPluginBase*)mod);
	
	if(!g->doFile("plugin_test.lua"))
	{
		printf("failed to run plugin_test.lua\n");
		printf("err: %s\n", g->lastError().c_str());
		destroy_fn(g, plugin);
		dlclose(mod);
		delete g;
		return -1;
	}
	
	destroy_fn(g, plugin);
	dlclose(mod);
	delete g;
	
	return 0;
}
