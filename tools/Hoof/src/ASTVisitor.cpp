#include "ASTVisitor.h"

using clang::FunctionDecl;
using clang::QualType;
using clang::Type;

namespace hoof {
	
bool ASTVisitor::VisitFunctionDecl(FunctionDecl *f)
{
	if(f->isGlobal())
		printf("gfunc: %s\n", f->getDeclName().getAsString().c_str());
	
	return true;
}

bool ASTVisitor::TraverseType(QualType qt)
{
	auto &t = *qt;
	if(t.isRecordType())
	{
		//auto rdec = t.getAsCXXRecordDecl();
			
		//printf("type: %s\n", rdec->getDeclName().getAsString().c_str());
	}
	
	//t.dump();
	//if(t->isRecordType())
	//{
	//	auto bti = t.getBaseTypeIdentifier();
	//	auto name = bti ? bti->getNameStart() : "unk";
	//	printf("class: %s\n", name ? name : "unk");
	//}
	
	return true;
}

bool ASTVisitor::VisitType(Type *t)
{
	//printf("VisitType: record:%i class:%i obj:%i\n", t->isRecordType(), t->isClassType(), t->isObjectType());
	/*if(t->isRecordType())
	{
		auto rdec = t->getAsCXXRecordDecl();
		if(rdec->isClass())
		{
			fprintf(stderr,"\n");
			rdec->dump();
		}
	}*/
	//if(t->isRecordType() && t->isClassType())
	//{
	//	auto rdec = t->getAsCXXRecordDecl();
	//	
	//	printf("type: %s\n", rdec->getDeclName().getAsString().c_str());
	//}
	
	return true;
}

}