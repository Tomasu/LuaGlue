#include <LuaGlue/LuaGlue.h>

bool register_mfile1(LuaGlue &g);
bool register_mfile2(LuaGlue &g);

int main(int, char **)
{
	LuaGlue g;
	
	register_mfile1(g);
	register_mfile2(g);
	
	g.open().glue();
	
	return 0;
}
