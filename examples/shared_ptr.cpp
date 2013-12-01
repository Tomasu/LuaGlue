#include <LuaGlue/LuaGlue.h>

class Shared {
	public:
		typedef std::shared_ptr<Shared> Ptr;
		
		Shared() { printf("ctor!\n"); }
		~Shared() { printf("dtor!\n"); }
		
		Ptr getRef() { printf("getRef!\n"); return Ptr(this); }
		void putRef(Ptr) { printf("putRef!\n"); }
	
};

int main(int, char **)
{
	LuaGlue state;
	
	state.
		Class<Shared>("Shared").
			ctor("new").
			method("getRef", &Shared::getRef).
			method("putRef", &Shared::putRef).
			end().
		open().glue();
	
	printf("running lua script!\n");
	if(luaL_dofile(state.state(), "shared_ptr.lua"))
	{
		printf("failed to dofile: shared_ptr.lua\n");
		const char *err = luaL_checkstring(state.state(), -1);
		printf("err: %s\n", err);
	}
		
	return 0;
}
