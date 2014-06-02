function lua_intArray_test(intArray)
	print("lua_intArray: "..intArray[0]..", "..intArray[1]..", "..intArray[2]..", "..intArray[3]);
	intArray[0] = 9001;
	intArray[1] = 9002;
	intArray[2] = 9003;
	intArray[3] = 9004;
end

function lua_structArray_test(structArray)
	print("lua_structArray: "..structArray[0].intProp..", "..structArray[1].intProp..", "..structArray[2].intProp);
	structArray[0].intProp = 10001;
	structArray[1].intProp = 10002;
	structArray[2].intProp = 10003;
end

--intArray_test(foo.intArray);
--structArray_test(foo.structArray);

