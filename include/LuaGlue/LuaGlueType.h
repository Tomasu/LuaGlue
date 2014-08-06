#ifndef LUAGLUE_TYPE_H
#define LUAGLUE_TYPE_H

#include <cassert>

#include "LuaGlue/LuaGlueBase.h"

#include "LuaGlue/LuaGlueTypeBase.h"
#include "LuaGlue/LuaGlueTypeValue.h"
#include "LuaGlue/LuaHelpers.h"

template<typename _Type>
class LuaGlueType : public LuaGlueTypeBase
{
	public:
		typedef _Type ValueType;
		
		LuaGlueType(LuaGlueBase *luaGlue, const std::string &name) : g(luaGlue), name_(name), typeid_(0), anonymous_type(false) { }
		LuaGlueType(LuaGlueBase *luaGlue) : g(luaGlue), name_(typeid(this).name()), typeid_(0), anonymous_type(true) { }
		
		virtual ~LuaGlueType() { LG_Debug("dtor"); }
		
		const std::string &name() { return name_; }
		LUA_UNSIGNED typeId() { return typeid_; }
		
		LuaGlueBase *luaGlue() { return g; }
		
	// TODO: maybe do a series of toTYPE cast methodss for various metamethods to try and call
	// if types don't match in metamethods (add, div, concat, etc)
		
		// FIXME: might need a LuaGlueTypeValue class after all, at the very least
		//  something is needed to buffer LuaGlueTypeValue from the stack templates... 
		//  as the two are trying to include each other through other headers...
		//  maybe a lower level interface class like LuaGlueTypeBase or smth.
		LuaGlueTypeValue<ValueType> *pushInstance(ValueType *d)
		{
			return pushInstance(this->luaGlue()->state(), d);
		}
		
		LuaGlueTypeValue<ValueType> *pushInstance(lua_State *s, ValueType *d, bool owner = false)
		{
			assert(d != nullptr);
			
			LG_Debug("%s %s pushInstance", name_.c_str(), CxxDemangle(ValueType));
			LuaGlueTypeValue<ValueType> *udata = (LuaGlueTypeValue<ValueType> *)lua_newuserdata(s, sizeof(LuaGlueTypeValue<ValueType>));
			new (udata) LuaGlueTypeValue<ValueType>(d, this, owner); // placement new
			
			luaL_getmetatable(s, name_.c_str());
			lua_dump_stack(s);
			lua_setmetatable(s, -2);
			lua_dump_stack(s);
			return udata;
		}
		
		LuaGlueTypeValue<ValueType> *pushInstance(lua_State *s, const LuaGlueTypeValue<ValueType> &d)
		{
			LG_Debug("LuaGlueTypeValue<%s> pushInstance", CxxDemangle(ValueType));
			
			LuaGlueTypeValue<ValueType> *udata = (LuaGlueTypeValue<ValueType> *)lua_newuserdata(s, sizeof(LuaGlueTypeValue<ValueType>));
			new (udata) LuaGlueTypeValue<ValueType>(d); // placement new to initialize object
			
			luaL_getmetatable(s, this->name().c_str());
			lua_setmetatable(s, -2);
			
			return udata;
		}
		
		LuaGlueTypeValue<std::shared_ptr<ValueType>> *pushInstance(lua_State *s, std::shared_ptr<ValueType> const &obj)
		{
			//assert(obj.get() != nullptr);
		
			LG_Debug("shared_ptr<%s> pushInstance", CxxDemangle(ValueType));
			std::shared_ptr<ValueType> *ptr_ptr = new std::shared_ptr<ValueType>(obj);
			LuaGlueTypeValue<std::shared_ptr<ValueType>> *udata = (LuaGlueTypeValue<std::shared_ptr<ValueType>> *)lua_newuserdata(s, sizeof(LuaGlueTypeValue<std::shared_ptr<ValueType>>));
			new (udata) LuaGlueTypeValue<std::shared_ptr<ValueType>>(ptr_ptr, this, true); // placement new to initialize object
			
			luaL_getmetatable(s, this->name().c_str());
			lua_setmetatable(s, -2);
			
			return udata;
		}
		
