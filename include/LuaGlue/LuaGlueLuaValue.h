#ifndef LUAGLUE_LUA_VALUE_H_GUARD
#define LUAGLUE_LUA_VALUE_H_GUARD

class LuaGlueBase;
class LuaGlueLuaTable;
class LuaGlueLuaFunction;
class LuaGlueLuaUserData;

class LuaGlueLuaValue
{
	public:
		enum class LuaType
		{
			None = LUA_TNONE,
			Nil = LUA_TNIL,
			Boolean = LUA_TBOOLEAN,
			LightUserData = LUA_TLIGHTUSERDATA,
			Number = LUA_TNUMBER,
			String = LUA_TSTRING,
			Table = LUA_TTABLE,
			Function = LUA_TFUNCTION,
			UserData = LUA_TUSERDATA,
			Thread = LUA_TTHREAD,
			NumTags = LUA_NUMTAGS
		};

	private:
		union ValueType
		{
			LuaGlueTypeValueBase *type_value;
			void *lud;
			lua_Number number;
			bool boolean;
		};
		
	public:
		template <typename... Args>
		LuaGlueLuaValue(Args... args) { clear(); set(args...); }
    
		LuaGlueLuaValue(const ValueType &d) { _data = d; }
		virtual ~LuaGlueLuaValue() { clear(); }
		
		LuaGlueLuaValue &operator=(const LuaGlueLuaValue &rhs)
		{
			switch(rhs._type)
			{
				case LuaType::None:
					set();
					break;
					
				case LuaType::Nil:
					set(rhs.type());
					break;
					
				case LuaType::Boolean:
					set(rhs._data.boolean);
					break;
				
				case LuaType::LightUserData:
					set(rhs._data.lud);
					break;
				
				case LuaType::Number:
					set(rhs._data.number);
					break;
				
				case LuaType::String:
					set(rhs._str);
					break;
				
				case LuaType::Table:
				case LuaType::Function:
				case LuaType::UserData:
					set(rhs._type, rhs._data.type_value);
					break;
				
				case LuaType::Thread:
					LG_Warn("unsupported LuaType LUA_THREAD");
					break;
					
				default:
					LG_Error("unknown type id: %i", rhs._type);
			}
			
			return *this;
		}
		
		void clear()
		{
			_type = LuaType::None;
			memset(&_data, 0, sizeof(_data));
		}
		
		LuaType type() const { return _type; }
	
		void load(LuaGlueBase* b, lua_State* s, int idx);
		void put(LuaGlueBase* base, lua_State* s);

		bool         getBoolean()               { return _data.boolean; }
		double       getNumber()                { return _data.number; }
		lua_Integer  getInteger()               { return (lua_Integer)_data.number; }
		std::string  getString()                { return _str; }
		LuaGlueTypeValueBase *getTypeValue()    { return _data.type_value; }
		LuaGlueLuaTable *getTable()             { return _data.type_value->ptr<LuaGlueLuaTable>(); }
		LuaGlueLuaFunction *getFunction()       { return _data.type_value->ptr<LuaGlueLuaFunction>(); }
		LuaGlueLuaUserData *getLuaUdata()       { return _data.type_value->ptr<LuaGlueLuaUserData>(); }
		
		template<class _T> _T *getUserData();
		
		bool isNil()                            { return _type == LuaType::Nil; }
		bool isBoolean()                        { return _type == LuaType::Boolean; }
		bool isNumber()                         { return _type == LuaType::Number; }
		bool isString()                         { return _type == LuaType::String; }
		bool isTable()                          { return _type == LuaType::Table; }
		bool isFunction()                       { return _type == LuaType::Function; }
		bool isUserData()                       { return _type == LuaType::UserData; }

		void set(LuaType type)                  { _type = type; }
		void set()                              { _type = LuaType::None; }
		void set(bool value)                    { _type = LuaType::Boolean;    _data.boolean = value; }
		void set(lua_Number value)              { _type = LuaType::Number;     _data.number = value; }
		void set(lua_Integer value)             { _type = LuaType::Number;     _data.number = (lua_Number)value; }
		void set(const char* value)             { _type = LuaType::String;     _str = std::string(value); }
		void set(std::string value)             { _type = LuaType::String;     _str = value; }
		void set(LuaType type, LuaGlueTypeValueBase *value)   { _type = type;  _data.type_value = value; }
		
