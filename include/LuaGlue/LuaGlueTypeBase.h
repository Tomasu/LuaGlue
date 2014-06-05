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
		static const char METATABLE_CLASSNAME_FIELD[];
		static const char METATABLE_INTCLASSNAME_FIELD[];
		static const char METATABLE_CLASSIDX_FIELD[];
		static const char METATABLE_TYPEID_FIELD[];
		
		LuaGlueTypeBase(LuaGlueBase *luaGlue, const std::string &name) : g(luaGlue), name_(name), typeid_(0) { }
		
		virtual ~LuaGlueTypeBase() { }
		virtual bool glue(LuaGlueBase *) = 0;
	
		const std::string &name() { return name_; }
		LUA_UNSIGNED typeId() { return typeid_; }
		
	protected:
		void setTypeId(LUA_UNSIGNED lgt) { typeid_ = lgt; }
		
	private:
		// symbol lookup metamethod
		// TODO: make sure inheritance works
		
		// if we skip the metatable check, we can speed things up a bit,
		// but that would mean any lua sub classes won't get their props checked.
		int mm_index(lua_State *state)
		{
			//printf("index!\n");
			
			int type = lua_type(state, 2);
			if(type == LUA_TSTRING)
			{
				const char *key = lua_tostring(state, 2);
				
				lua_getmetatable(state, 1);
				lua_pushvalue(state, 2); // push key
				
				lua_rawget(state, -2); // get function
				lua_remove(state, -2); // remove metatable
					
				if(properties_.exists(key))
				{
					//printf("prop!\n");
					lua_pushvalue(state, 1); // push args
					lua_pushvalue(state, 2);
					lua_pcall(state, 2, 1, 0); // call function
				}
				
			}
			else if(type == LUA_TNUMBER)
			{
				lua_dump_stack(state);
				LG_Debug("array type?");
				lua_getmetatable(state, 1);
				lua_pushstring(state, "m__index");
				
				lua_rawget(state, -2); // get m__index method from metatable
				lua_remove(state, -2); // remove metatable
				
				if(lua_isfunction(state, -1)) { // if m__index is a function, call it
					lua_pushvalue(state, 1); // copy 1st and 2nd stack elements
					lua_pushvalue(state, 2);
					
					LG_Debug("before pcall");
					lua_pcall(state, 2, 1, 0); // removes the argument copies
				// should always be a function.. might want to put some debug/trace messages to make sure
					
					//lua_dump_stack(state);
				}
				else
				{
					LG_Debug("not a function :( %s", lua_typename(state, -1));
				}
			}
			else
			{
				printf("index: unsupported type: %i\n", type);
			}
			
			return 1;
		}
		
		int mm_newindex(lua_State *state)
		{
			int type = lua_type(state, 2);
			if(type == LUA_TSTRING)
			{
				const char *key = lua_tostring(state, 2);

				lua_getmetatable(state, 1); // get metatable
				
				if(properties_.exists(key))
				{
					lua_pushvalue(state, 2); // push key
					lua_rawget(state, -2); // get field
					lua_remove(state, -2); // remove metatable
				
					lua_pushvalue(state, 1); // push self
					lua_pushvalue(state, 2); // push key
					lua_pushvalue(state, 3); // push value
				
					lua_pcall(state, 3, 0, 0); // call function from field above
					
					//lua_dump_stack(state);
				}
				else
				{
					lua_pushvalue(state, 2); // push key
					lua_pushvalue(state, 3); // push value
					
					//lua_dump_stack(state);
					lua_rawset(state, -3);
					//lua_dump_stack(state);
				}
				
				lua_pop(state, 1);
			}
			else if(type == LUA_TNUMBER)
			{
				lua_getmetatable(state, 1);
				
				lua_pushstring(state, "m__newindex");
				lua_rawget(state, -2); // get method
				lua_remove(state, -2); // remove metatable
				
				if(lua_isfunction(state, -1)) {
					lua_pushvalue(state, 1); // push self
					lua_pushvalue(state, 2); // push idx
					lua_pushvalue(state, 3); // push value
					lua_pcall(state, 3, 0, 0);
					//lua_dump_stack(state);
				}
			}
			else
			{
				printf("newindex: unsupported type: %i\n", type);
			}
			
			//printf("newindex end!\n");
			
			return 0;
		}
		
		int mm_gc(lua_State *state)
		{
			if(lua_isuserdata(state, -1))
			{
				LG_Debug("about to gc!");
				LuaGlueObjectBase *obj = (LuaGlueObjectBase *)lua_touserdata(state, -1);
				obj->put();
			}
			return 0;
		}
		
		int mm_concat(lua_State *state)
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
		
		static int lua_index(lua_State *state)
		{
			auto cimp = (LuaGlueClass<_Class> *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->index(state);
		}
		
		static int lua_newindex(lua_State *state)
		{
			auto cimp = (LuaGlueClass<_Class> *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->newindex(state);
		}
		
		static int lua_gc(lua_State *state)
		{
			auto cimp = (LuaGlueClass<_Class> *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->gc(state);
		}
		
		static int lua_meta_concat(lua_State *state)
		{
			auto cimp = (LuaGlueClass<_Class> *)lua_touserdata(state, lua_upvalueindex(1));
			return cimp->concat(state);
		}
		
		static int lua_typeid(lua_State *state)
		{
			auto cimp = (LuaGlueClass<_Class> *)lua_touserdata(state, lua_upvalueindex(1));
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
};

const char LuaGlueType::METATABLE_CLASSNAME_FIELD[] = "LuaGlueClassName";
const char LuaGlueType::METATABLE_INTCLASSNAME_FIELD[] = "LuaGlueIntClassName";
const char LuaGlueType::METATABLE_CLASSIDX_FIELD[] = "LuaGlueClassIdx";
const char LuaGlueType::METATABLE_TYPEID_FIELD[] = "typeid";

#endif /* LUAGLUE_TYPE_BASE_H */
