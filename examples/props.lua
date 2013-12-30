foo = Foo.new()

print("default direct props:")

print("foo.int: "..foo.d_int)
print("foo.char: "..foo.d_char)
print("foo.sstr: "..foo.d_sstr)
print("foo.objptr: "..foo.d_objptr)
print("foo.objsptr: "..foo.d_objsptr)
print("foo.obj: "..foo.d_obj)

print("default indirect props: ")

print("foo.int: "..foo.int)
print("foo.char: "..foo.char)
print("foo.sstr: "..foo.sstr)
print("foo.objptr: "..foo.objptr)
print("foo.objsptr: "..foo.objsptr)
print("foo.obj: "..foo.obj)

print("set direct props")

foo.d_int = 1337
foo.d_char = 10
foo.d_sstr = "a string"
foo.d_objptr = AProp.new()
foo.d_objsptr = AProp.newShared()

print("get direct props:")

print("foo.int: "..foo.d_int)
print("foo.char: "..foo.d_char)
print("foo.sstr: "..foo.d_sstr)
print("foo.objptr: "..foo.d_objptr)
print("foo.objsptr: "..foo.d_objsptr)
print("foo.obj: "..foo.d_obj)

print("get indirect props: ")

print("foo.int: "..foo.int)
print("foo.char: "..foo.char)
print("foo.sstr: "..foo.sstr)
print("foo.objptr: "..foo.objptr)
print("foo.objsptr: "..foo.objsptr)
print("foo.obj: "..foo.obj)

print("set indirect props: ")

foo.int = 8008135
foo.char = 20
foo.sstr = "b string"
foo.objptr = AProp.new()
foo.objsptr = AProp.newShared()

print("get direct props:")

print("foo.int: "..foo.d_int)
print("foo.char: "..foo.d_char)
print("foo.sstr: "..foo.d_sstr)
print("foo.objptr: "..foo.d_objptr)
print("foo.objsptr: "..foo.d_objsptr)
print("foo.obj: "..foo.d_obj)

print("get indirect props: ")

print("foo.int: "..foo.int)
print("foo.char: "..foo.char)
print("foo.sstr: "..foo.sstr)
print("foo.objptr: "..foo.objptr)
print("foo.objsptr: "..foo.objsptr)
print("foo.obj: "..foo.obj)

print("set custom props")
foo.custom_prop = 123
print("custom_prop: "..foo.custom_prop)
foo.custom_obj = AProp.new()
print("custom_obj: "..foo.custom_obj)

print("done lua script");
