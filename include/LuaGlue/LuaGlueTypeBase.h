#ifndef LUAGLUE_TYPE_BASE_H
#define LUAGLUE_TYPE_BASE_H

#include <random>
#include <chrono>

#include <lua.hpp>

#if LUAI_BITSINT > 32
#	define chrono_period std::chrono::nanoseconds
#elseif LUAI_BITSINT < 32
#	error unsupported LUA_UNSIGNED type
#else
#	define chrono_period std::chrono::milliseconds
#endif

class LuaGlueBase;

class LuaGlueTypeBase
{
	public:
		static const char METATABLE_TYPENAME_FIELD[];
		static const char METATABLE_TYPENAMEINT_FIELD[];
		static const char METATABLE_TYPEIDINT_FIELD[];
		static const char METATABLE_TYPEID_FIELD[];
		
		LuaGlueTypeBase(LuaGlueBase *luaGlue, const std::string &name) : g(luaGlue), name_(name), typeid_(0) { }
		
		virtual ~LuaGlueTypeBase() { }
	
		const std::string &name() { return name_; }
		LUA_UNSIGNED typeId() { return typeid_; }
		
		template<typename _Class, typename _Value, typename _Key>
		
		
		// NO OVERRIDE. KEKEKE.
		bool glue(LuaGlueBase *g) final
		{
			lua_State *s = g;
			lua_createtable(s, 0, 0);
			
			for(auto &prop: type_props_)
			{
				if(!prop.ptr->glue(g))
					goto err_type;
			}
			
			for(auto &method: type_methods_)
			{
				if(!method.ptr->glue(g))
					goto err_type;
			}
			
			luaL_newmetatable(s, name_.c_str());
			int meta_id = lua_gettop(s);
			
			int type_idx = g->getSymTab().findSym(name_.c_str()).idx;
			lua_pushinteger(s, type_idx);
			lua_setfield(s, meta_id, METATABLE_TYPEIDINT_FIELD);
			
			//printf("LuaGlueClass::glue: intclassname: %s\n", typeid(_Class).name());
			lua_pushstring(s, typeid(_Class).name());
			lua_setfield(s, meta_id, METATABLE_INTCLASSNAME_FIELD);

			//printf("LuaGlueClass::glue: classname: %s\n", name_.c_str());
			lua_pushstring(s, name_.c_str());
			lua_setfield(s, meta_id, METATABLE_TYPENAME_FIELD);
			
			lua_pushlightuserdata(s, this);
			lua_pushcclosure(s, &lua_typeid, 1);
			lua_setfield(s, meta_id, METATABLE_TYPEID_FIELD);
			
			lua_pushlightuserdata(s, this);
			lua_pushcclosure(s, &mm_index_cb, 1);
			lua_setfield(s, meta_id, "__index");
			
			lua_pushlightuserdata(s, this);
			lua_pushcclosure(s, &mm_newindex_cb, 1);
			lua_setfield(s, meta_id, "__newindex");
			
			// TODO: this is supposed to disable lua from changing the metatable.
			// make sure it actually does.
			lua_pushvalue(s, -2);
			lua_setfield(s, meta_id, "__metatable");
			
			lua_pushlightuserdata(s, this);
			lua_pushcclosure(s, &mm_gc_cb, 1);
			lua_setfield(s, meta_id, "__gc");
			
			lua_pushlightuserdata(s, this);
			lua_pushcclosure(s, &mm_concat_cb, 1);
			lua_setfield(s, meta_id, "__concat");
			
			// copy type methods to instance methods
			for(auto &method: type_methods_)
			{
				if(!method.ptr->glue(g))
					goto err_meta;
			}
			
			// allow sub types to add instance methods.
			if(!this->glue_instance_methods())
				goto err_meta;
			
			// allow sub types to add instance properties.
			if(!this->glue_instance_properties())
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
		void setTypeId(LUA_UNSIGNED lgt) { typeid_ = lgt; }
		
		// sub types can implement these to add instance methods and properties
		virtual bool glue_instance_methods(LuaGlueBase *) {}
		virtual bool glue_instance_properties(LuaGlueBase *) {}
		
	private:
		// symbol lookup metamethod
		// TODO: make sure inheritance works
		
		// if we skip the metatable check, we can speed things up a bit,
		// but that would mean any lua sub classes won't get their props checked.
		virtual int mm_index(lua_State *state)
		{
			return luaL_error(state, "index metamethod not defined for type %s", name_.c_str());
		}
		
		virtual int mm_newindex(lua_State *)
		{
			return luaL_error(state, "newindex metamethod not defined for type %s", name_.c_str());
		}
		
		// TODO: maybe move this to LuaGlueClass, and have it use a new LuaGlueValueType here?
		// or change everything to LuaGlueValueType?
		virtual int mm_gc(lua_State *state)
		{
			if(lua_isuserdata(state, -1))
			{
				LG_Debug("about to gc!");
				LuaGlueObjectBase *obj = (LuaGlueObjectBase *)lua_touserdata(state, -1);
				obj->put();
			}
			return 0;
		}
		
		// TODO: this deffinitely needs work. similar to mm_gc??
		virtual int mm_concat(lua_State *state)
		{
			int arg1_type = lua_type(state, 1);
			int arg2_type = lua_type(state, 2);
			
			push_obj_str(state, arg1_type, 1);
			push_obj_str(state, arg2_type, 2);
			
			lua_concat(state, 2);

			return 1;
		}
		
		void push_obj_str(lua_State *state, int type, int idx)
		{
			if(type == LUA_TUSERDATA)
			{
				LuaGlueObjectBase *lg_obj = (LuaGlueObjectBase *)lua_touserdata(state, idx);
				_Class *obj = nullptr;
				
				if(lg_obj->isSharedPtr())
				{
					auto o = CastLuaGlueObjectShared(_Class, lg_obj);
					obj = o->ptr();
				}
				else
				{
					auto o = CastLuaGlueObject(_Class, lg_obj);
					obj = o->ptr();
				}
				
				char buff[2048];
				sprintf(buff, "%s(%p)", name_.c_str(), obj);
				lua_pushstring(state, buff);
			}
			else if(type == LUA_TNIL)
			{
				LG_Debug("nil!");
				lua_pushstring(state, "nil");
			}
			else
			{
				LG_Debug("type: %s", lua_typename(state, type));
				lua_pushstring(state, lua_tostring(state, idx));
			}
		}
		
		int lg_typeid(lua_State *state)
		{
			lua_pushunsigned(state, lg_typeid_);
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
			return cimp->mm_gc(state);
		}
		
		static int mm_concat_cb(lua_State *state)
		{
			auto cimp = (LuaGlueTypeBase *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->mm_concat(state);
		}
		
		static int mm_typeid_cb(lua_State *state)
		{
			auto cimp = (LuaGlueTypeBase *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->lg_typeid(state);
		}
		
		static LUA_UNSIGNED next_typeid()
			{
				static std::random_device rd;
				auto clk = std::chrono::high_resolution_clock::now();
				auto count = std::chrono::duration_cast<chrono_period>(clk.time_since_epoch()).count();
#if LUAI_BITSINT > 32
				return ((uint64_t)(count & 0xfffffffff0000000LL)) | rd();
#else
				return ((uint32_t)(count & 0xffff0000L)) | (rd() & 0xffff0000L);
#endif
			}
			
	protected:
		LuaGlueBase *g;
		std::string name_;
		LUA_UNSIGNED typeid_;
		
		LuaGlueSymTab type_props_;
		LuaGlueSymTab type_methods_;
};

const char LuaGlueTypeBase::METATABLE_TYPENAME_FIELD[] = "TypeName";
const char LuaGlueTypeBase::METATABLE_TYPENAMEINT_FIELD[] = "InternalTypeName";
const char LuaGlueTypeBase::METATABLE_TYPEIDINT_FIELD[] = "InternalTypeId";
const char LuaGlueTypeBase::METATABLE_TYPEID_FIELD[] = "TypeId";

#endif /* LUAGLUE_TYPE_BASE_H */
