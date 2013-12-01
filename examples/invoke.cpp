#include <LuaGlue/LuaGlue.h>

class Invoke {
	public:
		Invoke() { printf("ctor!\n"); }
		~Invoke() { printf("dtor!\n"); }
		void invoke(int a, double b, const char *c)
		{
			printf("a: %d, b: %f, c: %s\n", a, b, c);
		}
		void invokeObj(Invoke *obj) { printf("obj: %p\n", obj); }
		Invoke *objInvoke(void) { printf("return this\n"); return this; }
		void fromlua() { printf("from lua!\n"); }
};

int main(int, char **)
{
	LuaGlue state;
	
	auto Class = state.Class<Invoke>("Invoke").
		ctor("new").
		method("invoke", &Invoke::invoke).
		method("invokeObj", &Invoke::invokeObj).
		method("objInvoke", &Invoke::objInvoke).
		method("fromlua", &Invoke::fromlua);
		
	state.open().glue();
	
	printf("running lua script!\n");
	if(luaL_dofile(state.state(), "invoke.lua"))
	{
		printf("failed to dofile: invoke.lua\n");
		const char *err = luaL_checkstring(state.state(), -1);
		printf("err: %s\n", err);
	}
	
	printf("testing invoking methods from C++\n");
	Invoke *test_obj = new Invoke();
	Class.invokeVoidMethod("invoke", test_obj, 1, 2.0, "three");
	Class.invokeVoidMethod("invoke_lua", test_obj, 2, 3.0, "four");
	Class.invokeVoidMethod("invokeObj", test_obj, test_obj);
	Invoke *ret_obj = Class.invokeMethod<Invoke *>("objInvoke", test_obj);
	
	if(ret_obj != test_obj)
	{
		printf("ret_obj != test_obj ! :o\n");
	}
	
	//state.invokeVoidMethod("invoke_lua", test_obj);
	
	return 0;
}
