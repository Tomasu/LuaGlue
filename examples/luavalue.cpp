#include <LuaGlue/LuaGlue.h>

LuaGlue g;

class ValueTest
{
	public:
		ValueTest()
		{
			LG_Debug("new: %p", this);
		}
		
		void tableTest(LuaGlueLuaValue value)
		{
			LuaGlueLuaTable *table = value.getTable();
			table->_dump();
		}
		
		void funcTest(LuaGlueLuaValue value)
		{
			LuaGlueLuaFunction *func = value.getFunction();
			func->invokeMulti();
		}
		
		void valueTest(LuaGlueLuaValue value)
		{
			LG_Debug("value: %s", value.toString().c_str());
		}
};

int main(int, char **)
{
	g.Class<ValueTest>("ValueTest").
		ctor("new").
		method("tableTest", &ValueTest::tableTest).
		method("funcTest", &ValueTest::funcTest).
		method("valueTest", &ValueTest::valueTest);
		
	g.open().glue();
	
	if(!g.doFile("luavalue.lua"))
	{
		printf("failed to dofile: luavalue.lua\n");
		printf("err: %s\n", g.lastError().c_str());
	}
	
	printf("done!\n");
		
	return 0;
}