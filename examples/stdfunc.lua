local A = ClassA:new("in lua")
A:test()
A:testFunc(function(x) print("hello std::function: ".. x) return "called" end)
ret = A:testRetFunc(function(x) print("hello std::function: ".. x) return "called" end)
print("testRetFunc: "..ret)

function testVoidLamda(a)
	print("in lamda: aret="..a())
end

function testLamda(a)
	return "boo"..a
end