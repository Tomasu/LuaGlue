class LuaGlueTypeBase;

template<typename _Class>
class LuaGlueTypeValueImpl;

template<typename _Class>
class LuaGlueTypeValue
{
	public:
		typedef _Class Type;
		
		LuaGlueTypeValue(Type *optr, LuaGlueTypeBase *type = nullptr, bool owner = false) : p(new LuaGlueTypeValueImpl<Type>(optr, type, owner))
		{
			__coverity_new__();
		}
		
		~LuaGlueTypeValue()
		{
			__coverity_delete__();
		}
		
		void put()
		{
			__coverity_delete__();
		}
};