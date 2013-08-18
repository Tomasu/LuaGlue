#ifndef LUAGLUE_SYMTAB_H_GUARD
#define LUAGLUE_SYMTAB_H_GUARD

#include <cstring>
#include <vector>

template<class T>
class LuaGlueSymTab
{
	private:
		struct Symbol {
			const char *name; T ptr; int idx;
			Symbol(const char *n = nullptr, T p = nullptr) : name(n), ptr(p), idx(-1) { }
		};
		
	public:
		LuaGlueSymTab() { }
		
		T &operator[]( const char *key )
		{
			Symbol &sym = findSym(key);
			if(!sym.name)
			{
				items.push_back(Symbol(key, nullptr));
				Symbol &new_sym = items.back();
				new_sym.name = strdup(key);
				return new_sym.ptr;
			}
			
			return sym.ptr;
		}
		
		T &operator[]( const std::string &key )
		{
			Symbol &sym = findSym(key.c_str());
			if(!sym.name)
			{
				items.push_back(Symbol(key.c_str(), nullptr));
				Symbol &new_sym = items.back();
				new_sym.name = strdup(key.c_str());
				return new_sym.ptr;
			}
			
			return sym.ptr;
		}
		
		typename std::vector<Symbol>::iterator begin()
		{
			return items.begin();
		}

		typename std::vector<Symbol>::iterator end()
		{
			return items.end();
		}
		
		bool exists(const char *key)
		{
			return findSym(key).name != nullptr;
		}
		
		T lookup(const char *key)
		{
			return findSym(key).ptr;
		}
		
		T lookup(int idx)
		{
			return findSym(idx).ptr;
		}
		
	private:
		
		Symbol nullSymbol;
		std::vector<Symbol> items;
		
		Symbol &findSym(const char *name)
		{
			for(auto &sym: items)
			{
				if(strcmp(sym.name, name) == 0)
					return sym;
			}
			
			return nullSymbol;
		}
		
		Symbol &findSym(int idx)
		{
			if(idx < 0 || idx > items.size())
				return nullSymbol;
			
			return items[idx];
		}
};

#endif /* LUAGLUE_SYMTAB_H_GUARD */
