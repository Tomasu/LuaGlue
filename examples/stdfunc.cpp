#include <LuaGlue/LuaGlue.h>
#include <iostream>

class ClassA {
	public:
		ClassA()
		{
			m_str = "default";
		}

		ClassA(std::string str)
		{
			m_str = str;
		}

		~ClassA()
		{
		}

		bool test()
		{
			std::cout << "ClassA->test() : " << m_str << std::endl;
			return true;
		}

		void testFunc(std::function<void(int)> f)
		{
			std::cout << "[testFunc]" << std::endl;
			f(5);
		}
		
		std::string testRetFunc(std::function<std::string(int)> f)
		{
			std::cout << "[testRetFunc]" << std::endl;
			return f(5);
		}

	private:
		std::string m_str;
};

int main(int, char **)
{
	LuaGlue state;

	state.
		Class<ClassA>("ClassA").
			ctor<std::string>("new").
			method("test", &ClassA::test).
			method("testFunc", &ClassA::testFunc).
			method("testRetFunc", &ClassA::testRetFunc).
		end();

	state.open().glue();

	if(!state.doFile("stdfunc.lua"))
		std::cout << "failed to dofile [stdfunc.lua]: " << state.lastError() << std::endl;

	state.invokeVoidFunction("testVoidLamda", std::function<std::string()>([]{ return "abc"; }));
	auto ret = state.invokeFunction<std::string>("testLamda", "abc");
	std::cout << "got: " << ret << std::endl;
	
	return 0;
}