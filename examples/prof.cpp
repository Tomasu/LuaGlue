#include <LuaGlue/LuaGlue.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdarg>
#include <cstring>
#include <cerrno>

static double tv_interval(const struct timeval &tv1, const struct timeval &tv2)
{
	unsigned int usec = tv2.tv_sec * 1000000 + tv2.tv_usec - tv1.tv_sec * 1000000 + tv1.tv_usec;
	
	return (double)usec / 1000000.0;
}

class Base {
	public:
		Base() { }
		virtual ~Base() { }
		virtual int intret() { return 1; }
		virtual Base *objptrret() { return this; }
		Base objret() { return *this; }
		
		virtual void argint(int) { }
		virtual void argobjptr(Base *) { }
		virtual void argobj(Base) { }
};

class A {
	public:
		int intret() { return 1; }
		A *objptrret() { return this; }
		A objret() { return *this; }
		
		void argint(int) { }
		void argobjptr(A *) { }
		void argobj(A) { }
};

class B {
	public:
		int intret() { return 1; }
		B *objptrret() { return this; }
		B objret() { return *this; }
		
		void argint(int) { }
		void argobjptr(B *) { }
		void argobj(B) { }
};

class C {
	public:
		int intret() { return 1; }
		C *objptrret() { return this; }
		C objret() { return *this; }
		
		void argint(int) { }
		void argobjptr(C *) { }
		void argobj(C) { }
};

class D {
	public:
		int intret() { return 1; }
		D *objptrret() { return this; }
		D objret() { return *this; }
		
		void argint(int) { }
		void argobjptr(D *) { }
		void argobj(D) { }
};

void log(const char *format, ...)
{
	va_list ap;
	pid_t pid = getpid();
	char fn[256];
	sprintf(fn, "prof.log.%i", pid);
	
	FILE *fh = fopen(fn, "a");
	if(!fh)
		fprintf(stderr, "failed to open %s for appending: %s", fn, strerror(errno));
	else
	{
		va_start(ap, format);
		vfprintf(fh, format, ap);
		va_end(ap);
	
		fclose(fh);
	}
	
	va_start(ap, format);
	fprintf(stderr, "[%i] ", pid);
	vfprintf(stderr, format, ap);
	va_end(ap);
}

#define DEFAULT_ITERATIONS 500000

int main(int argc, char **argv)
{
	LuaGlue state;
	
	int iterations = DEFAULT_ITERATIONS;
	if(argc > 1)
	{
		iterations = strtol(argv[1], 0, 0);
		if(errno == ERANGE || iterations < 0)
		{
			log("Error: iteration value out of range\n");
			exit(-1);
		}
		
		if(iterations == 0)
			iterations = DEFAULT_ITERATIONS;
	}
	
	log("Running test with %i iterations.\n", iterations);
	
	state.Class<Base>("Base").
		ctor("new").
		method("intret", &Base::intret).
		method("objptrret", &Base::objptrret).
		method("objret", &Base::objret).
		method("argint", &Base::argint).
		method("argobjptr", &Base::argobjptr).
		method("argobj", &Base::argobj).
		constant( "CONSTANT", 123 ).
	end();
	
	state.Class<A>("A").
		ctor("new").
		method("intret", &A::intret).
		method("objptrret", &A::objptrret).
		method("objret", &A::objret).
		method("argint", &A::argint).
		method("argobjptr", &A::argobjptr).
		method("argobj", &A::argobj).
		constant( "CONSTANT", 123 ).
	end();
	
	state.Class<B>("B").
		ctor("new").
		method("intret", &B::intret).
		method("objptrret", &B::objptrret).
		method("objret", &B::objret).
		method("argint", &B::argint).
		method("argobjptr", &B::argobjptr).
		method("argobj", &B::argobj).
		constant( "CONSTANT", 123 ).
	end();
	
	state.Class<C>("C").
		ctor("new").
		method("intret", &C::intret).
		method("objptrret", &C::objptrret).
		method("objret", &C::objret).
		method("argint", &C::argint).
		method("argobjptr", &C::argobjptr).
		method("argobj", &C::argobj).
		constant( "CONSTANT", 123 ).
	end();
	
	state.Class<D>("D").
		ctor("new").
		method("intret", &D::intret).
		method("objptrret", &D::objptrret).
		method("objret", &D::objret).
		method("argint", &D::argint).
		method("argobjptr", &D::argobjptr).
		method("argobj", &D::argobj).
		constant( "CONSTANT", 123 ).
	end();
	
	state.open();
	state.glue();
	
	struct timeval tv_start;
	gettimeofday(&tv_start, 0);
	
	lua_pushinteger(state.state(), iterations);
	lua_setglobal(state.state(), "ITERATIONS");
	
	if(!state.doFile("prof.lua"))
	{
		log("failed to dofile: prof.lua\n");
		printf("err: %s\n", state.lastError().c_str());
	}
	
	struct timeval tv_end;
	gettimeofday(&tv_end, 0);
	
	double elapsed = tv_interval(tv_start, tv_end);
	
	log("Iterations: %i, Time Elapsed: %02.2fs\n", iterations, elapsed);
	
	return 0;
}