#include <LuaGlue/LuaGlue.h>

class Invoke {
	public:
		int abc;
		Invoke() { printf("%p ctor!\n", this); }
		~Invoke() { printf("dtor!\n"); }
		void invoke(int a, double b, const char *c)
		{
			printf("a: %d, b: %f, c: %s\n", a, b, c);
		}
		void invokeObj(Invoke *obj) { printf("%p invokeObj: %p\n", this, obj); }
		Invoke *objInvoke(void) { printf("return this\n"); return this; }
		void fromlua() { printf("from lua!\n"); }
};

int main(int, char **)
{
	LuaGlue state;
	
	auto &Class = state.Class<Invoke>("Invoke").
		ctor("new").
		method("invoke", &Invoke::invoke).
		method("invokeObj", &Invoke::invokeObj).
		method("objInvoke", &Invoke::objInvoke).
		method("fromlua", &Invoke::fromlua);
		
	state.open().glue();
	
	printf("running lua script!\n");
	if(!state.doFile("invoke.lua"))
	{
		printf("failed to dofile: invoke.lua\n");
		printf("err: %s\n", state.lastError().c_str());
	}
	
	printf("testing invoking methods from C++\n");
	Invoke *test_obj = new Invoke();
	test_obj->abc = 123;
	
	Class.invokeVoidMethod("invoke", test_obj, 1, 2.0, "three");
	Class.invokeVoidMethod("invoke_lua", test_obj, 2, 3.0, "four");
	Class.invokeVoidMethod("invokeObj", test_obj, test_obj);
	Invoke *ret_obj = Class.invokeMethod<Invoke *>("objInvoke", test_obj);
	
	if(ret_obj != test_obj)
	{
		printf("ret_obj(%p) != test_obj(%p) ! :o\n", ret_obj, test_obj);
	}

	printf("test_obj.abc: %i, ret_obj.abc: %i\n", test_obj->abc, ret_obj->abc);
	
	state.invokeVoidFunction("from_c");
	int ret = state.invokeFunction<int>("from_c_ret");
	printf("from_c_ret ret: %i\n", ret);
	
	int ret2 = state.invokeFunction<int>("from_c_args", 1, 2.0, "three");
	printf("from_c_args ret: %i\n", ret2);
	
	return 0;
}
