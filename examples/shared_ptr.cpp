#include <LuaGlue/LuaGlue.h>

class Shared {
	public:
		typedef std::shared_ptr<Shared> Ptr;
		
		Shared(int v = 0) : value(v) { printf("in ctor!\n"); }
		~Shared() { printf("in dtor!\n"); }
		
		Ptr getRef() { printf("in getRef!\n"); return Ptr(new Shared()); }
		void putRef(Ptr) { printf("in putRef!\n"); }
	
		int getValue() { printf("in getValue()\n"); return value; }
	private:
		int value;
};

int main(int, char **)
{
	LuaGlue state;
	
	state.
		Class<Shared>("Shared").
			ctor("new").
			method("getRef", &Shared::getRef).
			method("putRef", &Shared::putRef).
			method("getValue", &Shared::getValue).
			end().
		open().glue();
	
	printf("running lua script!\n");
	if(!state.doFile("shared_ptr.lua"))
	{
		printf("failed to dofile: shared_ptr.lua\n");
		printf("err: %s\n", state.lastError().c_str());
	}
	
	Shared *shared = new Shared(123);
	std::shared_ptr<Shared> ptr(shared);

	printf("shared == %p\n", shared);
	state.invokeVoidFunction("test_ptr", ptr);
	state.invokeVoidFunction("test_ptr", ptr);
	state.invokeVoidFunction("test_ptr", ptr);
	
	printf("done!\n");
	
	//lua_gc(state.state(), LUA_GCCOLLECT, 0);

	return 0;
}