		std::string toString()
		{
			char buff[2048];
			
			switch(_type)
			{
				case LuaType::None:
					snprintf(buff, sizeof(buff), "LUA_TNONE");
					break;
					
				case LuaType::Nil:
					snprintf(buff, sizeof(buff), "nil");
					break;
				
				case LuaType::Boolean:
					snprintf(buff, sizeof(buff), "%s", _data.boolean ? "true" : "false");
					break;
				
				case LuaType::LightUserData:
					snprintf(buff, sizeof(buff), "lud(%p)", _data.lud);
					break;
				
				case LuaType::Number:
					snprintf(buff, sizeof(buff), "%f", _data.number);
					break;
				
				case LuaType::String:
					snprintf(buff, sizeof(buff), "%s", _str.c_str());
					break;
				
				case LuaType::Table:
					snprintf(buff, sizeof(buff), "LUA_TTABLE");
					break;
				
				case LuaType::Function:
					snprintf(buff, sizeof(buff), "LUA_TFUNCTION");
					break;
					
				case LuaType::UserData:
					snprintf(buff, sizeof(buff), "%s", userdata_tostring().c_str());
					break;
					
				case LuaType::Thread:
					snprintf(buff, sizeof(buff), "LUA_TTHREAD");
					break;
				
				default:
					snprintf(buff, sizeof(buff), "UNK");
					break;
			}
			
			return buff;
		}
		
	protected:
		void load_function(LuaGlueBase* b, lua_State* s, int idx);
		void load_table(LuaGlueBase* b, lua_State* s, int idx);
		void load_userdata(LuaGlueBase *b, lua_State *s, int idx);
		std::string userdata_tostring();
		
	private:
		LuaType _type;
		ValueType _data;
		std::string _str;
};

inline void LuaGlueLuaValue::load(LuaGlueBase* b, lua_State* s, int idx)
{
	_type = (LuaType)lua_type(s, idx);
	
	switch(_type)
	{
		case LuaType::None:
			break;
			
		case LuaType::Nil:
			set(nullptr);
			break;
		
		case LuaType::Boolean:
			set(stack<bool>::get(b, s, idx));
			break;
		
		case LuaType::LightUserData:
			set(stack<void *>::get(b, s, idx));
			break;
		
		case LuaType::Number:
			set(stack<lua_Number>::get(b, s, idx));
			break;
		
		case LuaType::String:
			set(stack<std::string>::get(b, s, idx));
			break;
		
		case LuaType::Table:
			load_table(b, s, idx);
			break;
		
		case LuaType::Function:
			load_function(b, s, idx);
			break;
			
		case LuaType::UserData:
			load_userdata(b, s, idx);
			break;
			
		case LuaType::Thread:
			LG_Warn("unsupported lua type: LUA_TTHREAD");
			break;
		
		default:
			lua_error(s);
	}
}

#include "LuaGlue/LuaGlueLuaFunction.h"

inline void LuaGlueLuaValue::load_function(LuaGlueBase* b, lua_State* s, int idx)
{
	LuaGlueLuaFunction *function = new LuaGlueLuaFunction();
	function->load(b, s, idx);
	
	_data.type_value = new LuaGlueTypeValue<LuaGlueLuaFunction>(function, nullptr, true);
}

#include "LuaGlue/LuaGlueLuaTable.h"

inline void LuaGlueLuaValue::load_table(LuaGlueBase* b, lua_State* s, int idx)
{
	LuaGlueLuaTable *table = new LuaGlueLuaTable();
	table->load(b, s, idx);
	
	_data.type_value = new LuaGlueTypeValue<LuaGlueLuaTable>(table, nullptr, true);
}

#include "LuaGlue/LuaGlueLuaUserData.h"

std::string LuaGlueLuaValue::userdata_tostring()
{
	LuaGlueLuaUserData *udata = getLuaUdata();
	return udata->toString();
}

template<class _T>
_T *LuaGlueLuaValue::getUserData()
{
	LuaGlueLuaUserData *d = getLuaUdata();
	return d->ptr()->ptr<_T>();
}

inline void LuaGlueLuaValue::load_userdata(LuaGlueBase *b, lua_State *s, int idx)
{
	LuaGlueLuaUserData *udata = new LuaGlueLuaUserData();
	udata->load(b, s, idx);
	
	_data.type_value = new LuaGlueTypeValue<LuaGlueLuaUserData>(udata, nullptr, true);
}

