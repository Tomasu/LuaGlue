#include <LuaGlue/LuaGlue.h>

static void func()
{
	printf("in func!\n");
}

class Array
{
	public:
		Array() { printf("ctor!\n"); }
		
		void newindex(int key, int val)
		{
			printf("key: %i val: %i\n", key, val);
			arr[key] = val;
		}
		
		int index(int key)
		{
			printf("key: %i len: %lu\n", key, arr.size());
			return arr[key];
		}
		
		void test() { printf("Array.test!\n"); }
		
	private:
		std::map<int, int> arr;
};

class Foo
{
	public:
		Foo(int i) { printf("ctor! %i\n", i); }
		~Foo() { };
		
		int abc(int a, int b, int c) { printf("%i:%i:%i\n", a,b,c); return 143; }
		static void aaa() { printf("aaa!\n"); }
		
		void ptrArgTest(Foo *foo) { printf("ptrArgTest: %p abc:%i\n", foo, foo->abc(4,4,4)); }
		Foo *ptrTest() { return this; }
		
		void lua_gc() { printf("__gc!\n"); }
		
		void constmethod() const { printf("constmethod!\n"); }
		int constretmethod() const { return 123; }
		
		int testProp() { return testProp_; }
		void setTestProp(int v) { testProp_ = v; }
	private:
		
		int testProp_;
};

struct STestA
{
	int a;
};

struct STestB
{
	STestB() {printf("STestB()\n");}
	STestA a;
	int b;
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
			method("ptrTest", &Foo::ptrTest).
			method("ptrArgTest", &Foo::ptrArgTest).
			method("constmethod", &Foo::constmethod).
			method("constretmethod", &Foo::constretmethod).
			property("testProp", &Foo::testProp, &Foo::setTestProp).
			constants( { { "ONE", 1 }, { "TWO", 2.12 }, { "THREE", "three" } } ).
		end();
	state.
		Class<Array>("Array").
			ctor("new").
			index(&Array::index).
			newindex(&Array::newindex).
			method("test", &Array::test).
		end();
	
	state.
		Class<STestA>("STestA").
			prop("a", &STestA::a).
		end().
		Class<STestB>("STestB").
			ctor("new").
			prop("a", &STestB::a).
			prop("b", &STestB::b).
		end().
		func("func", &func).
		open().glue();
		
	if(!state.doFile("foo.lua"))
	{
		printf("failed to dofile: foo.lua\n");
		printf("err: %s\n", state.lastError().c_str());
	}
		
	return 0;
}
