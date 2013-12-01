#include <LuaGlue/LuaGlue.h>

class Shared {
	public:
		typedef std::shared_ptr<Shared> Ptr;
		
		Shared() { printf("in ctor!\n"); }
		~Shared() { printf("in dtor!\n"); }
		
		Ptr getRef() { printf("in getRef!\n"); return Ptr(new Shared()); }
		void putRef(Ptr) { printf("in putRef!\n"); }
	
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
	
	printf("done!\n");
	
	lua_gc(state.state(), LUA_GCCOLLECT, 0);

	return 0;
}
