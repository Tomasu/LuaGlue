--for i=0,10000 do
shared = Shared.new();
ref = shared:getRef();
shared:putRef(ref);
ref:putRef(ref);
shared:getRef();
ref = 0;
--end