		LuaGlueTypeValue<std::shared_ptr<ValueType>> *pushInstance(lua_State *s, const LuaGlueTypeValue<std::shared_ptr<ValueType>> &obj)
		{
			assert(obj.ptr() != nullptr);
			
			LG_Debug("LuaGlueTypeValue<std::shared_ptr<%s>> pushInstance", CxxDemangle(ValueType));
			LuaGlueTypeValue<std::shared_ptr<ValueType>> *udata = (LuaGlueTypeValue<std::shared_ptr<ValueType>> *)lua_newuserdata(s, sizeof(LuaGlueTypeValue<std::shared_ptr<ValueType>>));
			new (udata) LuaGlueTypeValue<std::shared_ptr<ValueType>>(obj); // placement new to initialize object
			
			luaL_getmetatable(s, this->name().c_str());
			lua_setmetatable(s, -2);
			
			return udata;
		}
		
		// NO OVERRIDE. KEKEKE.
		virtual bool glue(LuaGlueBase *g) final
		{
			int metatable_id;
			int typetable_id;

			int type_idx;
			
			lua_State *s = g->state();
			
			lua_createtable(s, 0, 0);
			typetable_id = lua_gettop(s);
			
			const char *metaname = !this->anonymous_type ? name_.c_str() : typeid(this).name();
			
			if(!this->glue_type_properties(g))
				goto err_type;
			
			if(!this->glue_type_methods(g))
				goto err_type;
			
			lua_newtable(s); // metatable
			metatable_id = lua_gettop(s);
			
			// add type to reigstry
			lua_pushvalue(s, -1); // push ref to table on stack
			//LG_Debug("metatype: %s", metaname); 
			lua_setfield(s, LUA_REGISTRYINDEX, metaname); // set registry entry for type.
			
			type_idx = g->getSymTab().findSym(metaname).idx;
			LuaHelpers::setField(g, LuaGlueTypeBase::METATABLE_TYPEIDINT_FIELD, type_idx, metatable_id);
			
			// FIXME: METATABLE_TYPENAMEINT_FIELD is broken, either fix, or completely remove
			//LuaHelpers::setField(g, LuaGlueTypeBase::METATABLE_TYPENAMEINT_FIELD, typeid(_Class).name(), meta_id);
			LuaHelpers::setField(g, LuaGlueTypeBase::METATABLE_TYPENAME_FIELD, name_, metatable_id);
			
			LuaHelpers::glueFunction(g, LuaGlueTypeBase::METATABLE_TYPEID_FIELD, this, &typeid_cb, metatable_id);
			LuaHelpers::glueFunction(g, "__index", this, &mm_index_cb, metatable_id);
			LuaHelpers::glueFunction(g, "__newindex", this, &mm_newindex_cb, metatable_id);
			
			// TODO: this is supposed to disable lua from changing the metatable.
			// make sure it actually does.
			lua_pushvalue(s, typetable_id);
			lua_setfield(s, metatable_id, "__metatable");
			
			LuaHelpers::glueFunction(g, "__gc", this, &mm_gc_cb, metatable_id);
			LuaHelpers::glueFunction(g, "__concat", this, &mm_concat_cb, metatable_id);
			
			// TODO: add more meta method handlers.
			
			// duplicate type methods and props to instance methods and props.
			if(!this->glue_type_properties(g))
				goto err_meta;
			
			if(!this->glue_type_methods(g))
				goto err_meta;
			
			// allow sub types to add instance methods.
			if(!this->glue_instance_methods(g))
				goto err_meta;
			
			// allow sub types to add instance properties.
			if(!this->glue_instance_properties(g))
				goto err_meta;
			
			if(!this->glue_meta_methods(g))
				goto err_meta;
			
			// pop meta table, and assign as type metatable
			lua_setmetatable(s, -2);
			
			// make copy of type table
			lua_pushvalue(s, -1);
			// create global so lua can access our type as type Name.
			lua_setglobal(s, name_.c_str());
			
			// leave our type table on the stack
			return true;
			
		err_meta:
			lua_pop(s, 1); // remove metatable
		err_type:
			lua_pop(s, 1); // remove type table
			return false;
		}
		
