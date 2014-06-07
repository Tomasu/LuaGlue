#include <LuaGlue/LuaGlue.h>
#include <memory>

class AProp
{
	public:
		typedef std::shared_ptr<AProp> SPtr;
		
		static SPtr newShared() { return SPtr(new AProp()) ; }
};

class Foo
{
	public:
		typedef std::shared_ptr<Foo> SPtr;
		
		Foo(int iprop = -1, char cprop = -1, const std::string &ssprop = "init", AProp *opprop = nullptr, AProp oprop = AProp()) :
			int_prop(iprop), char_prop(cprop), stdstr_prop(ssprop), obj_ptr_prop(opprop ? opprop : new AProp()), obj_sptr_prop(new AProp()),
			obj_prop(oprop)
		{
			// empty
			LG_Debug("Foo::Foo(...) opprop:%p", obj_ptr_prop);
		}
		
		~Foo()
		{
			LG_Debug("Foo::~Foo()");
		}
		
		int getInt() { return int_prop; }
		void setInt(int v) { int_prop = v; }
		
		char getChar() { return char_prop; }
		void setChar(char v) { char_prop = v; }
		
		const std::string &getStdString() { return stdstr_prop; }
		void setStdString(const std::string &v) { stdstr_prop = v; }
		
		AProp *getObjPtr() const { return obj_ptr_prop; }
		void setObjPtr(AProp *v) { obj_ptr_prop = v; }
		
		AProp::SPtr getObjSPtr() { return obj_sptr_prop; }
		void setObjSPtr(AProp::SPtr v) { obj_sptr_prop = v; }
		
		AProp getObj() { return obj_prop; }
		void setObj(AProp v) { obj_prop = v; }
		
	//private:
		int int_prop;
		char char_prop;
		std::string stdstr_prop;
		AProp *obj_ptr_prop;
		std::shared_ptr<AProp> obj_sptr_prop;
		AProp obj_prop;
};

int main(int, char **)
{
	LuaGlue g;
	
	g.Class<Foo>("Foo").
		ctor("new").
		property("d_int", &Foo::int_prop).
		property("int", &Foo::getInt, &Foo::setInt).
		property("d_char", &Foo::char_prop).
		property("char", &Foo::getChar, &Foo::setChar).
		property("d_sstr", &Foo::stdstr_prop).
		property("sstr", &Foo::getStdString, &Foo::setStdString).
		property("d_objptr", &Foo::obj_ptr_prop).
		property("objptr", &Foo::getObjPtr, &Foo::setObjPtr).
		property("d_objsptr", &Foo::obj_sptr_prop).
		property("objsptr", &Foo::getObjSPtr, &Foo::setObjSPtr).
		property("d_obj", &Foo::obj_prop).
		property("obj", &Foo::getObj, &Foo::setObj);
		
	g.Class<AProp>("AProp").
		ctor("new").
		method("newShared", &AProp::newShared);
		
	g.open().glue();
	
	printf("run lua script\n");
	
	if(!g.doFile("props.lua"))
	{
		printf("failed to dofile: props.lua\n");
		printf("err: %s\n", g.lastError().c_str());
	}
	
	Foo *foo = g.getGlobal<Foo *>("foo");
	auto foo_class = g.getClass<Foo>("Foo");
	printf("get global 'foo': %p\n", foo);
	
	AProp *ptr = foo->obj_ptr_prop;
	printf("foo->obj_ptr_prop == %p\n", ptr);

	ptr = foo_class->getProperty<AProp *>("d_objptr", foo);
	printf("get foo[d_objptr] == %p\n", ptr);
	
	printf("get custom props:\n");
	int cprop = foo_class->getProperty<int>("custom_prop", foo);
	printf("custom_prop: %i\n", cprop);
	
	ptr = foo_class->getProperty<AProp *>("custom_obj", foo);
	printf("custom_obj: %p\n", ptr);
	
	cprop = 1337;
	ptr = new AProp();
	
	printf("set custom props: %i %p\n", cprop, ptr);
	
	foo_class->setProperty("custom_prop", foo, cprop);
	foo_class->setProperty("custom_obj", foo, ptr);
	
	printf("get custom props:\n");
	cprop = foo_class->getProperty<int>("custom_prop", foo);
	printf("custom_prop: %i\n", cprop);
	
	ptr = foo_class->getProperty<AProp *>("custom_obj", foo);
	printf("custom_obj: %p\n", ptr);
	
	cprop = 8008135;
	ptr = new AProp();
	
	printf("set custom props 2: %i %p\n", cprop, ptr);
	
	foo_class->setProperty("custom_prop2", foo, cprop);
	foo_class->setProperty("custom_obj2", foo, ptr);
	
	printf("get custom props 2:\n");
	cprop = foo_class->getProperty<int>("custom_prop2", foo);
	printf("custom_prop2: %i\n", cprop);
	
	ptr = foo_class->getProperty<AProp *>("custom_obj2", foo);
	printf("custom_obj2: %p\n", ptr);
	
	return 0;
}
