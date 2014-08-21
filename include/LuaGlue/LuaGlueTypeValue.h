#ifndef LUAGLUE_TYPE_VALUE_H_GUARD
#define LUAGLUE_TYPE_VALUE_H_GUARD

#include <memory>
#include <atomic>
#include <stdexcept>

#include "LuaGlue/LuaGlueDebug.h"
#include "LuaGlue/LuaGlueTypeValueBase.h"

#include "LuaGlue/LuaGlueTypeBase.h"

#define CastLuaGlueTypeValue(ClassType, o) ((LuaGlueTypeValue<ClassType> *)o)
#define CastLuaGlueTypeValueShared(ClassType, o) ( (LuaGlueTypeValue<std::shared_ptr<ClassType>> *)o )

template<class _Class>
class LuaGlueTypeValueImpl : public virtual LuaGlueTypeValueImplBase
{
	public:
		typedef _Class Type;
		LuaGlueTypeValueImpl(Type *p, LuaGlueTypeBase *type, bool owner = false) : _ref_cnt(1), _type(type), _ptr(p), _owner(owner)
		{
			// NOTE: std::atomic_init does not compile in GCC
			// something else will be needed for MSVC
			// std::atomic_init(&_ref_cnt, 1);
			
			//LG_Debug("ctor");
		}
		
		~LuaGlueTypeValueImpl()
		{
			LG_Debug("dtor %p", _ptr);
			if(_type) _type->impl_dtor(_ptr); 
			if(_owner)
			{
				LG_Debug("we're owner, delete");
				delete _ptr;
			}
			
			_ref_cnt = 0;
			_type = 0;
			_ptr = 0;
			_owner = false;
			LG_Debug("dtor end");
		}
		
		void *vget()
		{
			_ref_cnt++;
			//LG_Debug("inc: %i", _ref_cnt.load());
			return _ptr;
		}
		
		int put()
		{
			_ref_cnt--;
			//LG_Debug("dec: %i", _ref_cnt.load());
			return _ref_cnt;
		}
		
		//Type &ref() { return *_ptr; }
		void *vptr() { return _ptr; }
		
		int ref_cnt() { return _ref_cnt; }
	private:
		std::atomic_int _ref_cnt;
		LuaGlueTypeBase *_type;
		Type *_ptr;
		bool _owner;
};

template<class _Class>
class LuaGlueTypeValueImpl<std::shared_ptr<_Class>> : public virtual LuaGlueTypeValueImplBase
{
	public:
		typedef std::shared_ptr<_Class> Type;
		typedef _Class ClassType;
		LuaGlueTypeValueImpl(Type *p, LuaGlueTypeBase *type, bool owner = false) : _ref_cnt(1), _type(type), _ptr(p), _owner(owner)
		{
         // NOTE: std::atomic_init does not compile in GCC
			// something else will be needed for MSVC
			// std::atomic_init(&_ref_cnt, 1);
			
			LG_Debug("ctor");
		}
		
		~LuaGlueTypeValueImpl()
		{
			LG_Debug("dtor %p", _ptr);
			if(_type) _type->impl_dtor(_ptr); 
			if(_owner)
			{
				LG_Debug("we're owner, delete");
				delete _ptr;
			}
			
			_ref_cnt = 0;
			_type = 0;
			_ptr = 0;
			_owner = false;
		}
		
		void *vget()
		{
			_ref_cnt++;
			return _ptr;
		}
		
		int put()
		{
			_ref_cnt--;
			return _ref_cnt;
		}
		
		//Type &ref() { return *_ptr; }
		void *vptr() { return _ptr; }
		
		int ref_cnt() { return _ref_cnt; }
	private:
		std::atomic_int _ref_cnt;
		LuaGlueTypeBase *_type;
		Type *_ptr;
		bool _owner;
};

template<class _Class>
class LuaGlueTypeValue : public LuaGlueTypeValueBase
{
	public:
		typedef _Class Type;
		
		LuaGlueTypeValue() : LuaGlueTypeValueBase(false), p(0)
		{
			//LG_Debug("ctor()");
		}
		
		// coverity[+alloc]
		LuaGlueTypeValue(Type *optr, LuaGlueTypeBase *type = nullptr, bool owner = false) : LuaGlueTypeValueBase(false), p(new LuaGlueTypeValueImpl<Type>(optr, type, owner))
		{
			//LG_Debug("ctor(%p, %s, %i)", ptr, clss->name().c_str(), owner);
		}
		
		LuaGlueTypeValue(const LuaGlueTypeValueBase *rhs) : LuaGlueTypeValueBase(false), p(rhs->impl())
		{
			//LG_Debug("ctor(LuaGlueTypeValueBase(%p))", p->vptr());
			(void)p->vget();
		}
		
