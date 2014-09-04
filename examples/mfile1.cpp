#include <LuaGlue/LuaGlue.h>

class MFile1
{
	public:
		MFile1() { printf("ctor!\n"); }
};

bool register_mfile1(LuaGlue &g);
bool register_mfile1(LuaGlue &g)
{
	g.Class<MFile1>("MFile1").
		ctor("new");
		
	return true;
}
