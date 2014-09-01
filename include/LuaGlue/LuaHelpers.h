#ifndef LUAGLUE_LUAHELPERS_H_GUARD
#define LUAGLUE_LUAHELPERS_H_GUARD

#include "LuaGlue/LuaGlueBase.h"
#include "LuaGlue/LuaGlueStackTemplates.h"

class LuaHelpers
{
	public:
		template<class _Class>
		static bool glueFunction(LuaGlueBase *g, const std::string &name, _Class *ctx, int (*fn)(lua_State *), int idx = -2)
		{
			lua_pushlightuserdata(g->state(), ctx);
			lua_pushcclosure(g->state(), fn, 1);
			lua_setfield(g->state(), idx, name.c_str());
			return true;
		}
		
		template<typename _Value>
		static bool setField(LuaGlueBase *g, const std::string &name, _Value v, int idx = -2)
		{
			stack<_Value>::put(g, g->state(), v);
			lua_setfield(g->state(), idx, name.c_str());
			return true;
		}
		
		template<typename _Type>
		static bool setDottedField(LuaGlueBase *g, const std::string &name, _Type v)
		{
			lua_State *s = g->state();
			
			// explode dotted string
			std::vector<std::string> explode;
			std::size_t lastpos = 0, pos = name.find(".");

			if(pos == std::string::npos)
				explode.push_back(name);

			while(pos != std::string::npos)
			{
				explode.push_back(std::string(name, lastpos, pos - lastpos).c_str());

				lastpos = ++pos;
				if((pos = name.find(".", pos)) == std::string::npos)
					explode.push_back(std::string(name, lastpos, pos - lastpos).c_str());
			}
			
			if(explode.size() == 1)
			{
				stack<_Type>::put(g, s, v);
				lua_setglobal(s, explode[0].c_str());
				return true;
			}
			
			// find symbol
			lua_getglobal(s, explode[0].c_str());
			if(lua_isnil(s, -1))
			{
				lua_pop(s, 1);
				return false;
			}
			
			if(explode.size() > 1)
			{
				auto it = explode.begin() + 1;
				for(; it != explode.end()-1; it++)
				{
					lua_getfield(s, -1, (*it).c_str());
					if(lua_isnil(s, -1))
					{
						// table not found
						lua_pop(s, 1); // pop sym.
						return false;
					}
				}
				
				if(it != explode.end())
				{
					stack<_Type>::put(g, s, v);
					lua_setfield(s, -1, (*it).c_str());
					return true;
				}
			}
			
			return false;
		}
		
		static bool getDottedField(LuaGlueBase *g, const std::string &name)
		{
			lua_State *s = g->state();
			
			// explode dotted string
			std::vector<std::string> explode;
			std::size_t lastpos = 0, pos = name.find(".");

			if(pos == std::string::npos)
				explode.push_back(name);

			while(pos != std::string::npos)
			{
				explode.push_back(std::string(name, lastpos, pos - lastpos).c_str());

				lastpos = ++pos;
				if((pos = name.find(".", pos)) == std::string::npos)
					explode.push_back(std::string(name, lastpos, pos - lastpos).c_str());
			}
			
			// find symbol
			lua_getglobal(s, explode[0].c_str());
			if(lua_isnil(s, -1))
			{
				lua_pop(s, 1);
				return false;
			}
			
			if(explode.size() > 1)
			{
				for(auto it = explode.begin() + 1; it != explode.end(); it++)
				{
					lua_getfield(s, -1, (*it).c_str());
					if(lua_isnil(s, -1))
					{
						// table not found
						lua_pop(s, 1); // pop sym.
						return false;
					}
				}
			}
			
			return true;
		}
};

#endif /* LUAGLUE_LUAHELPERS_H_GUARD */
