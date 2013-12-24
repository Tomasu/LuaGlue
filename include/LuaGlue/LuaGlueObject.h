#ifndef LUAGLUE_OBJECT_BASE_H_GUARD
#define LUAGLUE_OBJECT_BASE_H_GUARD

#include <atomic>
#include <stdexcept>

#include "LuaGlue/LuaGlueDebug.h"

#ifdef LUAGLUE_TYPECHECK
#	define GetLuaUdata(state, idx, name) ((LuaGlueObjectBase *)luaL_checkudata(state, idx, name))
#else
#	define GetLuaUdata(state, idx, name) ((LuaGlueObjectBase *)lua_touserdata(state, idx))
#endif

#define CastLuaGlueObject(ClassType, o) ((LuaGlueObject<ClassType> *)o)
#define CastLuaGlueObjectShared(ClassType, o) ( (LuaGlueObject<std::shared_ptr<ClassType>> *)o )

template<class _Class>
class LuaGlueClass;

class LuaGlueObjectImplBase
{
	public:
		virtual ~LuaGlueObjectImplBase() { }
		virtual int put() = 0;
		
		virtual void *vget() = 0;
		virtual void *vptr() = 0;
		
		template<typename T>
		T *get() { return (T *)vget(); }
		
		template<typename T>
		T *ptr() { return (T *)vptr(); }
};

class LuaGlueObjectBase
{
	public:
		LuaGlueObjectBase(bool is_shared_ptr_ = false) : is_shared_ptr(is_shared_ptr_) { }
		virtual ~LuaGlueObjectBase() { }
		
		virtual void put() = 0;
		virtual LuaGlueObjectImplBase *impl() const = 0;
		
		bool isSharedPtr() { return is_shared_ptr; }
	private:
		bool is_shared_ptr;
};


template<class _Class>
class LuaGlueObjectImpl : public virtual LuaGlueObjectImplBase
{
	public:
		typedef _Class Type;
		LuaGlueObjectImpl(Type *p, LuaGlueClass<_Class> *clss, bool owner = false) : _clss(clss), _ptr(p), _owner(owner)
		{
			_ref_cnt = 1;
		}
		
		~LuaGlueObjectImpl()
		{
			if(_clss) _clss->_impl_dtor(_ptr); 
			if(_owner)
			{
				delete _ptr;
			}
			
			_ref_cnt = 0;
			_clss = 0;
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
	private:
		std::atomic_int _ref_cnt;
		LuaGlueClass<Type> *_clss;
		Type *_ptr;
		bool _owner;
};

template<class _Class>
class LuaGlueObjectImpl<std::shared_ptr<_Class>> : public virtual LuaGlueObjectImplBase
{
	public:
		typedef std::shared_ptr<_Class> Type;
		typedef _Class ClassType;
		LuaGlueObjectImpl(Type *p, LuaGlueClass<_Class> *clss, bool owner = false) : _clss(clss), _ptr(p), _owner(owner)
		{
			_ref_cnt = 1;
		}
		
		~LuaGlueObjectImpl()
		{
			if(_clss) _clss->_impl_dtor(_ptr); 
			if(_owner)
			{
				delete _ptr;
			}
			
			_ref_cnt = 0;
			_clss = 0;
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
	private:
		std::atomic_int _ref_cnt;
		LuaGlueClass<ClassType> *_clss;
		Type *_ptr;
		bool _owner;
};

template<class _Class>
class LuaGlueObject : public LuaGlueObjectBase
{
	public:
		typedef _Class Type;
		
		LuaGlueObject() : LuaGlueObjectBase(false), p(0)
		{
		}
		
		LuaGlueObject(Type *optr, LuaGlueClass<Type> *clss = nullptr, bool owner = false) : LuaGlueObjectBase(false), p(new LuaGlueObjectImpl<Type>(optr, clss, owner))
		{
		}
		
		LuaGlueObject(const LuaGlueObjectBase *rhs) : LuaGlueObjectBase(false), p(rhs->impl())
		{
			(void)p->vget();
		}
		
		LuaGlueObject(const LuaGlueObject &rhs) : LuaGlueObjectBase(false), p(rhs.p)
		{
			(void)p->vget();
		}
		
		LuaGlueObject &operator=( const LuaGlueObject &rhs )
		{
			p = rhs.p;
			(void)p->vget();
		}
		
		~LuaGlueObject()
		{
			//if(!p)
			
			if(p && !p->put())
			{
				delete p;
			}
		}
		
		void put()
		{
			if(!p)
				throw std::runtime_error("blah");
			
			if(!p->put())
			{
				delete p;
			}
		}
		
		Type &operator*() { return *p->ptr<Type>(); }
		Type *operator->() { return p->ptr<Type>(); }
		
		Type *ptr() { return p->ptr<Type>(); }
		
		LuaGlueObjectImplBase *impl() const { return p; }
	private:
		LuaGlueObjectImplBase *p;
};

template<class _Class>
class LuaGlueObject<std::shared_ptr<_Class>> : public LuaGlueObjectBase
{
	public:
		typedef _Class ClassType;
		typedef std::shared_ptr<_Class> Type;
		
		LuaGlueObject() : LuaGlueObjectBase(true), p(0)
		{
		}
		
		LuaGlueObject(Type *optr, LuaGlueClass<ClassType> *clss = nullptr, bool owner = false) : LuaGlueObjectBase(true), p(new LuaGlueObjectImpl<Type>(optr, clss, owner))
		{
		}
		
		LuaGlueObject(const LuaGlueObjectBase *rhs) : LuaGlueObjectBase(true), p(rhs->impl())
		{
			(void)p->vget();
		}
		
		LuaGlueObject(const LuaGlueObject &rhs) : LuaGlueObjectBase(true), p(rhs.p)
		{
			(void)p->vget();
		}
		
		LuaGlueObject &operator=( const LuaGlueObject &rhs )
		{
			p = rhs.p;
			(void)p->vget();
		}
		
		~LuaGlueObject()
		{
			if(p && !p->put())
			{
				delete p;
				p = nullptr;
			}
			
			p = nullptr;
		}
		
		void put()
		{
			if(!p)
				throw std::runtime_error("blah");
			
			if(!p->put())
			{
				delete p;
				p = nullptr;
			}
		}
		
		Type operator*() { return *p->ptr<Type>(); }
		Type *operator->() { return p->ptr<Type>(); }
		
		ClassType *ptr() { return (*(p->ptr<Type>())).get(); }
		
		LuaGlueObjectImplBase *impl() const { return p; }
	private:
		LuaGlueObjectImplBase *p;
};

#endif /* LUAGLUE_OBJECT_BASE_H_GUARD */
