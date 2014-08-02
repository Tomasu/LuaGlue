#ifndef LUAGLUE_TYPE_H
#define LUAGLUE_TYPE_H

class LuaGlueBase;

#include "LuaGlue/LuaGlueTypeBase.h"
#include "LuaGlue/LuaHelpers.h"

template<typename _Type>
class LuaGlueType : public LuaGlueTypeBase
{
	public:
		typedef _Type ValueType;
		
		LuaGlueType(LuaGlueBase *g, const std::string &name) : LuaGlueTypeBase(g, name) { }
		virtual ~LuaGlueType() { }
		
	// TODO: maybe do a series of toTYPE cast methodss for various metamethods to try and call
	// if types don't match in metamethods (add, div, concat, etc)
		
		LuaGlueObject<ValueType> *pushInstance(lua_State *s, ValueType *d, bool owner = false)
		{
			assert(d != nullptr);
			
			LG_Debug("%s pushInstance", CxxDemangle(ValueType));
			LuaGlueObject<ValueType> *udata = (LuaGlueObject<ValueType> *)lua_newuserdata(s, sizeof(LuaGlueObject<ValueType>));
			new (udata) LuaGlueObject<ValueType>(d, this, owner); // placement new
			
			luaL_getmetatable(s, name_.c_str());
			lua_setmetatable(s, -2);
			
			return udata;
		}
		
		// NO OVERRIDE. KEKEKE.
		virtual bool glue(LuaGlueBase *g) final
		{
			int meta_id;
			int type_idx;
			
			lua_State *s = g->state();
			lua_createtable(s, 0, 0);
			
			if(!this->glue_type_properties(g))
				goto err_type;
			
			if(!this->glue_type_methods(g))
				goto err_type;
			
			lua_newtable(s);
			
			// if we are a named type, add to registry
			if(!this->anonymous_type)
			{
				lua_pushvalue(s, -1); // push ref to table on stack
				lua_setfield(s, LUA_REGISTRYINDEX, name_.c_str()); // set registry entry for type.
			}
			
			meta_id = lua_gettop(s);
			
			type_idx = g->getSymTab().findSym(name_.c_str()).idx;
			LuaHelpers::setField(g, LuaGlueTypeBase::METATABLE_TYPEIDINT_FIELD, type_idx, meta_id);
			
			// FIXME: METATABLE_TYPENAMEINT_FIELD is broken, either fix, or completely remove
			//LuaHelpers::setField(g, LuaGlueTypeBase::METATABLE_TYPENAMEINT_FIELD, typeid(_Class).name(), meta_id);
			LuaHelpers::setField(g, LuaGlueTypeBase::METATABLE_TYPENAME_FIELD, name_, meta_id);
			
			LuaHelpers::glueFunction(g, LuaGlueTypeBase::METATABLE_TYPEID_FIELD, this, &typeid_cb, meta_id);
			LuaHelpers::glueFunction(g, "__index", this, &mm_index_cb, meta_id);
			LuaHelpers::glueFunction(g, "__newindex", this, &mm_newindex_cb, meta_id);
			
			// TODO: this is supposed to disable lua from changing the metatable.
			// make sure it actually does.
			lua_pushvalue(s, -2);
			lua_setfield(s, meta_id, "__metatable");
			
			LuaHelpers::glueFunction(g, "__gc", this, &mm_gc_cb, meta_id);
			LuaHelpers::glueFunction(g, "__concat", this, &mm_concat_cb, meta_id);
			
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
		
	protected:
		
};

#endif /* LUAGLUE_TYPE_H */
