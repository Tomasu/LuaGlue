#include "ASTConsumer.h"
#include "ASTVisitor.h"

#include <sstream>
using clang::Decl;
using clang::SourceManager;
using clang::DeclGroup;
using clang::DeclGroupRef;
using clang::TagDecl;
using clang::NamedDecl;
using clang::TypedefNameDecl;
using clang::DeclContext;
using clang::LinkageSpecDecl;
using clang::CXXRecordDecl;
using clang::RecordDecl;
using clang::ClassTemplateDecl;
using clang::FieldDecl;
using clang::ArrayType;
using clang::ConstantArrayType;
using clang::ASTContext;
using clang::FunctionDecl;
using clang::QualType;
using clang::Type;
using clang::ValueDecl;
using clang::CXXMethodDecl;
using clang::VarDecl;
using clang::EnumDecl;
using clang::EnumConstantDecl;
using clang::ParmVarDecl;
using clang::CXXConstructorDecl;
using clang::RecordType;
using clang::TagType;
using clang::BuiltinType;

using llvm::StringRef;


namespace hoof {

ASTConsumer::ASTConsumer(SourceManager *sm) : sourceManager(sm)
{
	rv = new ASTVisitor();
	anonTypeNum = 0;
}

ASTConsumer::~ASTConsumer()
{
	delete rv;
	rv = 0;
}

void ASTConsumer::HandleTranslationUnit (ASTContext &)
{
	printf("end of TranslationUnit\n");
}

bool ASTConsumer::HandleTopLevelDecl(DeclGroupRef d)
{
	//printf("new DeclGroupRef\n");
	if(d.isSingleDecl())
	{
		auto decl = d.getSingleDecl();
		
		auto loc = decl->getLocation();
		loc = sourceManager->getExpansionLoc(loc);
		
		auto buffer = sourceManager->getBufferName(loc);
		
		if(strstr(buffer, basePath))
			visitSingle(decl);
	}
	else
		visitGroup(d.getDeclGroup());
	
	//printf("end new DeclGroupRef\n");
	//for(auto ii = d.begin(); ii != d.end(); ii++)
	//{
		//rv->TraverseDecl(*ii);
		
		
		//printf("decl type: %s\n", (*ii)->getDeclKindName()); 
		//auto namedDecl = llvm::dyn_cast<clang::NamedDecl>(*ii);
		//if(namedDecl)
		//{
		//	printf("name: %s\n", namedDecl->getDeclName().getAsString().c_str());
		//}
	//}
	
	return true;
}

void ASTConsumer::HandleTagDeclDefinition(TagDecl *d)
{
	return;
	//printf("handle tagdecl\n");
	
	auto loc = d->getLocation();
	loc = sourceManager->getExpansionLoc(loc);
	auto buffer = sourceManager->getBufferName(loc);
		
	if(strstr(buffer, basePath))
	{
		auto declName = d->getDeclName();
		const char *tag_type = "unknown";
		if(d->isStruct())
			tag_type = "struct";
		else if(d->isUnion())
			tag_type = "union";
		else if(d->isClass())
			tag_type = "class";
		else if(d->isEnum())
			tag_type = "enum";
		
		printf("tag decl: %s %s\n", tag_type, declName.isEmpty() ? "anonymous" : declName.getAsString().c_str());
		
		/*for(auto ii = d->decls_begin(); ii != d->decls_end(); ii++)
		{
			auto namedDecl = llvm::dyn_cast<NamedDecl>(*ii);
			if(namedDecl)
			{
				visitSingle(*ii);
			}
		}*/
	}
	/*if(d->isClass() && d->isCompleteDefinition())
	{
		auto loc = d->getOuterLocStart();
		loc = sourceManager->getExpansionLoc(loc);
		auto buffer = sourceManager->getBufferName(loc);
		
		if(strstr(buffer, basePath))
		{
			printf("%s: %s\n", d->isDependentType() ? "template" : "class", d->getDeclName().getAsString().c_str());
		}
		//fprintf(stderr,"\n");
		//d->dump();
		//fprintf(stderr,"\n");
	}*/
}

void ASTConsumer::visitSingle(Decl *d)
{
	auto loc = d->getLocation();
	loc = sourceManager->getExpansionLoc(loc);
	
	auto buffer = sourceManager->getBufferName(loc);
		
	if(!strstr(buffer, basePath))
		return;
	
	if(d->getAccess() == clang::AS_private || d->getAccess() == clang::AS_protected)
		return;
	
	//printf("single: %s\n", d->getDeclKindName());
	
	auto namedDecl = llvm::dyn_cast<NamedDecl>(d);
	if(namedDecl)
	{
		//printf("single: %s %s\n", namedDecl->getDeclKindName(), namedDecl->getName().data());
		//if(namedDecl->getDeclName().getAsString()[0] == '_')
		//	return;
	}
	
	switch(d->getKind())
	{
		case Decl::Kind::ClassTemplate: {
			auto templatedecl = llvm::dyn_cast<ClassTemplateDecl>(d);
			if(templatedecl)
			{
				TagDecl *tagDecl = templatedecl->getTemplatedDecl();
				visitTagDecl(tagDecl);
			}
		} break;
		
		//case Decl::Kind::Record:
		case Decl::Kind::CXXRecord:
		{
			auto tagDecl = llvm::dyn_cast<TagDecl>(d);
			visitTagDecl(tagDecl);
		} break;
		
		case Decl::Kind::Var: {
			auto varDecl = llvm::dyn_cast<VarDecl>(d);
			//auto declName = varDecl->getName();
			//printf("variable %s\n", declName.data());
			visitVar(varDecl);
		} break;
		
		case Decl::Kind::Field: {
			auto fieldDecl = llvm::dyn_cast<FieldDecl>(d);
			visitField(fieldDecl);
		} break;
		
		case Decl::Kind::Function: {
			auto funcDecl = llvm::dyn_cast<FunctionDecl>(d);
			visitFunction(funcDecl);
		} break;
		
		case Decl::Kind::CXXMethod: {
			auto methodDecl = llvm::dyn_cast<CXXMethodDecl>(d);
			visitCXXMethod(methodDecl);
		} break;
		
		case Decl::Kind::CXXConstructor: {
			auto ctorDecl = llvm::dyn_cast<CXXConstructorDecl>(d);
			visitCXXConstructor(ctorDecl);
		} break;
		
		case Decl::Kind::CXXDestructor: {
			// we don't care about dtors.
		} break;
		
		case Decl::Kind::LinkageSpec: {
			LinkageSpecDecl *lsDecl = llvm::dyn_cast<LinkageSpecDecl>(d);
			visitLinkageSpec(lsDecl);
		} break;
		
		case Decl::Kind::Typedef: {
			TypedefNameDecl *typedefDecl = llvm::dyn_cast<TypedefNameDecl>(d);
			visitTypedefDecl(typedefDecl);
		} break;
		
		case Decl::Kind::AccessSpec:
			// ignore
			break;
		
		case Decl::Kind::Enum: {
			EnumDecl *enumDecl = llvm::dyn_cast<EnumDecl>(d);
			visitEnumDecl(enumDecl);
		} break;
		
		default:
			printf("UNHANDLED Decl::Kind: %s\n", d->getDeclKindName());
			break;
	}
}

void ASTConsumer::visitVar(VarDecl *varDecl)
{
	printVar(varDecl);
}

void ASTConsumer::visitEnumDecl(EnumDecl *enumDecl)
{
	// is scoped with class tag, need to prepend enum name, and support class like features
	
	printEnumStart(enumDecl);
	
	if(enumDecl->isScoped())
	{
		printf("UNHANDLED scoped enum\n");
	}
	else
	{
		for(auto ii = enumDecl->decls_begin(); ii != enumDecl->decls_end(); ii++)
		{
			auto enumField = llvm::dyn_cast<EnumConstantDecl>(*ii);
			if(enumField)
			{
				if(enumField->getDeclName().getAsString()[0] == '_')
					continue;
				
				printEnumField(enumField);
			}
			else
			{
				printf("unscoped enum contains non constant field?\n");
			}
		}
	}
	
	printEnumEnd(enumDecl);
}

void ASTConsumer::visitField(FieldDecl *fieldDecl)
{
	auto declName = fieldDecl->getName();

	// skip reserved symbols
	auto name = declName.str();
	if(name[0] == '_')
		return;
	
	QualType qtype = fieldDecl->getType();
	const Type *type = qtype.getTypePtr();
	
	if(qtype->isArrayType())
	{
		type = type->getBaseElementTypeUnsafe();
	}
	
	//printf("type name: %s %s\n", type->getTypeClassName(), declName.data());
	auto recordType = type->getAsStructureType();
	if(recordType)
	{
		auto tagDecl = recordType->getDecl();
	
		const char *tag_type = fieldDecl->getDeclKindName();
		if(tagDecl)
		{
			if(tagDecl->isStruct())
				tag_type = "struct";
			else if(tagDecl->isUnion())
				tag_type = "union";
			else if(tagDecl->isClass())
				tag_type = "class";
			else if(tagDecl->isEnum())
				tag_type = "enum";
		}
		
		//printf("record: %p tcn: %s\n", tagDecl, type->getTypeClassName());
		//printf("field2: %s %s\n", tag_type, declName.empty() ? "anonymous" : declName.data());
	}
	
	printField(fieldDecl);
}

void ASTConsumer::visitCXXConstructor(CXXConstructorDecl *ctor)
{
	if(ctor->isDeleted() || ctor->isCopyOrMoveConstructor())
		return;
	
	printCXXConstructor(ctor);
}

void ASTConsumer::visitCXXMethod(CXXMethodDecl *meth)
{
	if(meth->isDeleted())
		return;
		
	printCXXMethod(meth);
}

void ASTConsumer::visitFunction(FunctionDecl *funcDecl)
{
	printFunction(funcDecl);
}

void ASTConsumer::visitLinkageSpec(LinkageSpecDecl *lsd)
{
	//printf("extern \"%s\" { (%s)\n", lsd->getLanguage() == LinkageSpecDecl::lang_c ? "C" : "C++", buffer);
	
	for(auto ii = lsd->decls_begin(); ii != lsd->decls_end(); ii++)
	{
		visitSingle(*ii);
	}
	
	//printf("}\n");
}

void ASTConsumer::visitGroup(DeclGroup &d)
{
	//printf("DeclGroup: %i\n", d.size());
	for(unsigned int i = 0; i < d.size(); i++)
	{
		DeclContext *ctx = d[i]->getDeclContext();
		NamedDecl *namedDecl = llvm::dyn_cast<NamedDecl>(d[i]);
		
		if(namedDecl)
		{
			auto loc = namedDecl->getLocation();
			loc = sourceManager->getExpansionLoc(loc);
			
			auto buffer = sourceManager->getBufferName(loc);
		
			if(strstr(buffer, basePath))
				visitSingle(namedDecl);
		}
	}
}

void ASTConsumer::visitTagDecl(TagDecl *tagDecl, std::string typedefName)
{
	auto declName = tagDecl->getDeclName();
	
	if(!tagDecl->isThisDeclarationADefinition())
	{
		// TODO: try seeing what happens if we either get the actual decl here,
		//  or even try using the HandleTagDeclDefinition function for decls instead?
		
		//printf("skip tag %s??\n", declName.getAsString().c_str());
		return;
	}
	
	//tagDecl = tagDecl->getCanonicalDecl();
	
	typemap[declName.getAsString().c_str()] = true;
	
	const char *tag_type = "unknown";
	if(tagDecl->isStruct())
		tag_type = "struct";
	else if(tagDecl->isUnion())
		tag_type = "union";
	else if(tagDecl->isClass())
		tag_type = "class";
	else if(tagDecl->isEnum())
		tag_type = "enum";
	
	// skip reserved symbols
	//if(tagDecl->getName().startswith("_"))
	//	return;
	
	//printf("sdecl tag: %s %s\n", tag_type, declName.isEmpty() ? "anonymous" : declName.getAsString().c_str());
	
	/*Decl *nextDecl = tagDecl->getNextDeclInContext();
	if(nextDecl)
	{
		auto next_named = llvm::dyn_cast<NamedDecl>(nextDecl);
		if(next_named)
		{
			auto nextDeclName = next_named->getDeclName();
			printf("next: %s\n", nextDeclName.isEmpty() ? "anonymous" : nextDeclName.getAsString().c_str());
		}
	}*/
	
	//if(curTag)
	//	tagStack.push_back(curTag);
	
	//curTag = tagDecl;
	printTag(tagDecl, typedefName);
	
	for(auto ii = tagDecl->decls_begin(); ii != tagDecl->decls_end(); ii++)
	{
		visitSingle(*ii);
	}
	
	printTagEnd(tagDecl);

	//curTag = tagStack.back();
	//tagStack.pop_back();
	
	//printf("sdecl tag end\n");
}

void ASTConsumer::visitTypedefDecl(TypedefNameDecl *tnd)
{
	//printf("UNHANDLED typedef\n");
	QualType qtype = tnd->getTypeSourceInfo()->getType();
	std::string typeName = tnd->getDeclName().getAsString();
	
	// we don't care about non record (enum/union/struct/class) types
	if(!qtype->isRecordType())
	{
		printf("typedef %s is not a record type\n", typeName.c_str());
		return;
	}
	else
	{
		printf("typedef %s!\n", typeName.c_str());
	}
	
	auto recordType = qtype->getAs<RecordType>();
	auto recordTypeDecl = recordType->getDecl();
	auto recordTypeDeclName = recordTypeDecl->getDeclName().getAsString();
	
	if(tnd->getDeclContext()->getParent())
	{
		printf("got a decl context!\n");
	}
	
	auto uqtype = tnd->getUnderlyingType();
	auto utype = uqtype->getAs<RecordType>();
	if(utype)
	{
		auto udecl = utype->getDecl();
		auto udeclName = udecl->getDeclName().getAsString();
		printf("udeclName: %s\n", udeclName.c_str());
		
		visitTagDecl(udecl, typeName);
		return;
	}
	
	auto nextDecl = tnd->getNextDeclInContext();
	
	if(nextDecl)
	{
		printf("got a next decl... %s\n", nextDecl->getDeclKindName());
		
		auto nextTagDecl = llvm::dyn_cast<TagDecl>(nextDecl);
		if(nextTagDecl)
		{
			auto nextName = nextTagDecl->getDeclName().getAsString();
			
			printf("next decl %s is a tag!\n", nextName.c_str());
			
			if(nextTagDecl->isThisDeclarationADefinition())
			{
				printf("next decl %s is a deffinition\n", nextName.c_str());
				if(nextName.length() == 0)
				{
					//printf("next tag is anon, we're typedefing it :o\n");
					//visitTagDecl(nextTagDecl);
					return;
				}
				else if(nextName == recordTypeDeclName)
				{
					// FIXME: I think we want to check canonical type or underlying type here
					// instead of this bull crap..
					
					printf("skip typedef: %s == next decl\n", nextName.c_str());
					return;
				}
			}
		}
	}
	
	if(typemap[recordTypeDeclName])
	{
		//printf("type already handled: %s\n", recordTypeDeclName);
		return;
	}
	
	//typeName = recordTypeDeclName;
	
	printf("g.Class<%s>(\"%s\");\n", typeName.c_str(), typeName.c_str());
}

// FIXME: should probably check for a containing class/namespace,
//  or have a separate method for static class vars and global static vars...
void ASTConsumer::printVar(VarDecl *var)
{
	auto declName = var->getDeclName();
	auto field = declName.getAsString().c_str();
	auto context = var->getDeclContext();
	auto parent = llvm::dyn_cast<NamedDecl>(context);
	if(!parent)
	{
		//	throw std::runtime_error("WAT!");
		printf("g.global(\"%s\", &%s);\n", field, field);
	}
	else
	{
		//auto parentType = parent->getName();
	
		auto typeName = parent->getName().data(); //parent->getDeclName().getAsString().c_str();
	
		printf("type.property(\"%s\", &%s::%s);\n", field, typeName, field);
	}
}

void ASTConsumer::printField(FieldDecl *fieldDecl)
{
	auto type = fieldDecl->getType();
	auto parent = fieldDecl->getParent();
	auto parentType = parent->getTypeForDecl();
	
	const char *field = fieldDecl->getName().data();
	std::string typeName = parent->getDeclName().getAsString();
	bool isArray = false;
	
	if(type->isArrayType())
	{
		const ArrayType *arrayType = type->getAsArrayTypeUnsafe();
		if(arrayType->getSizeModifier() != ArrayType::Star)
		{
			const ConstantArrayType *constantArrayType = llvm::dyn_cast<ConstantArrayType>(arrayType->getAsArrayTypeUnsafe());
			type = constantArrayType->getElementType();
		}
		
		isArray = true;
	}
	
	if(parentType->isRecordType())
	{
		auto recordType = parentType->getAsStructureType();
		//auto typeDecl = recordType ? recordType->getDecl() : 0;
		auto declName = parent->getDeclName();  //typeDecl->getDeclName();
		
		if(declName.isEmpty())
		{
			//printf("anon!\n");
			typeName = getAnonType();
		}
		//else
		//	printf("!annon %s\n", declName.getAsString().c_str());
		//else
		//	typeName = declName.getAsString();
	}
	
	printf("type.property(\"%s\", &%s::%s);\n", field, typeName.c_str(), field);
}

void ASTConsumer::printCXXConstructor(CXXConstructorDecl *ctorDecl)
{
	//ctorDecl->dump();
	
	std::stringstream sstr;
	std::stringstream type_sstr;
	
	for(auto ii = ctorDecl->param_begin(); ii != ctorDecl->param_end(); ii++)
	{
		auto paramDecl = *ii;
		auto paramDeclName = paramDecl->getDeclName();
		std::string paramName = paramDeclName.getAsString().c_str();
		std::string paramTypeName = "unk";
		
		auto paramType = paramDecl->getType();
		//if(paramType)
		{
			bool isPtr = paramType->isPointerType();
			
			if(isPtr)
				paramType = paramType->getPointeeType();
			
			if(paramType->isRecordType())
			{
				auto typeDecl = paramType->getAs<RecordType>()->getDecl();
				paramTypeName = typeDecl->getDeclName().getAsString();
				if(isPtr)
					paramTypeName += "*";
			}
			else if(paramType->isBuiltinType())
			{
				//auto builtinType = paramType->getAs<BuiltinType>();
				paramTypeName = paramType.getAsString().c_str();
			}
		}
		
		//if(!paramDecl->getType().getBase
		//std::string paramTypeName = paramTypeID ? paramTypeID->getName() : StringRef("anon");
		//printf("param: %s %s\n", paramTypeName.c_str(), paramName.c_str());
		
		if(!paramDecl->hasDefaultArg())
		{
			if(ii != ctorDecl->param_begin())
				sstr << "And";
			else
				sstr << "With";
			
			paramName[0] = toupper(paramName[0]);
			paramTypeName[0] = toupper(paramTypeName[0]);
			sstr << paramName;
		}
		
		type_sstr << paramTypeName;
		if(ii+1 != ctorDecl->param_end())
			type_sstr << ", ";
	}
	
	if(type_sstr.str().length())
		printf("type.ctor<%s>(\"new%s\");\n", type_sstr.str().c_str(), sstr.str().c_str());
	else
		printf("type.ctor(\"new%s\");\n", sstr.str().c_str());
}

void ASTConsumer::printCXXMethod(CXXMethodDecl *methDecl)
{
	auto name = methDecl->getNameInfo().getAsString();
	auto parent = methDecl->getParent();
	auto typeName = parent->getName();
	
	printf("type.method(\"%s\", &%s::%s);\n", name.c_str(), typeName.data(), name.c_str());
}

void ASTConsumer::printFunction(FunctionDecl *funcDecl)
{
	auto name = funcDecl->getNameInfo().getAsString();
	
	printf("g.func(\"%s\", &%s);\n", name.c_str(), name.c_str());
}

void ASTConsumer::printEnumStart(EnumDecl *enumDecl)
{
	auto declName = enumDecl->getDeclName();
	auto name = declName.getAsString().c_str();
	
	printf("// ENUM: %s\n", name);
}

void ASTConsumer::printEnumEnd(EnumDecl *enumDecl)
{
	auto declName = enumDecl->getDeclName();
	auto name = declName.getAsString().c_str();
	
	printf("// ENUM END: %s\n", name);
}

void ASTConsumer::printEnumField(EnumConstantDecl *field)
{
	auto declName = field->getDeclName();
	auto name = declName.getAsString().c_str();
	auto ancestor = field->getDeclContext()->getParent();
	auto contextDecl = llvm::dyn_cast<NamedDecl>(ancestor);
	
	if(contextDecl)
	{
		auto declName = contextDecl->getDeclName();
		auto typeName = declName.getAsString().c_str();
		
		printf("type.constant(\"%s\", %s::%s);\n", name, typeName, name);
	}
	else
	{
		// TODO: find containing scope?
		printf("g.constant(\"%s\", %s);\n", name, name);
	}
}

void ASTConsumer::printTag(TagDecl *tag, std::string typedefName)
{
	auto declName = tag->getDeclName();
	std::string typeName = declName.getAsString();
	
	if(declName.isEmpty() && !typedefName.length())
	{
		typeName = getNextAnonType();
	
		ValueDecl *nextDecl = llvm::dyn_cast<ValueDecl>(tag->getNextDeclInContext());
		if(nextDecl)
		{
			QualType ndType = nextDecl->getType();
			auto next_declName = nextDecl->getName();
			
			if(ndType->isArrayType())
			{
				printf("typedef decltype(%s[0]) %s;\n", getTagName(nextDecl).c_str(), typeName.c_str());
			}
			else
			{
				printf("typedef decltype(%s) %s;\n", getTagName(nextDecl).c_str(), typeName.c_str());
			}
		}
	}
	
	if(typedefName.length())
		typeName = typedefName;
	
	printf("{\nauto type = g.Class<%s>(\"%s\");\n", typeName.c_str(), typeName.c_str());
}

void ASTConsumer::printTagEnd(TagDecl *tag)
{
	auto declName = tag->getDeclName();
	
	printf("}\n");
}

}