		LuaGlueTypeValue(const LuaGlueTypeValue &rhs) : LuaGlueTypeValueBase(false), p(rhs.p)
		{
			//LG_Debug("copy ctor(%p)", p->vptr());
			(void)p->vget();
		}
		
		LuaGlueTypeValue &operator=( const LuaGlueTypeValue &rhs )
		{
			//LG_Debug("assign(%p)", rhs.p->vptr());
			p = rhs.p;
			(void)p->vget();
		}
		
		~LuaGlueTypeValue()
		{
			LG_Debug("dtor");
			//abort();
			
			if(!p)
				LG_Debug("p == 0");
			
			if(p && !p->put())
			{
				LG_Debug("dtor ref count hit 0, delete impl");
				delete p;
			}
		}
		
		void put()
		{
			if(!p)
				throw new std::runtime_error("blah");
			
			if(!p->put())
			{
				LG_Debug("put ref count hit 0, delete impl");
				delete p;
			}
		}
		
		Type &operator*() const { return *p->ptr<Type>(); }
		Type *operator->() const { return p->ptr<Type>(); }
		
		Type *ptr() const { return p->ptr<Type>(); }
		
		LuaGlueTypeValueImplBase *impl() const { return p; }
	private:
		LuaGlueTypeValueImplBase *p;
};

template<class _Class>
class LuaGlueTypeValue<std::shared_ptr<_Class>> : public LuaGlueTypeValueBase
{
	public:
		typedef _Class ClassType;
		typedef std::shared_ptr<_Class> Type;
		
		LuaGlueTypeValue() : LuaGlueTypeValueBase(true), p(0)
		{
			LG_Debug("ctor()");
		}
		
		//LuaGlueTypeValue(Type *optr, LuaGlueClass<ClassType> *clss = nullptr, bool owner = false) : LuaGlueTypeValueBase(true), p(new LuaGlueTypeValueImpl<Type>(optr, clss, owner))
		//{
		//	//LG_Debug("ctor(%p, %s, %i)", ptr, clss->name().c_str(), owner);
		//}
		
		LuaGlueTypeValue(Type *&optr, LuaGlueTypeBase *type = nullptr, bool owner = false) : LuaGlueTypeValueBase(true), p(new LuaGlueTypeValueImpl<Type>(optr, type, owner))
		{
			LG_Debug("ctor(%p, %s, %i)", optr, type->name().c_str(), owner);
		}
		
		LuaGlueTypeValue(const LuaGlueTypeValueBase *rhs) : LuaGlueTypeValueBase(true), p(rhs->impl())
		{
			LG_Debug("ctor(LuaGlueTypeValueBase(%p)) ref_cnt:%i", p->vptr(), p->ref_cnt());
			(void)p->vget();
		}
		
		LuaGlueTypeValue(const LuaGlueTypeValue &rhs) : LuaGlueTypeValueBase(true), p(rhs.p)
		{
			LG_Debug("copy ctor(%p) ref_cnt:%i", p->vptr(), p->ref_cnt());
			(void)p->vget();
		}
		
		LuaGlueTypeValue &operator=( const LuaGlueTypeValue &rhs )
		{
			LG_Debug("assign(%p) ref_cnt:%i", rhs.p->vptr(), rhs.p->ref_cnt());
			p = rhs.p;
			(void)p->vget();
		}
		
		~LuaGlueTypeValue()
		{
			if(p)
			{
				LG_Debug("dtor ref_cnt:%i", p->ref_cnt());
				LG_Debug("ptr: %p", p->vptr());
				
				if(!p->put())
				{
					LG_Debug("dtor ref count hit 0, delete impl");
					delete p;
					p = nullptr;
				}
			}
			else
			{
				LG_Debug("dtor ref_cnt:-1");
				LG_Debug("ptr: nullptr");
			}
	
			p = nullptr;
		}
		
		void put()
		{
			if(!p)
				throw new std::runtime_error("blah");
			
			if(!p->put())
			{
				LG_Debug("put ref count hit 0, delete impl");
				delete p;
				p = nullptr;
			}
		}
		
		Type operator*() const { LG_Debug("operator*"); return *p->ptr<Type>(); }
		Type *operator->() const { return p->ptr<Type>(); }
		
		ClassType *ptr() const { return (*(p->ptr<Type>())).get(); }
		
		LuaGlueTypeValueImplBase *impl() const { return p; }
	private:
		LuaGlueTypeValueImplBase *p;
};

#include "LuaGlue/StackTemplates/TypeValue.h"

#endif /* LUAGLUE_TYPE_VALUE_H_GUARD */
