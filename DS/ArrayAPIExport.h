#pragma once
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "CollectionTypeId.h"

#include "PrimitiveArray.hpp"
#include "PrimitiveArrayExport.hpp"
#include "PapyrusExports.h"
#include "StructArrayExport.hpp"

#define BUILD_SCRIPT_NAME(ElementType) \
	AS_IS(PapyrusPrefix) AS_STR(ElementType) AS_IS("Array")

#define DECLARE_VARIABLE(type, name) extern type name

#define DECLARE_PRIMITIVE_ARRAY_EXPORT(Variable, ElementType, TypeId, ElementTypeName) \
	inline char AS_IS(##ElementTypeName##)ArrayScriptName[] = BUILD_SCRIPT_NAME(ElementTypeName);\
	typedef PrimitiveArrayExport<ElementType, TypeId, ##ElementTypeName##ArrayScriptName> ##ElementTypeName##ArrayExport;\
	DECLARE_VARIABLE(##ElementTypeName##ArrayExport, Variable);

namespace ArrayAPIExport
{
	using namespace DS;
	DECLARE_PRIMITIVE_ARRAY_EXPORT(g_IntArrayExport, SInt32, IntArrayTypeId, Int)
	DECLARE_PRIMITIVE_ARRAY_EXPORT(g_FloatArrayExport, float, FloatArrayTypeId, Float)
	DECLARE_PRIMITIVE_ARRAY_EXPORT(g_StringArrayExport, BSFixedString, StringArrayTypeId, String)
	DECLARE_PRIMITIVE_ARRAY_EXPORT(g_FormArrayExport, TESForm*, FormArrayTypeId, Form)

	inline char StructArrayScriptName[] = BUILD_SCRIPT_NAME(Struct);
	typedef StructArrayExportTemplate<StructArrayScriptName> StructArrayExport;
	extern StructArrayExport g_StructArrayExport;

	void DumpAll();
	void Clear();
	bool Save(const F4SESerializationInterface* serializer);
	void RegisterLoaders();
	bool Register(VirtualMachine* vm);

};
#undef DECLARE_PRIMITIVE_ARRAY_EXPORT
#undef DECLARE_VARIABLE
#undef BUILD_SCRIPT_NAME
