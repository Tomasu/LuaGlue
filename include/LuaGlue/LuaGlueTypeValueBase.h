#ifndef LUAGLUE_TYPE_VALUE_BASE_H_GUARD
#define LUAGLUE_TYPE_VALUE_BASE_H_GUARD


#ifdef LUAGLUE_TYPECHECK
#	define GetLuaUdata(state, idx, name) ((LuaGlueTypeValueBase *)luaL_checkudata(state, idx, name))
#else
#	define GetLuaUdata(state, idx, name) ((LuaGlueTypeValueBase *)lua_touserdata(state, idx))
#endif

class LuaGlueTypeValueImplBase
{
	public:
		virtual ~LuaGlueTypeValueImplBase() { }
		virtual int put() = 0;
		
		virtual void *vget() = 0;
		virtual void *vptr() = 0;
		
		template<typename T>
		T *get() { return (T *)vget(); }
		
		template<typename T>
		T *ptr() { return (T *)vptr(); }
		
		virtual int ref_cnt() = 0;
};

class LuaGlueTypeValueBase
{
	public:
		LuaGlueTypeValueBase(bool is_shared_ptr_ = false) : is_shared_ptr(is_shared_ptr_) { }
		virtual ~LuaGlueTypeValueBase() { }
		
		virtual void put() = 0;
		virtual LuaGlueTypeValueImplBase *impl() const = 0;
		
		template<typename _T>
		_T *ptr() const { return (_T*)this->impl()->vptr(); }
		
		bool isSharedPtr() { return is_shared_ptr; }
	private:
		bool is_shared_ptr;
};

#endif /* LUAGLUE_TYPE_VALUE_BASE_H_GUARD */
