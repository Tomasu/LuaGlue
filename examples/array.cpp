#include <LuaGlue/LuaGlue.h>

struct Foo
{
	int intArray[4];
	struct {
		int intProp;
	} structArray[3];
};

typedef std::remove_reference<decltype(Foo::structArray[0])>::type FooStruct;

/*
static void intArray_test(int foo[4])
{
	printf("intArray: %i, %i, %i, %i\n", foo[0], foo[1], foo[2], foo[3]);
}

static void structArray_test(FooStruct foo[3])
{
	printf("intProp: %i, %i, %i\n", foo[0].intProp, foo[1].intProp, foo[2].intProp);
}
*/

int main(int, char **)
{
	LuaGlue g;
	
	g.Class<Foo>("Foo").
		ctor("new").
		property<4, int>("intArray", &Foo::intArray).
		property("structArray", &Foo::structArray);
		
	g.Class< std::remove_reference<decltype(Foo::structArray[0])>::type >("FooStruct")
		.property("intProp", &std::remove_reference<decltype(Foo::structArray[0])>::type::intProp);
	
	//g.func("intArray_test", intArray_test);
	//g.func("structArray_test", structArray_test);
	
	g.open().glue();
	//printf("array typename: %s\n", typeid(decltype(Foo::structArray[0])).name());
	
	Foo *foo = new Foo;
	foo->intArray[0] = 10;
	foo->intArray[1] = 20;
	foo->intArray[2] = 30;
	foo->intArray[3] = 40;
	
	foo->structArray[0].intProp = 100;
	foo->structArray[1].intProp = 200;
	foo->structArray[2].intProp = 300;

	printf("lua to c test:\n");
	
	g.setGlobal("foo", foo);
	
	if(!g.doFile("array.lua"))
	{
		printf("failed to dofile: array.lua\n");
		printf("err: %s\n", g.lastError().c_str());
	}
	
	printf("c to lua test:\n");
	
	printf("\nlua_intArray_test:\n");
	g.invokeVoidFunction("lua_intArray_test", (foo->intArray));
	
	printf("\nlua_structArray_test:\n");
	g.invokeVoidFunction("lua_structArray_test", foo->structArray);
	
	printf("intArray: %i, %i, %i, %i\n", foo->intArray[0], foo->intArray[1], foo->intArray[2], foo->intArray[3]);
	printf("structArray.intProp: %i, %i, %i\n", foo->structArray[0].intProp, foo->structArray[1].intProp, foo->structArray[2].intProp);

	printf("done!\n");
	
	return 0;
}
