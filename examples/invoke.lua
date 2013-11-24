
function Invoke:invoke_lua (a, b, c)
	io.write(string.format("lua: a: %d, b: %f, c: %s\n", a, b, c))
end

invoke = Invoke.new();
invoke:invoke(1, 2.0, "three");
invoke:invoke_lua(2, 3, "four");