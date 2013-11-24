#include <LuaGlue/LuaGlue.h>

class Invoke {
	public:
		Invoke() { printf("ctor!\n"); }
		~Invoke() { printf("dtor!\n"); }
		void invoke(int a, double b, const char *c)
		{
			printf("a: %d, b: %f, c: %s\n", a, b, c);
		}
};

int main(int, char **)
{
	LuaGlue state;
	
	auto Class = state.Class<Invoke>("Invoke").
		ctor("new").
		method("invoke", &Invoke::invoke);
		
	state.open().glue();
	
	if(luaL_dofile(state.state(), "invoke.lua"))
	{
		printf("failed to dofile: invoke.lua\n");
		const char *err = luaL_checkstring(state.state(), -1);
		printf("err: %s\n", err);
	}
	
	Invoke *test_obj = new Invoke();
	Class.invokeMethod("invoke", test_obj, 1, 2.0, "three");
	Class.invokeMethod("invoke_lua", test_obj, 2, 3.0, "four");
	
	return 0;
}
