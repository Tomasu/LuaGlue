#ifndef HOOF_H_GUARD
#define HOOF_H_GUARD

#include <vector>
#include <string>

#include "clang/Frontend/CompilerInstance.h"

using clang::CompilerInstance;

namespace hoof {

class ASTConsumer;

class Hoof
{
	public:
		Hoof();
		~Hoof();
		
		bool initCompilerInstance();
		bool applyHeaderSearchOptions();
		
		bool addHeaderSearchPath(const std::string &p);
		bool addHeaderSearchPaths(const std::vector<std::string> &list);

		bool addSearchPaths(const std::vector<std::string> &list);
		bool addExcludePatterns(const std::vector<std::string> &list);
		bool addIncludePatterns(const std::vector<std::string> &list);
		
		bool addExcludeSymPatterns(const std::vector<std::string> &list);
		bool addIncludeSymPatterns(const std::vector<std::string> &list);
		
		bool processDirectory(const std::string &p, const std::string &base = "");
		bool processSingleFile(const std::string &p, const std::string &base = "");
		
	private:
		CompilerInstance *ci;
		ASTConsumer *astConsumer;
		
		std::vector<std::string> headerSearchPaths;
		
		std::vector<std::string> searchPaths;
		std::vector<std::string> excludePatterns;
		std::vector<std::string> includePatterns;
		
		std::vector<std::string> excludeSymPatterns;
		std::vector<std::string> includeSymPatterns;
		
		bool includeMatch(const std::string &p);
		bool excludeMatch(const std::string &p);
		
		bool includeSymMatch(const std::string &p);
		bool excludeSymMatch(const std::string &p);
};

}


#endif /* HOOF_H_GUARD */
