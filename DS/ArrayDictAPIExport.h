#pragma once
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/GameReferences.h"
#include "f4se/PapyrusEvents.h"
#include "CollectionTypeId.h"
#include "Shared/BSFixedStringEqual.hpp"
#include "Shared/BSFixedStringHashier.hpp"
#include "PrimitiveArrayDictsSet.hpp"
#include "StructArrayHandler.hpp"
#include "StructArrayDictsSet.hpp"
#include "SetAPIExport.h"

#include "PrimitiveArrayDictExport.hpp"
#include "StructArrayDictExport.hpp"

#define BUILD_SCRIPT_NAME(KeyTypeName, ValueTypeName) \
	AS_IS(PapyrusPrefix) AS_STR(KeyTypeName) AS_IS("Dict") AS_STR(ValueTypeName) AS_IS("Array")

// just to avoid declaration of StructName_* variable in multiple units
#define DECLARE_STRUCT_GLOBAL(prefix, structName, scriptOwner) \
	inline char StructName_##prefix##_##structName##[] = ##scriptOwner## "#" DECLARE_STRUCT_STRING(structName); \
	typedef VMStruct<StructName_##prefix##_##structName##> VMStruct_##prefix##_##structName##;

#define DECLARE_VARIABLE(type, name) extern type name

