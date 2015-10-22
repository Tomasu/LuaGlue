#ifndef HOOF_DEBUG_H_GUARD
#define HOOF_DEBUG_H_GUARD

#include <cstring>
#include <vector>
#include <cstdio>
#include <cstdarg>
#include <iostream>
#include <llvm/Support/raw_ostream.h>

#ifndef HF_DEBUG
#	define HF_DEBUG 0
#endif

enum {
	HF_LOG_NONE = 0,
	HF_LOG_TRACE,
	HF_LOG_DEBUG,
	HF_LOG_WARN,
	HF_LOG_ERROR
};

#include "clang/AST/Decl.h"

class HF_LogOut
{
	public:
		HF_LogOut(FILE *_fh) : fh(_fh), los(fileno(_fh), false, false)
		{
		
		}
		
		static HF_LogOut *getInstance()
		{
			if(!inst)
				inst = new HF_LogOut(stderr);
			
			return inst;
		}
		
		void log(const char *FILE, const char *FUNCTION, int LINE, int mode, const char *format, ...);
		void dumpDecl(clang::Decl *decl)
		{
			decl->dump(los);
		}
		
	private:
		static HF_LogOut *inst;
		FILE *fh;
		llvm::raw_fd_ostream los;
};

inline void HF_LogOut::log(const char *FILE, const char *FUNCTION, int LINE, int mode, const char *format, ...)
{
	static const char *mode_str[] = {
		"N", "T", "D", "W", "E", 0
	};
	
	va_list ap;
	va_start(ap, format);
	int msg_len = vsnprintf(0, 0, format, ap);
	va_end(ap);
	
	char *msg = new char[msg_len+1];
	if(!msg)
		return;
	
	msg[msg_len] = 0;
	
	va_start(ap, format);
	vsnprintf(msg, (size_t)msg_len+1, format, ap);
	va_end(ap);
	
	if(HF_DEBUG)
	{
		// log everything, with file/line/function details, in debug mode
		const char *fptr = strrchr(FILE, '/');
		if(!fptr)
			fptr = strrchr(FILE, '\\');
		
		if(!fptr)
			fptr = FILE;
		else
			fptr++;
	
		fprintf(fh, "%s:%s:%i: %s: %s\n", fptr, FUNCTION, LINE, mode_str[mode], msg);
	}
	else if(mode > HF_LOG_DEBUG)
	{
		// only log warnings and errors in release mode.
		fprintf(fh, "%s: %s\n", mode_str[mode], msg);
	}
	
	delete[] msg;
}

#define HF_Debug(a, ...) HF_LogOut::getInstance()->log(__FILE__, __FUNCTION__, __LINE__, HF_LOG_DEBUG, a, ##__VA_ARGS__)
#define HF_Trace(a, ...) HF_LogOut::getInstance()->log(__FILE__, __FUNCTION__, __LINE__, HF_LOG_TRACE, a, ##__VA_ARGS__)
#define HF_Warn(a, ...) HF_LogOut::getInstance()->log(__FILE__, __FUNCTION__, __LINE__, HF_LOG_WARN, a, ##__VA_ARGS__)
#define HF_Error(a, ...) HF_LogOut::getInstance()->log(__FILE__, __FUNCTION__, __LINE__, HF_LOG_ERROR, a, ##__VA_ARGS__)

#define HF_DumpDecl(d) HF_LogOut::getInstance()->dumpDecl(d)

#endif /* HOOF_DEBUG_H_GUARD */
