
function test_obj(obj)
	local ir = obj:intret()
	local objptrret = obj:objptrret()
	local objret = obj:objret()
	obj:argint(123)
	obj:argobjptr(objptrret)
	obj:argobj(objret)
end

for i=0,ITERATIONS do
	local base = Base.new()
	local a = A.new()
	local b = B.new()
	local c = C.new()
	local d = D.new()
	
	test_obj(base)
	test_obj(a)
	test_obj(b)
	test_obj(c)
	test_obj(d)
end
