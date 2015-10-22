#include "Hoof.h"
#include "HoofDebug.h"
#include "ASTConsumer.h"

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fnmatch.h>

#include <vector>

#include <memory>

#include "llvm/Support/Host.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/StringRef.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/CompilerInvocation.h"
#include "clang/Frontend/LangStandard.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/HeaderSearchOptions.h"
#include "clang/Lex/DirectoryLookup.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"

using llvm::StringRef;
using clang::SourceManager;
using clang::CompilerInstance;
using clang::TargetOptions;
using clang::TargetInfo;
using clang::FileEntry;
using clang::DirectoryLookup;
using clang::LangOptions;
using clang::LangStandard;
using clang::CompilerInvocation;
using clang::HeaderSearch;
using clang::HeaderSearchOptions;
using clang::SourceManager;
using clang::SourceLocation;
using clang::FileManager;
using clang::SrcMgr::CharacteristicKind;

namespace hoof {

Hoof::Hoof() : ci(0)
{
	
}

Hoof::~Hoof()
{
	astConsumer = 0;
}

bool Hoof::initCompilerInstance()
{
	if(ci)
	{
		delete ci;
		ci = 0;
	}
	
	ci = new CompilerInstance();
	
	ci->createDiagnostics();
	
	ci->createFileManager();
	auto &fileManager = ci->getFileManager();
	
	ci->createSourceManager(fileManager);
	
	CompilerInvocation *invocation = new CompilerInvocation;
	ci->setInvocation(invocation);
	
	LangOptions &langOpts = ci->getLangOpts();
	/*
	langOpts.RTTI = 1;
	langOpts.Bool = 1;
	langOpts.CPlusPlus = 1;
	langOpts.CPlusPlus11 = 1;
	langOpts.GNUKeywords = 1;
	langOpts.CXXExceptions = 1;
	langOpts.POSIXThreads = 1;
	langOpts.SpellChecking = 1;
	*/
	
	invocation->setLangDefaults(langOpts, clang::IK_CXX, LangStandard::lang_gnucxx11);
	
	TargetOptions *pto = new TargetOptions();
	pto->Triple = llvm::sys::getDefaultTargetTriple();
	
	llvm::IntrusiveRefCntPtr<TargetInfo> pti(TargetInfo::CreateTargetInfo(ci->getDiagnostics(), std::shared_ptr<clang::TargetOptions>(pto)));
	ci->setTarget(pti.get());

	ci->createPreprocessor(clang::TU_Complete);
	auto &pp = ci->getPreprocessor();
	//pp.setPredefines(builtinMacros);
	
	pp.getBuiltinInfo().InitializeBuiltins(pp.getIdentifierTable(), pp.getLangOpts());
	
	ci->createASTContext();
	
	return true;
}

bool Hoof::applyHeaderSearchOptions()
{
	auto &pp = ci->getPreprocessor();
	LangOptions &langOpts = ci->getLangOpts();
	auto &target = ci->getTarget();
	
	HeaderSearch &headerSearch = pp.getHeaderSearchInfo();
	HeaderSearchOptions &headerSearchOpts = headerSearch.getHeaderSearchOpts();
	
	FileManager &fileManager = ci->getFileManager();
	
	clang::ApplyHeaderSearchOptions(headerSearch, headerSearchOpts, langOpts, target.getTriple());
	
	for(auto ii = headerSearchPaths.begin(); ii != headerSearchPaths.end(); ii++)
	{
		auto dirEntry = fileManager.getDirectory(StringRef(*ii), true);
		DirectoryLookup dirLookup(dirEntry, CharacteristicKind::C_System, false);
		headerSearch.AddSearchPath (dirLookup, true);
	}
	
	return true;
}

bool Hoof::addSearchPaths(const std::vector<std::string> &list)
{
	searchPaths.insert( searchPaths.end(), list.begin(), list.end() );
	return true;
}

bool Hoof::addExcludePatterns(const std::vector<std::string> &list)
{
	excludePatterns.insert( excludePatterns.end(), list.begin(), list.end() );
	return true;
}

bool Hoof::addIncludePatterns(const std::vector<std::string> &list)
{
	includePatterns.insert( includePatterns.end(), list.begin(), list.end() );
	return true;
}

bool Hoof::addExcludeSymPatterns(const std::vector<std::string> &list)
{
	excludeSymPatterns.insert( excludeSymPatterns.end(), list.begin(), list.end() );
	return true;
}

bool Hoof::addIncludeSymPatterns(const std::vector<std::string> &list)
{
	includeSymPatterns.insert( includeSymPatterns.end(), list.begin(), list.end() );
	return true;
}

bool Hoof::addHeaderSearchPath(const std::string &p)
{
	headerSearchPaths.push_back(p);
	
	return true;
}

bool Hoof::addHeaderSearchPaths(const std::vector<std::string> &list)
{
	headerSearchPaths.insert( headerSearchPaths.end(), list.begin(), list.end() );
	return true;
}

bool Hoof::processDirectory(const std::string &p, const std::string &base)
{
	// directory name too long, we can't do anything with that
	if(p.size() >= PATH_MAX)
		return false;
	
	DIR *dh = opendir(p.c_str());
	if(!dh)
		return false;
	
	std::string real_base = base;
	if(!real_base.length())
		real_base = p;
	
	struct dirent *dent = 0;
	char path_buff[PATH_MAX+1] = {0};
	while((dent = readdir(dh)))
	{
		int ent_len = strlen(dent->d_name);
		if(dent->d_name[0] == '.') // skip '.', '..', and hidden dot files
			continue;
		
		int path_len = p.size()+ent_len+2; // add slash, and null byte
		if(path_len > PATH_MAX)
		{
			// length of combined path is too long, skip
			HF_Warn("can not scan %s in %s: path too long.", dent->d_name, p.c_str());
			continue;
		}
		
		int base_len = std::min((int)p.size(), PATH_MAX);
		
		strncpy(path_buff, p.c_str(), base_len);
		if(path_buff[p.size()-1] != '/')
		{
			path_buff[p.size()] = '/';
			base_len++;
		}
		
		strncpy(path_buff+base_len, dent->d_name, std::min(ent_len+1, PATH_MAX-base_len));
		
		struct stat stat_buf;
		if(stat(path_buff, &stat_buf) != 0)
		{
			HF_Warn("WARN: can not stat %s: %s", path_buff, strerror(errno));
			continue;
		}
		
		char *ext = rindex(dent->d_name, '.');
		bool ext_test = false;
		
		if(ext)
		{
			int ext_len = strlen(ext);
			ext_test = ( strncmp(ext, ".h", ext_len) == 0 || strncmp(ext, ".hpp", ext_len) == 0 || strncmp(ext, ".hxx", ext_len) == 0 );
		}
		
		// include any file that matches the includePatterns, or headers
		int included = includeMatch(path_buff) || ext_test;

		// exclude any files and directories that match the excludePatterns
		if(excludeMatch(path_buff))
			continue;
		
		// exclude non included files, include dirs
		if(!included && !S_ISDIR(stat_buf.st_mode))
			continue;
		
		if(S_ISDIR(stat_buf.st_mode))
		{
			// recurse, if recursion becomes a problem, use an iterative algo
			//printf("dir: %s\n", path_buff);
			processDirectory(path_buff, real_base);
		}
		else if(S_ISREG(stat_buf.st_mode))
		{
			// process one file
			
			processSingleFile(path_buff, real_base);
			//printf("file: %s\n", path_buff);
		}
	}

	return true;
}

bool Hoof::includeMatch(const std::string &p)
{
	for(auto it = includePatterns.begin(); it != includePatterns.end(); it++)
	{
		int fnmatch_test = fnmatch(it->c_str(), p.c_str(), 0);
		if(fnmatch_test == 0)
			return true;
	}
	
	return false;
}

bool Hoof::excludeMatch(const std::string &p)
{
	for(auto it = excludePatterns.begin(); it != excludePatterns.end(); it++)
	{
		if(fnmatch(it->c_str(), p.c_str(), 0) == 0)
			return true;
	}
	
	return false;
}

bool Hoof::includeSymMatch(const std::string &p)
{
	for(auto it = includeSymPatterns.begin(); it != includeSymPatterns.end(); it++)
	{
		if(fnmatch(it->c_str(), p.c_str(), 0) == 0)
			return true;
	}
	
	return false;
}

bool Hoof::excludeSymMatch(const std::string &p)
{
	for(auto it = excludeSymPatterns.begin(); it != excludeSymPatterns.end(); it++)
	{
		if(fnmatch(it->c_str(), p.c_str(), 0) == 0)
			return true;
	}
	
	return false;
}

bool Hoof::processSingleFile(const std::string &p, const std::string &base)
{
	initCompilerInstance();
	applyHeaderSearchOptions();
	
	HF_Trace("file: %s", p.c_str());
	
	auto &fileManager = ci->getFileManager();
	const FileEntry *pFile = fileManager.getFile(p);
	if(!pFile)
	{
		HF_Error("Failed to get file: %s", p.c_str());
		return false;
	}
	
	auto &sourceManager = ci->getSourceManager();
	
	sourceManager.setMainFileID(sourceManager.createFileID(pFile, SourceLocation(), CharacteristicKind::C_User));
	
	astConsumer = new ASTConsumer(&sourceManager);
	ci->setASTConsumer(astConsumer);
	
	if(!base.length())
	{
		char *basePath = strdup(p.c_str());
		char *slashPath = rindex(basePath, '/');
		if(slashPath)
		{
			*slashPath = 0;
		}
		else
		{
			free(basePath);
			basePath = strdup("."); // should probably be getcwd()
		}
		
		astConsumer->setBasePath(basePath);
		//free(basePath);
	}
	else
	{
		astConsumer->setBasePath(base.c_str());
	}
	
	
	ci->getDiagnosticClient().BeginSourceFile(
		ci->getLangOpts(),
		&ci->getPreprocessor()
	);
	
	clang::ParseAST(ci->getPreprocessor(), astConsumer, ci->getASTContext(), false, clang::TU_Complete, 0, true);
	ci->getDiagnosticClient().EndSourceFile();
	
	return true;
}

}
