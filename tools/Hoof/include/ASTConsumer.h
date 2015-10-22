#ifndef HOOF_ASTConsumer_H_GUARD
#define HOOF_ASTConsumer_H_GUARD

#include "clang/Basic/SourceManager.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclGroup.h"

namespace hoof {
	
class ASTVisitor;

/*
class HoofDecl
{
	public:
		enum {
			DECL_
		};
		
	private:
		std::string name_;
};*/

class ASTConsumer : public clang::ASTConsumer {
	public:
		ASTConsumer(clang::SourceManager *sm);
		virtual ~ASTConsumer();
		
		void Initialize(clang::ASTContext &ctx) override { this->ctx = &ctx; }
		
		void HandleTranslationUnit (clang::ASTContext &ctx);

		
		bool HandleTopLevelDecl(clang::DeclGroupRef d);
		void HandleTagDeclDefinition(clang::TagDecl *d);
		
		void setBasePath(const char *bp) { basePath = bp; }
		
	private:
		clang::ASTContext *ctx;
		ASTVisitor *rv;
		clang::SourceManager *sourceManager;
		const char *basePath;
		
		std::vector<clang::TagDecl*> tagStack;
		clang::TagDecl *curTag;
		
		std::map<std::string, bool> typemap;
		
		int anonTypeNum;
		
		inline std::string getAnonType()
		{
			static char buff[1024];
			snprintf(buff, sizeof(buff), "hoofAnonType%i", anonTypeNum);
			return std::string(buff);
		}
		
		inline std::string getNextAnonType()
		{
			anonTypeNum++;
			return getAnonType();
		}
		
		std::string getTagName(clang::NamedDecl *tag)
		{
			std::string name = tag->getName().str();
			
			clang::FieldDecl *fieldDecl = llvm::dyn_cast<clang::FieldDecl>(tag);
			if(!fieldDecl)
				return name;
			
			clang::Decl *parent = fieldDecl->getParent();
			while(parent)
			{
				clang::NamedDecl *n = llvm::dyn_cast<clang::NamedDecl>(parent);
				name = n->getName().str() + "::" + name;
				
				
				clang::FieldDecl *fd = llvm::dyn_cast<clang::FieldDecl>(parent);
				if(fd)
					parent = fd->getParent();
				else
					parent = nullptr;
			}
			
			return name;
		}
		
		std::string getAccessSpec(clang::Decl *d)
		{
			std::string access;
			
			switch(d->getAccess())
			{
				case clang::AS_public:
					access = "public";
					break;
					
				case clang::AS_protected:
					access = "protected";
					break;
					
				case clang::AS_private:
					access = "private";
					break;
					
				case clang::AS_none:
					access = "none";
					break;
					
				default:
					access = "UNK";
					break;
			}
			
			return access;
		}
		
		//std::map<std::string, clang::Decl
		void visitSingle(clang::Decl *d, bool isInDecl = false);
		void visitGroup(clang::DeclGroup &d);
		
		void visitTagDecl(clang::TagDecl *td, std::string typedefName = std::string(), bool isInnerDecl = false, bool force = false, std::string nameOverride = std::string());
		void visitField(clang::FieldDecl *fd);
		void visitLinkageSpec(clang::LinkageSpecDecl *lsd);
		void visitTypedefDecl(clang::TypedefNameDecl *typedefDecl);
		void visitCXXMethod(clang::CXXMethodDecl *meth);
		void visitCXXConstructor(clang::CXXConstructorDecl *ctor);
		void visitVar(clang::VarDecl *var);
		void visitEnumDecl(clang::EnumDecl *ed);
		void visitFunction(clang::FunctionDecl *fd);
		
		//void visitEnumField(clang::EnumConstantDecl *ecd);
		
		void printTag(clang::TagDecl *tag, std::string typedefName = std::string(), std::string nameOverride = std::string());
		void printTagEnd(clang::TagDecl *tag);
		void printEnumStart(clang::EnumDecl *ed);
		void printEnumEnd(clang::EnumDecl *ed);
		void printEnumField(clang::EnumConstantDecl *ecd);
		
		void printCXXConstructor(clang::CXXConstructorDecl *ctd);
		void printCXXMethod(clang::CXXMethodDecl *cmd);
		void printFunction(clang::FunctionDecl *fd);
		
		void printField(clang::FieldDecl *field);
		void printVar(clang::VarDecl *var);
};

}

#endif /* HOOF_ASTConsumer_H_GUARD */