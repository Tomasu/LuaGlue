#ifndef LUAGLUE_OBJECT_BASE_H_GUARD
#define LUAGLUE_OBJECT_BASE_H_GUARD

#include <atomic>
#include <exception>

#include "LuaGlue/LuaGlueDebug.h"

template<class _Class>
class LuaGlueClass;

template<class _Class>
class LuaGlueObjectImpl
{
	public:
		typedef _Class Type;
		LuaGlueObjectImpl(Type *ptr, LuaGlueClass<_Class> *clss, bool owner = false) : _ref_cnt(1), _clss(clss), _ptr(ptr), owner(owner)
		{
			LG_Debug("ctor");
		}
		
		~LuaGlueObjectImpl()
		{
			if(_clss) _clss->_impl_dtor(_ptr); 
			if(owner)
			{
				LG_Debug("we're owner, delete");
				delete _ptr;
			}
			
			_ref_cnt = 0;
			_clss = 0;
			_ptr = 0;
			owner = false;
		}
		
		Type *get()
		{
			_ref_cnt++;
			return _ptr;
		}
		
		int put()
		{
			_ref_cnt--;
			return _ref_cnt;
		}
		
		Type &ref() { return *_ptr; }
		Type *ptr() { return _ptr; }
	private:
		std::atomic_int _ref_cnt;
		LuaGlueClass<Type> *_clss;
		Type *_ptr;
		bool owner;
};

template<class _Class>
class LuaGlueObjectImpl<std::shared_ptr<_Class>>
{
	public:
		typedef std::shared_ptr<_Class> Type;
		typedef _Class ClassType;
		LuaGlueObjectImpl(Type *ptr, LuaGlueClass<_Class> *clss, bool owner = false) : _ref_cnt(1), _clss(clss), _ptr(ptr), owner(owner)
		{
			LG_Debug("ctor");
		}
		
		~LuaGlueObjectImpl()
		{
			if(_clss) _clss->_impl_dtor(_ptr); 
			if(owner)
			{
				printf("~LuaGlueObjectImpl<std::shared_ptr<%s>>: we're owner, delete\n", typeid(Type).name());
				delete _ptr;
			}
			
			_ref_cnt = 0;
			_clss = 0;
			_ptr = 0;
			owner = false;
		}
		
		Type *get()
		{
			_ref_cnt++;
			return _ptr;
		}
		
		int put()
		{
			_ref_cnt--;
			return _ref_cnt;
		}
		
		Type &ref() { return *_ptr; }
		Type *ptr() { return _ptr; }
	private:
		std::atomic_int _ref_cnt;
		LuaGlueClass<ClassType> *_clss;
		Type *_ptr;
		bool owner;
};

template<class _Class>
class LuaGlueObject
{
	public:
		typedef _Class Type;
		
		LuaGlueObject() : p(0)
		{
			LG_Debug("ctor()");
		}
		
		LuaGlueObject(Type *ptr, LuaGlueClass<Type> *clss = nullptr, bool owner = false) : p(new LuaGlueObjectImpl<Type>(ptr, clss, owner))
		{
			LG_Debug("ctor(%p, %s, %i)", ptr, clss->name().c_str(), owner);
		}
		
		LuaGlueObject(const LuaGlueObject<Type> *rhs) : p(rhs->p)
		{
			LG_Debug("ctor(LuaGlueObject(%p))", p->ptr());
			(void)p->get();
		}
		
		LuaGlueObject(const LuaGlueObject &rhs) : p(rhs.p)
		{
			LG_Debug("copy ctor(%p)", p->ptr());
			(void)p->get();
		}
		
		LuaGlueObject &operator=( const LuaGlueObject &rhs )
		{
			LG_Debug("assign(%p)", rhs.p->ptr());
			p = rhs.p;
			(void)p->get();
		}
		
		~LuaGlueObject()
		{
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
		
		Type &operator*() { return p->ref(); }
		Type *operator->() { return p->ptr(); }
		
		Type *ptr() { return p->ptr(); }
	private:
		LuaGlueObjectImpl<Type> *p;
};

template<class _Class>
class LuaGlueObject<std::shared_ptr<_Class>>
{
	public:
		typedef _Class ClassType;
		typedef std::shared_ptr<_Class> Type;
		
		LuaGlueObject() : p(0)
		{
			LG_Debug("ctor()");
		}
		
		LuaGlueObject(Type *ptr, LuaGlueClass<ClassType> *clss = nullptr, bool owner = false) : p(new LuaGlueObjectImpl<Type>(ptr, clss, owner))
		{
			LG_Debug("ctor(%p, %s, %i)", ptr, clss->name().c_str(), owner);
		}
		
		LuaGlueObject(const LuaGlueObject<Type> *rhs) : p(rhs->p)
		{
			LG_Debug("ctor(LuaGlueObject(%p))", p->ptr());
			(void)p->get();
		}
		
		LuaGlueObject(const LuaGlueObject &rhs) : p(rhs.p)
		{
			LG_Debug("copy ctor(%p)", p->ptr());
			(void)p->get();
		}
		
		LuaGlueObject &operator=( const LuaGlueObject &rhs )
		{
			LG_Debug("assign(%p)", rhs.p->ptr());
			p = rhs.p;
			(void)p->get();
		}
		
		~LuaGlueObject()
		{
			if(!p)
				LG_Debug("p == 0");
			
			if(p && !p->put())
			{
				LG_Debug("dtor ref count hit 0, delete impl");
				delete p;
				p = nullptr;
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
		
		Type &operator*() { return p->ref(); }
		Type *operator->() { return p->ptr(); }
		
		Type *ptr() { return p->ptr(); }
	private:
		LuaGlueObjectImpl<Type> *p;
};

#endif /* LUAGLUE_OBJECT_BASE_H_GUARD */
