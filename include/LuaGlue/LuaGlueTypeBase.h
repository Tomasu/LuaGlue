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

#include "LuaGlue/LuaGlueDebug.h"
#include "LuaGlue/LuaGlueObject.h"

class LuaGlueBase;


class LuaGlueTypeBase
{
	public:
		static const char METATABLE_TYPENAME_FIELD[];
		static const char METATABLE_TYPENAMEINT_FIELD[];
		static const char METATABLE_TYPEIDINT_FIELD[];
		static const char METATABLE_TYPEID_FIELD[];
		
		LuaGlueTypeBase(LuaGlueBase *luaGlue, const std::string &name) : g(luaGlue), name_(name), typeid_(0), anonymous_type(false) { }
		LuaGlueTypeBase(LuaGlueBase *luaGlue) : g(luaGlue), name_(""), typeid_(0), anonymous_type(true) { }
		
		virtual ~LuaGlueTypeBase() { }
	
		const std::string &name() { return name_; }
		LUA_UNSIGNED typeId() { return typeid_; }
		
		virtual bool glue(LuaGlueBase *g) = 0;
	
		virtual std::string toString() = 0;
		virtual lua_Integer toInteger() = 0;
		virtual lua_Number toNumber() = 0;
		
	protected:
		void setTypeId(LUA_UNSIGNED lgt) { typeid_ = lgt; }
		
		// sub types can implement these to add instance methods and properties
		virtual bool glue_type_methods(LuaGlueBase *) { return true; }
		virtual bool glue_type_properties(LuaGlueBase *) { return true; }
		virtual bool glue_instance_methods(LuaGlueBase *) { return true; }
		virtual bool glue_instance_properties(LuaGlueBase *) { return true; }
		virtual bool glue_meta_methods(LuaGlueBase *) { return true; }
		
		LuaGlueBase *luaGlue() { return g; }
		
	private:
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
		
		virtual int mm_concat(lua_State *state)
		{
			std::string str = this->toString();
			
			lua_pushstring(state, str.c_str());
			lua_concat(state, 2);

			return 1;
		}
		
		int lg_typeid(lua_State *state)
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
			
	protected:
		LuaGlueBase *g;
		std::string name_;
		LUA_UNSIGNED typeid_;
		bool anonymous_type;
};

const char LuaGlueTypeBase::METATABLE_TYPENAME_FIELD[] = "TypeName";
const char LuaGlueTypeBase::METATABLE_TYPENAMEINT_FIELD[] = "InternalTypeName";
const char LuaGlueTypeBase::METATABLE_TYPEIDINT_FIELD[] = "InternalTypeId";
const char LuaGlueTypeBase::METATABLE_TYPEID_FIELD[] = "TypeId";

#endif /* LUAGLUE_TYPE_BASE_H */
