function printf (fmt, ...)
	if arg then
		return io.write(string.format(fmt .. "\n", unpack(arg)))
	else
		return io.write(fmt .. "\n");
	end
end

-- shared_ptr.lua
function test_ptr(ptr)
	print("test_ptr")
	print(ptr:getValue())
end

--for i=0,10000 do

--shared = Shared.new();
--ref = shared:getRef();
--shared:putRef(ref);
--ref:putRef(ref);
--shared:getRef();
--ref = 0;
--end