inline void LuaGlueLuaValue::put(LuaGlueBase* b, lua_State* s)
{
	if(isBoolean())
		stack<bool>::put(b, s, getBoolean());
	else if(isNumber())
		stack<double>::put(b, s, getNumber());
	else if(isString())
		stack<std::string>::put(b, s, getString());
	else if(isTable())
	{
		LuaGlueLuaTable *table = getTable();
		table->put();
		
		/* will always pass a new table...
		lua_newtable(s);

		for(auto& it : m_stringTable)
		{
				stack<std::string>::put(b, s, it.first);
				it.second.put(b, s);

				lua_settable(s, -3);
		}

		for(auto& it : m_intTable)
		{
				stack<double>::put(b, s, it.first);
				it.second.put(b, s);

				lua_settable(s, -3);
		}*/
	}
	else if(isFunction())
	{
		LuaGlueLuaFunction *func = getFunction();
		func->put();
	}
	else if(isUserData())
	{
		LuaGlueLuaUserData *udata = getUserData<LuaGlueLuaUserData>();
		udata->put();
	}
	else
		lua_pushnil(s);
}

/*
template<int _Type>
class LuaGlueValueType : LuaGlueType< LuaGlueValue<_Type> >
{
	typedef LuaGlueValue<_Type> LG_ValueType;
	
	public:
		virtual ~LuaGlueValueType() { }
		LuaGlueValueType(LuaGlueBase *b) : LuaGlueType< LG_ValueType >(b, typeid(decltype(*this)).name()) { }
		
		virtual std::string toString()
		{
			LuaGlueBase *g = this->luaGlue();
			lua_State *state = g->state();
			
			std::string ret;
			
			int type = lua_type(state, 1);
			if(type == LUA_TUSERDATA)
			{
				LuaGlueTypeBase *tb = (LuaGlueTypeBase *)lua_touserdata(state, 1);
				ret = tb->toString();
			}
			else
			{
				ret = lua_tostring(state, 1);
			}
			
			return ret;
		}
		
		virtual lua_Integer toInteger()
		{
			LuaGlueBase *g = this->luaGlue();
			lua_State *state = g->state();
			
			return lua_tointeger(state, 1);
		}
		
		virtual lua_Number toNumber()
		{
			LuaGlueBase *g = this->luaGlue();
			lua_State *state = g->state();
			
			return lua_tonumber(state, 1);
		}
		
	protected:
		virtual bool glue_instance_properties(LuaGlueBase *g)
		{
			LuaHelpers::setField(g, "type", _Type);
			return true;
		}
		
		virtual int mm_index(lua_State *s)
		{
			LG_Debug("index");
			
			LuaGlueTypeValue< LG_ValueType > *vt = (LuaGlueTypeValue< LG_ValueType > *)GetLuaUdata(s, 1, typeid(LG_ValueType).name());
			LG_ValueType *obj = vt->ptr();
			
			luaL_argcheck(s, obj->TYPE == LUA_TTABLE, 1, "expected a table");
			
			obj->mm_index(s);
			
			return 1;
		}
		
		virtual int mm_newindex(lua_State *s)
		{
			LG_Debug("newindex");
			
			LuaGlueTypeValue< LG_ValueType > *vt = (LuaGlueTypeValue< LG_ValueType > *)GetLuaUdata(s, 1, typeid(LG_ValueType).name());
			LG_ValueType *obj = vt->ptr();
			
			luaL_argcheck(s, obj->TYPE == LUA_TTABLE, 1, "expected a table");
			
			obj->mm_newindex(s);
			
			return 0;
		}
		
		virtual int mm_call(lua_State *s)
		{
			LG_Debug("call");
			
			LuaGlueTypeValue< LG_ValueType > *vt = (LuaGlueTypeValue< LG_ValueType > *)GetLuaUdata(s, 1, typeid(LG_ValueType).name());
			LG_ValueType *obj = vt->ptr();
			
			luaL_argcheck(s, obj->TYPE == LUA_TFUNCTION, 1, "expected a function");
			
			obj->mm_call(s);
			
			return 1;
		}
		
		virtual int mm_concat(lua_State *s)
		{
			LG_Debug("concat");
			
			LuaGlueTypeValue< LG_ValueType > *vt = (LuaGlueTypeValue< LG_ValueType > *)GetLuaUdata(s, 1, typeid(LG_ValueType).name());
			LG_ValueType *obj = vt->ptr();
			
			obj->mm_concat(s);
			
			return 1;
		}
}
*/
#endif /* LUAGLUE_LUA_VALUE_H_GUARD */
