#include <LuaGlue/LuaGlue.h>

class Foo
{
	public:
		Foo(int i) { printf("ctor! %i\n", i); }
		~Foo();
		
		int abc(int a, int b, int c) { printf("%i:%i:%i\n", a,b,c); return 143; }
		static void aaa() { printf("aaa!\n"); }
		
		void lua_gc() { printf("__gc!\n"); }
};

int main(int, char **)
{
	LuaGlue state;
	
	state.
		Class<Foo>("Foo").
			ctor<int>("new").
			dtor(&Foo::lua_gc).
			method("abc", &Foo::abc).
			method("aaa", &Foo::aaa).
			constants( { { "ONE", 1 }, { "TWO", 2.0 }, { "THREE", "three" } } ).
		end().open().glue();
	
	if(luaL_dofile(state.state(), "foo.lua"))
	{
		printf("failed to dofile: foo.lua\n");
		const char *err = luaL_checkstring(state.state(), -1);
		printf("err: %s\n", err);
	}
		
	return 0;
}
