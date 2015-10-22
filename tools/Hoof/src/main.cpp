#include <iostream>
#include <string>

#include "llvm/Support/CommandLine.h"

#include "Hoof.h"
#include "HoofDebug.h"

// these are generated
#include "builtin_macros.h"
#include "builtin_inc.h"

using namespace hoof;

static llvm::cl::list<std::string>
I_dirs("I", llvm::cl::value_desc("directory"), llvm::cl::Prefix,
    llvm::cl::desc("Add directory to include search path"));

static llvm::cl::opt<std::string>
InputFilename(llvm::cl::Positional, llvm::cl::value_desc("<input file>"),
	llvm::cl::Optional);

static llvm::cl::list<std::string>
BasePaths("d", llvm::cl::value_desc("directory"), llvm::cl::Prefix,
	llvm::cl::desc("Add a path to scan for headers"));

static llvm::cl::list<std::string>
ExcludePatterns("x", llvm::cl::value_desc("patterns"), llvm::cl::Prefix,
	llvm::cl::desc("Add exclude patterns"));

static llvm::cl::alias
ExcludePatternsLong("exclude", llvm::cl::desc("Alias for -x"), llvm::cl::aliasopt(ExcludePatterns));

static llvm::cl::list<std::string>
IncludePatterns("i", llvm::cl::value_desc("patterns"), llvm::cl::Prefix,
	llvm::cl::desc("Add include patterns. These will override any exclude patterns."));

static llvm::cl::alias
IncludePatternsLong("include", llvm::cl::desc("Alias for -i"), llvm::cl::aliasopt(IncludePatterns));

static llvm::cl::list<std::string>
ExcludeSymPatterns("xs", llvm::cl::value_desc("patterns"), llvm::cl::Prefix,
	llvm::cl::desc("Add symbol exclude patterns"));

static llvm::cl::alias
ExcludeSymPatternsLong("exclude-sym", llvm::cl::desc("Alias for -xs"), llvm::cl::aliasopt(ExcludeSymPatterns));

static llvm::cl::list<std::string>
IncludeSymPatterns("is", llvm::cl::value_desc("patterns"), llvm::cl::Prefix,
	llvm::cl::desc("Add symbol include patterns."));

static llvm::cl::alias
IncludeSymPatternsLong("include-sym", llvm::cl::desc("Alias for -is"), llvm::cl::aliasopt(IncludeSymPatterns));

int main(int argc, char **argv)
{
	llvm::cl::ParseCommandLineOptions(argc, argv, " hoof\n"
      "  This generates lua bindings using LuaGlue.");

	Hoof *hoof = new Hoof;
	if(!hoof->initCompilerInstance())
	{
		HF_Error("Hoof initCompilerInstance failed :(\n");
		exit(-1);
	}
	
	// user include paths
	hoof->addHeaderSearchPaths(I_dirs);
	// apply user and system include paths
	// user include paths override system paths
	
	// builtin include paths
	for(auto &inc: builtinIncludePaths)
	{
		hoof->addHeaderSearchPath(inc);
	}
	
	hoof->applyHeaderSearchOptions();
	
	hoof->addIncludePatterns(IncludePatterns);
	hoof->addExcludePatterns(ExcludePatterns);
	
	hoof->addIncludeSymPatterns(IncludeSymPatterns);
	hoof->addExcludeSymPatterns(ExcludeSymPatterns);
	
	hoof->addSearchPaths(BasePaths);
	
	if(InputFilename.getValue().size())
		hoof->processSingleFile(InputFilename.getValue());
	
	for(auto bp = BasePaths.begin(); bp != BasePaths.end(); bp++)
	{
		HF_Debug("process base path: %s\n", bp->c_str());
		hoof->processDirectory(*bp);
	}
	
	delete hoof;
	
	return 0;
}