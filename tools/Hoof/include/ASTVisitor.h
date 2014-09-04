#ifndef HOOF_ASTVisitor_H_GUARD
#define HOOF_ASTVisitor_H_GUARD

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Decl.h"

namespace hoof {

	class ASTVisitor : public clang::RecursiveASTVisitor<ASTVisitor>
	{
		public:
			ASTVisitor() { }
			bool VisitFunctionDecl(clang::FunctionDecl *f);
			bool VisitType(clang::Type *T);
			bool TraverseType(clang::QualType t);
	};

}

#endif /* HOOF_ASTVisitor_H_GUARD */
