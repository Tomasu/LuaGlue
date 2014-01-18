#include <LuaGlue/LuaGlue.h>
#include "LuaPluginBase.h"

class Foo
{
	public:
		Foo(int n) { printf("Foo::Foo: %i\n", n); }
		bool test(float frob) { printf("Foo::test: %f\n", frob); return true; }
};

class MyPlugin : public LuaPluginBase
{
	public:
		MyPlugin()
		{
			printf("MyPlugin::MyPlugin!\n");
		}
		
		~MyPlugin()
		{
			printf("MyPlugin::~MyPlugin\n");
		}
		
		bool bind(LuaGlueBase *g_)
		{
			LuaGlue *g = (LuaGlue*)g_;
			printf("MyPlugin::bind!\n");
			g->Class<Foo>("Foo").
				ctor<int>("new").
				method("test", &Foo::test);
				
			return true;
		}
};

extern "C" LuaPluginBase *CreatePlugin(LuaGlue *);
LuaPluginBase *CreatePlugin(LuaGlue *)
{
	return new MyPlugin();
}

extern "C" void DestroyPlugin(LuaGlue *, MyPlugin *plugin);

void DestroyPlugin(LuaGlue *, MyPlugin *plugin)
{
	delete plugin;
}
