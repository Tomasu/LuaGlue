#include <LuaGlue/LuaGlue.h>

class Sandboxed {
	public:
		int test_c() { printf("in Sandboxed::test_c\n"); return 1; }
};


int main(int, char **)
{
	LuaGlue state;
	
	auto &Class = state.Class<Sandboxed>("Sandboxed").
		ctor("new").
		method("test_c", &Sandboxed::test_c);
		
	state.open().glue();
	
	printf("running lua script!\n");
	
	if(!state.doFile("Sandboxed", "light_sandbox.lua"))
	{
		printf("failed to dofile: light_sandbox.lua\n");
		printf("err: %s\n", state.lastError().c_str());
		return -1;
	}
	
	printf("testing invoking sandboxed functions\n");
	
	int test_lua_ret = state.invokeFunction<int>("Sandboxed", "test_lua");
	printf("test_lua ret: %i\n", test_lua_ret);
	
	//Sandboxed *sandboxed = new Sandboxed;
	
	//int test_sandboxed_ret = Class.invokeMethod<int>("Sandboxed", "test_lua");
	//printf("test_sandboxed ret: %i\n", test_sandboxed_ret);
	
	return 0;
}
