function lua_func()
	io.write(string.format("in lua_func\n"));
end

local valuetest = ValueTest.new();

local table = {}
table["tabletest"] = 123;

valuetest:tableTest(table);
valuetest:funcTest(lua_func);
valuetest:valueTest(123.456);
valuetest:valueTest(9001);
valuetest:valueTest("a string");