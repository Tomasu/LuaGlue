local test = Foo.new(333);
test:abc(1,2,3);

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
arr.test();
