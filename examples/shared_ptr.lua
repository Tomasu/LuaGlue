function printf (fmt, ...)
	if arg then
		return io.write(string.format(fmt .. "\n", unpack(arg)))
	else
		return io.write(fmt .. "\n");
	end
end

--for i=0,10000 do

printf("before Shared.new()");
shared = Shared.new();
printf("before getRef");
ref = shared:getRef();
printf("before shared:putRef");
shared:putRef(ref);
printf("before ref:putRef");
ref:putRef(ref);
printf("before shared:getRef");
shared:getRef();
printf("before ref=0");
ref = 0;
--end