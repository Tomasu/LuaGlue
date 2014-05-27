function Sandboxed:test_lua()
	io.write(string.format("in Sandboxed::test_lua\n"));
end

function test_lua()
	io.write(string.format("in test_lua\n"));
end

sandboxed = Sandboxed.new();

sandboxed:test_c();