	private:
		void setTypeId(LUA_UNSIGNED lgt) { typeid_ = lgt; }
		
		// symbol lookup metamethod
		// TODO: make sure inheritance works
		
		// if we skip the metatable check, we can speed things up a bit,
		// but that would mean any lua sub classes won't get their props checked.
		virtual int mm_index(lua_State *state)
		{
			return luaL_error(state, "index metamethod not defined for type %s", name_.c_str());
		}
		
		virtual int mm_newindex(lua_State *state)
		{
			return luaL_error(state, "newindex metamethod not defined for type %s", name_.c_str());
		}
		
		// TODO: maybe move this to LuaGlueClass, and have it use a new LuaGlueTypeValue here?
		// or change everything to LuaGlueTypeValue?
		virtual int mm_gc(lua_State *state)
		{
			LG_Debug("%s mm_gc!", lua_demangle_sym(name_.c_str()).c_str());
			//lua_dump_stack(state);
			if(lua_isuserdata(state, -1))
			{
				LG_Debug("about to put ref!");
				LuaGlueTypeValueBase *obj = (LuaGlueTypeValueBase *)lua_touserdata(state, -1);
				obj->put();
			}
			else
			{
				int type = lua_type(state, -1);
				if(type == LUA_TTABLE)
				{
					// FIXME: this is our class table, we get a gc on it
					// it is safe to ignore. There might be a way to register
					// the type with lua such that we don't get the __gc on the table
					// but for now, its ok.
					
					//lua_dump_table(state, -1);
				}
			}
			LG_Debug("mm_gc end!");
			return 0;
		}
		
		virtual int mm_concat(lua_State *state)
		{
			std::string str = this->toString();
			
			lua_pushstring(state, str.c_str());
			lua_concat(state, 2);

			return 1;
		}
		
		virtual int lg_typeid(lua_State *state)
		{
			lua_pushunsigned(state, typeid_);
			return 1;
		}
		
		static int mm_index_cb(lua_State *state)
		{
			auto cimp = (LuaGlueTypeBase *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->mm_index(state);
		}
		
		static int mm_newindex_cb(lua_State *state)
		{
			auto cimp = (LuaGlueTypeBase *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->mm_newindex(state);
		}
		
		static int mm_gc_cb(lua_State *state)
		{
			auto cimp = (LuaGlueTypeBase *)lua_touserdata(state, lua_upvalueindex(1));
			assert(cimp != nullptr);
			return cimp->mm_gc(state);
		}
		
		static int mm_concat_cb(lua_State *state)
		{
			auto cimp = (LuaGlueTypeBase *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->mm_concat(state);
		}
		
		static int typeid_cb(lua_State *state)
		{
			auto cimp = (LuaGlueTypeBase *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->lg_typeid(state);
		}
		
		static LUA_UNSIGNED next_typeid()
		{
			// TODO: use std::mersenne_twister_engine instead of a system random device
			// http://stackoverflow.com/questions/21096015/how-to-generate-64-bit-random-numbers
			static std::random_device rd;
			auto clk = std::chrono::high_resolution_clock::now();
			auto count = std::chrono::duration_cast<chrono_period>(clk.time_since_epoch()).count();
#if LUAI_BITSINT > 32
			return ((uint64_t)(count & 0xfffffffff0000000LL)) | rd();
#else
			return ((uint32_t)(count & 0xffff0000L)) | (rd() & 0xffff0000L);
#endif
		}
		
		virtual void impl_dtor(std::shared_ptr<_Type> *)
		{
			LG_Debug("type impl shared dtor");
		}
		
		virtual void impl_dtor(_Type *)
		{
			LG_Debug("type impl dtor");
		}
		
		void _impl_dtor(void *p) final
		{
			this->impl_dtor((_Type *)p);
		}
		
	protected:
		LuaGlueBase *g;
		std::string name_;
		LUA_UNSIGNED typeid_;
		bool anonymous_type;
};

#endif /* LUAGLUE_TYPE_H */
