#ifndef LuaGlueUtils_H_GUARD
#define LuaGlueUtils_H_GUARD

#define lua_dump_stack(L) lua_dump_stack_(__LINE__, __PRETTY_FUNCTION__, L)
inline void lua_dump_stack_(int line, const char *func, lua_State *L)
{
	int i;
	int top = lua_gettop(L);
	
	printf("%s:%i: lua_dump_stack: ", func, line);
	for (i = 1; i <= top; i++) {  /* repeat for each level */
		int t = lua_type(L, i);
		switch (t) {

			case LUA_TSTRING:  /* strings */
			printf("`%s'", lua_tostring(L, i));
			break;

			case LUA_TBOOLEAN:  /* booleans */
			printf(lua_toboolean(L, i) ? "true" : "false");
			break;

			case LUA_TNUMBER:  /* numbers */
			printf("%g", lua_tonumber(L, i));
			break;

			default:  /* other values */
			printf("%s", lua_typename(L, t));
			break;

		}
		printf(", ");  /* put a separator */
	}
	printf("\n");  /* end the listing */
}

#endif /* LuaGlueUtils_H_GUARD */