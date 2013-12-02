Foo.aaa();
local test = Foo.new(333);
test:abc(1,2,3);
--test:aaa();
test:constmethod();
ret = test:constretmethod();
print("constretmethod: "..ret);

test.testProp = 123;
print("testProp: "..test.testProp);

func()

print("ONE: "..Foo.ONE);
print("TWO: "..Foo.TWO);
print("THREE: "..Foo.THREE);

local ptrtest = test:ptrTest();

test:ptrArgTest(test);

test = nil;
collectgarbage();

local arr = Array.new();
arr[0] = 123;
print("arr[0]: "..arr[0]);
arr:test();

local testb = STestB.new();
print("set testb.b");
testb.b = 123;
print("testb.b: "..testb.b);
print("set testb.a.a");
testb.a.a = 456;
print("testb.a.a: "..testb.a.a);
