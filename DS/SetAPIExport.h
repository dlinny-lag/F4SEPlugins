#pragma once
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "CollectionTypeId.h"

#include "Set.hpp"
#include "Shared/FormComparer.hpp"
#include "ArrayAPIExport.h"
#include "SetExport.hpp"

#define BUILD_SCRIPT_NAME(ElementType) \
	AS_IS(PapyrusPrefix) AS_STR(ElementType) AS_IS("Set")

// just to avoid definition of StructName_* variable in multiple units
#define DECLARE_STRUCT_GLOBAL(prefix, structName, scriptOwner) \
	inline char StructName_##prefix##_##structName##[] = ##scriptOwner## "#" DECLARE_STRUCT_STRING(structName); \
	typedef VMStruct<StructName_##prefix##_##structName##> VMStruct_##prefix##_##structName##;

#define DECLARE_VARIABLE(type, name) extern type name

#define DECLARE_SET_EXPORT(Variable, ElementType, TypeId, ElementTypeName) \
	inline char AS_IS(##ElementTypeName##)SetPapyrusName[] = BUILD_SCRIPT_NAME(ElementTypeName);\
	DECLARE_STRUCT_GLOBAL(##ElementTypeName##Set, Iterator, BUILD_SCRIPT_NAME(ElementTypeName))\
	DECLARE_STRUCT_GLOBAL(##ElementTypeName##Set, Pointer, BUILD_SCRIPT_NAME(ElementTypeName))\
	typedef SetExport<ElementType, TypeId, ##ElementTypeName##SetPapyrusName, \
		VMStruct_##ElementTypeName##Set_Iterator, VMStruct_##ElementTypeName##Set_Pointer, \
		ArrayAPIExport::##ElementTypeName##ArrayExport> ##ElementTypeName##SetExport; \
	DECLARE_VARIABLE(##ElementTypeName##SetExport, Variable);

namespace SetAPIExport
{
	using namespace DS;
	DECLARE_SET_EXPORT(g_IntSetExport, SInt32, IntSetTypeId, Int)
	DECLARE_SET_EXPORT(g_FloatSetExport, float, FloatSetTypeId, Float)
	DECLARE_SET_EXPORT(g_StringSetExport, BSFixedString, StringSetTypeId, String)
	DECLARE_SET_EXPORT(g_FormSetExport, FORM_TYPE, FormSetTypeId, Form)

	void DumpAll();
	void Clear();
	bool Save(const F4SESerializationInterface* serializer);
	void RegisterLoaders();
	bool Register(VirtualMachine* vm);
};

#undef DECLARE_SET_EXPORT
#undef DECLARE_VARIABLE
#undef DECLARE_STRUCT_GLOBAL
#undef BUILD_SCRIPT_NAME