#define DECLARE_PRIMITIVE_ARRAY_DICT(Variable, KeyType, ValueType, DictTypeId, Version, KeyTypeName, ValueTypeName) \
	inline char AS_IS(##KeyTypeName##)Dict##ValueTypeName##Array_ScriptName[] = BUILD_SCRIPT_NAME(KeyTypeName, ValueTypeName); \
	DECLARE_STRUCT_GLOBAL(##KeyTypeName##Dict##ValueTypeName##Array, KeyIterator, BUILD_SCRIPT_NAME(KeyTypeName, ValueTypeName)) \
	DECLARE_STRUCT_GLOBAL(##KeyTypeName##Dict##ValueTypeName##Array, KeyPointer, BUILD_SCRIPT_NAME(KeyTypeName, ValueTypeName)) \
	DECLARE_STRUCT_GLOBAL(##KeyTypeName##Dict##ValueTypeName##Array, Result, BUILD_SCRIPT_NAME(KeyTypeName, ValueTypeName)) \
	typedef PrimitiveArrayDictExport<KeyType, ValueType, DictTypeId, Version, \
			##KeyTypeName##Dict##ValueTypeName##Array_ScriptName, \
			ArrayAPIExport::##KeyTypeName##ArrayExport, SetAPIExport::##KeyTypeName##SetExport, ArrayAPIExport::##ValueTypeName##ArrayExport, \
			VMStruct_##KeyTypeName##Dict##ValueTypeName##Array_KeyIterator, \
			VMStruct_##KeyTypeName##Dict##ValueTypeName##Array_KeyPointer, \
			VMStruct_##KeyTypeName##Dict##ValueTypeName##Array_Result> ##KeyTypeName##Dict##ValueTypeName##ArrayExport; \
	DECLARE_VARIABLE(##KeyTypeName##Dict##ValueTypeName##ArrayExport, Variable)

#define DECLARE_STRUCT_ARRAY_DICT(Variable, KeyType, DictTypeId, Version, KeyTypeName) \
	inline char AS_IS(##KeyTypeName##)DictStructArray_ScriptName[] = BUILD_SCRIPT_NAME(KeyTypeName, Struct); \
	DECLARE_STRUCT_GLOBAL(##KeyTypeName##DictStructArray, KeyIterator, BUILD_SCRIPT_NAME(KeyTypeName, Struct)) \
	DECLARE_STRUCT_GLOBAL(##KeyTypeName##DictStructArray, KeyPointer, BUILD_SCRIPT_NAME(KeyTypeName, Struct)) \
	typedef StructArrayDictExport<KeyType, DictTypeId, Version, ##KeyTypeName##DictStructArray_ScriptName, \
            ArrayAPIExport::##KeyTypeName##ArrayExport, SetAPIExport::##KeyTypeName##SetExport, ArrayAPIExport::StructArrayExport, \
            VMStruct_##KeyTypeName##DictStructArray_KeyIterator, \
			VMStruct_##KeyTypeName##DictStructArray_KeyPointer> ##KeyTypeName##DictStructArrayExport; \
	DECLARE_VARIABLE(##KeyTypeName##DictStructArrayExport, Variable)


namespace ArrayDictAPIExport
{
	using namespace DS;
	// --- int key
	DECLARE_PRIMITIVE_ARRAY_DICT(g_IntDictIntArrayExport, SInt32, SInt32, IntDictIntArrayTypeId, 1, Int, Int);
	DECLARE_PRIMITIVE_ARRAY_DICT(g_IntDictFloatArrayExport, SInt32, float, IntDictFloatArrayTypeId, 1, Int, Float);
	DECLARE_PRIMITIVE_ARRAY_DICT(g_IntDictStringArrayExport, SInt32, BSFixedString, IntDictStringArrayTypeId, 1, Int, String);
	DECLARE_PRIMITIVE_ARRAY_DICT(g_IntDictFormArrayExport, SInt32, FORM_TYPE, IntDictFormArrayTypeId, 1, Int, Form);

	DECLARE_STRUCT_ARRAY_DICT(g_IntDictStructArrayExport, SInt32, IntDictStructArrayTypeId, 1, Int);

// --- string key
	DECLARE_PRIMITIVE_ARRAY_DICT(g_StringDictIntArrayExport, BSFixedString, SInt32, StringDictIntArrayTypeId, 1, String, Int);
	DECLARE_PRIMITIVE_ARRAY_DICT(g_StringDictFloatArrayExport, BSFixedString, float, StringDictFloatArrayTypeId, 1, String, Float);
	DECLARE_PRIMITIVE_ARRAY_DICT(g_StringDictStringArrayExport, BSFixedString, BSFixedString, StringDictStringArrayTypeId, 1, String, String);
	DECLARE_PRIMITIVE_ARRAY_DICT(g_StringDictFormArrayExport, BSFixedString, FORM_TYPE, StringDictFormArrayTypeId, 1, String, Form);

	DECLARE_STRUCT_ARRAY_DICT(g_StringDictStructArrayExport, BSFixedString, StringDictStructArrayTypeId, 1, String);

// --- form key
	DECLARE_PRIMITIVE_ARRAY_DICT(g_FormDictIntArrayExport, FORM_TYPE, SInt32, FormDictIntArrayTypeId, 1, Form, Int);
	DECLARE_PRIMITIVE_ARRAY_DICT(g_FormDictFloatArrayExport, FORM_TYPE, float, FormDictFloatArrayTypeId, 1, Form, Float);
	DECLARE_PRIMITIVE_ARRAY_DICT(g_FormDictStringArrayExport, FORM_TYPE, BSFixedString, FormDictStringArrayTypeId, 1, Form, String);
	DECLARE_PRIMITIVE_ARRAY_DICT(g_FormDictFormArrayExport, FORM_TYPE, FORM_TYPE, FormDictFormArrayTypeId, 1, Form, Form);

	DECLARE_STRUCT_ARRAY_DICT(g_FormDictStructArrayExport, FORM_TYPE, FormDictStructArrayTypeId, 1, Form);

	void DumpAll();
	void Clear();
	bool Save(const F4SESerializationInterface* serializer);
	void RegisterLoaders();
	bool Register(VirtualMachine* vm);

}

#undef DECLARE_STRUCT_ARRAY_DICT
#undef DECLARE_PRIMITIVE_ARRAY_DICT
#undef DECLARE_STRUCT_GLOBAL
#undef DECLARE_VARIABLE
#undef BUILD_SCRIPT_NAME