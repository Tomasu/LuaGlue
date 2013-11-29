#include <LuaGlue/LuaGlue.h>
#include <memory>
#include <string>

class test
{
	public:
		void method(const std::string &s);
};

int main(int, char **)
{
	LuaGlue state;

	state
		.Class<test>("test")
				.method("method", &test::method)
		.end()
		.open()
		.glue();
		
	return 0;
}