
function Invoke:invoke_lua (a, b, c)
	io.write(string.format("lua: a: %d, b: %f, c: %s\n", a, b, c))
end

function invoke_lua(obj)
	obj:fromlua();
end

function from_c()
	io.write("in lua! called from c!\n");
end

function from_c_ret()
	return 12345;
end

function from_c_args(a, b, c)
	io.write(string.format("from_c_args: a: %d, b: %f, c: %s\n", a, b, c))
	return 4;
end

invoke = Invoke.new();
invoke:invoke(1, 2.0, "three");
invoke:invoke_lua(2, 3, "four");

invoke:invokeObj(invoke);
obj = invoke:objInvoke();