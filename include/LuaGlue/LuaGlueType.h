#ifndef LUAGLUE_TYPE_H
#define LUAGLUE_TYPE_H

class LuaGlueBase;

#include "LuaGlue/LuaGlueTypeBase.h"

template<typename _Type>
class LuaGlueType : public LuaGlueTypeBase
{
	public:
		typedef _Type ValueType;
		
		static const char METATABLE_CLASSNAME_FIELD[];
		static const char METATABLE_INTCLASSNAME_FIELD[];
		static const char METATABLE_CLASSIDX_FIELD[];
		static const char METATABLE_TYPEID_FIELD[];
		
		virtual ~LuaGlueTYPE() { }
		virtual bool glue(LuaGlueBase *) = 0;
		
		
		
};

template<class _Class>
const char LuaGlueType::METATABLE_CLASSNAME_FIELD[] = "LuaGlueClassName";

template<class _Class>
const char LuaGlueType::METATABLE_INTCLASSNAME_FIELD[] = "LuaGlueIntClassName";

template<class _Class>
const char LuaGlueType::METATABLE_CLASSIDX_FIELD[] = "LuaGlueClassIdx";

template<class _Class>
const char LuaGlueType::METATABLE_TYPEID_FIELD[] = "typeid";


#endif /* LUAGLUE_TYPE_H */
