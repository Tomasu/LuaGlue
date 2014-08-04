#ifndef LUAGLUE_APPLYTUPLE_H_GUARD
#define LUAGLUE_APPLYTUPLE_H_GUARD

#include <cstdint>
#include <tuple>
#include <lua.hpp>
#include <typeinfo>

class LuaGlueBase;
template<typename _Ret, typename... _Args>
class LuaGlueLuaFuncRef;

template<int N, typename T>
class LuaGlueStaticArray;

#include "LuaGlue/LuaGlueStackTemplates.h"

// original apply tuple code:
// http://stackoverflow.com/questions/687490/how-do-i-expand-a-tuple-into-variadic-template-functions-arguments

#include "LuaGlue/ApplyTuple/ObjFunc.h"
#include "LuaGlue/ApplyTuple/ConstObjFunc.h"
#include "LuaGlue/ApplyTuple/GlueObjFunc.h"
#include "LuaGlue/ApplyTuple/GlueObjSPtrFunc.h"
#include "LuaGlue/ApplyTuple/GlueObjSPtrConstFunc.h"
#include "LuaGlue/ApplyTuple/GlueObjConstFunc.h"
#include "LuaGlue/ApplyTuple/LuaFunc.h"
#include "LuaGlue/ApplyTuple/ObjCtor.h"
#include "LuaGlue/ApplyTuple/StaticFunc.h"
#include "LuaGlue/ApplyTuple/StdFunc.h"

#endif /* LUAGLUE_APPLYTUPLE_H_GUARD */
