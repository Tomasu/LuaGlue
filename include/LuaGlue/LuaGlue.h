#ifndef LUAGLUE_H_GUARD
#define LUAGLUE_H_GUARD

#include <lua.hpp>
#include <string>
#include <map>

#include "LuaGlue/LuaGlueClassBase.h"

template<typename _Class> class LuaGlueClass;

class LuaGlue
{
	public:
		LuaGlue(lua_State *state = 0) : state_(state) { }
		~LuaGlue() { }
		
		LuaGlue &open(lua_State *state) { state_ = state; return *this; }
		LuaGlue &open()
		{
			state_ = luaL_newstate();
			luaL_openlibs(state_);
			return *this;
		}
		
		template<typename _Class>
		LuaGlueClass<_Class> &Class(const std::string &name)
		{
			auto new_class = new LuaGlueClass<_Class>(this, name);
			classes[name] = new_class;
			return *new_class;
		}
		
		lua_State *state() { return state_; }
		
		bool glue()
		{
			for(auto &c: classes)
			{
				if(!c.second->glue(this))
					return false;
			}
			
			return true;
		}
		
		const std::map<std::string, LuaGlueClassBase *> &getClasses() { return classes; }
		
	private:
		lua_State *state_;
		std::map<std::string, LuaGlueClassBase *> classes;
};

#include "LuaGlue/LuaGlueClass.h"
#include "LuaGlue/LuaGlueConstant.h"
#include "LuaGlue/LuaGlueCtorMethod.h"
#include "LuaGlue/LuaGlueDtorMethod.h"
#include "LuaGlue/LuaGlueMethod.h"
#include "LuaGlue/LuaGlueStaticMethod.h"
#include "LuaGlue/LuaGlueIndexMethod.h"
#include "LuaGlue/LuaGlueNewIndexMethod.h"
#include "LuaGlue/LuaGlueProperty.h"

#endif /* LUAGLUE_H_GUARD */
