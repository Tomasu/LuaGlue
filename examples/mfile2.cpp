#include <LuaGlue/LuaGlue.h>

class MFile2
{
	public:
		MFile2() { printf("mfile2 ctor!\n"); }
};

bool register_mfile2(LuaGlue &g);
bool register_mfile2(LuaGlue &g)
{
	g.Class<MFile2>("MFile2").
		ctor("new");
		
	return true;
}