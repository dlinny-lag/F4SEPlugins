#pragma once

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "CollectionTypeId.h"
#include "SetAPIExport.h"

#include "PrimitiveValueDictExport.hpp"
#include "StructValueDictExport.hpp"

#define BUILD_SCRIPT_NAME(KeyType, ValueType) \
	AS_IS(PapyrusPrefix) AS_STR(KeyType) AS_IS("Dict") AS_STR(ValueType)

// just to avoid declaration of StructName_* variable in multiple units
#define DECLARE_STRUCT_GLOBAL(prefix, structName, scriptOwner) \
	inline char StructName_##prefix##_##structName##[] = ##scriptOwner## "#" DECLARE_STRUCT_STRING(structName); \
	typedef VMStruct<StructName_##prefix##_##structName##> VMStruct_##prefix##_##structName##;

#define DECLARE_VARIABLE(type, name) extern type name

#define DECLARE_PRIMITIVE_DICT(Variable, KeyType, ValueType, DictTypeId, Version, KeyTypeName, ValueTypeName) \
	inline const char AS_IS(##KeyTypeName##)Dict##ValueTypeName##ScriptName[] = BUILD_SCRIPT_NAME(KeyTypeName,ValueTypeName);\
	DECLARE_STRUCT_GLOBAL(##KeyTypeName##Dict##ValueTypeName##, KeyIterator, BUILD_SCRIPT_NAME(KeyTypeName,ValueTypeName))\
	DECLARE_STRUCT_GLOBAL(##KeyTypeName##Dict##ValueTypeName##, KeyPointer, BUILD_SCRIPT_NAME(KeyTypeName,ValueTypeName))\
	DECLARE_STRUCT_GLOBAL(##KeyTypeName##Dict##ValueTypeName##, Result, BUILD_SCRIPT_NAME(KeyTypeName,ValueTypeName))\
	typedef PrimitiveValueDictExport<KeyType, ValueType, DictTypeId, Version,\
	##KeyTypeName##Dict##ValueTypeName##ScriptName,\
	ArrayAPIExport::##KeyTypeName##ArrayExport, SetAPIExport::##KeyTypeName##SetExport,\
	VMStruct_##KeyTypeName##Dict##ValueTypeName##_KeyIterator, \
	VMStruct_##KeyTypeName##Dict##ValueTypeName##_KeyPointer, \
	VMStruct_##KeyTypeName##Dict##ValueTypeName##_Result> ##KeyTypeName##Dict##ValueTypeName##Export; \
	DECLARE_VARIABLE(##KeyTypeName##Dict##ValueTypeName##Export, Variable);

#define DECLARE_STRUCT_DICT(Variable, KeyType, DictTypeId, Version, KeyTypeName) \
	inline const char AS_IS(##KeyTypeName##)DictStructScriptName[] = BUILD_SCRIPT_NAME(KeyTypeName, Struct);\
	DECLARE_STRUCT_GLOBAL(##KeyTypeName##DictStruct, KeyIterator, BUILD_SCRIPT_NAME(KeyTypeName, Struct))\
	DECLARE_STRUCT_GLOBAL(##KeyTypeName##DictStruct, KeyPointer, BUILD_SCRIPT_NAME(KeyTypeName, Struct))\
	typedef StructValueDictExport<KeyType, DictTypeId, Version, \
	##KeyTypeName##DictStructScriptName,\
	ArrayAPIExport::##KeyTypeName##ArrayExport, SetAPIExport::##KeyTypeName##SetExport,\
	VMStruct_##KeyTypeName##DictStruct_KeyIterator, VMStruct_##KeyTypeName##DictStruct_KeyPointer> \
		##KeyTypeName##DictStructExport; \
	DECLARE_VARIABLE(##KeyTypeName##DictStructExport, Variable);


namespace ValueDictAPIExport
{
	using namespace DS;
	DECLARE_PRIMITIVE_DICT(g_IntDictIntExport, SInt32, SInt32, IntDictIntTypeId, 1, Int, Int)
	DECLARE_PRIMITIVE_DICT(g_IntDictFloatExport, SInt32, float, IntDictFloatTypeId, 1, Int, Float)
	DECLARE_PRIMITIVE_DICT(g_IntDictStringExport, SInt32, BSFixedString, IntDictStringTypeId, 1, Int, String)
	DECLARE_PRIMITIVE_DICT(g_IntDictFormExport, SInt32, FORM_TYPE, IntDictFormTypeId, 1, Int, Form)

	DECLARE_STRUCT_DICT(g_IntDictStructExport, SInt32, IntDictStructTypeId, 1, Int)


	DECLARE_PRIMITIVE_DICT(g_StringDictIntExport, BSFixedString, SInt32, StringDictIntTypeId, 1, String, Int)
	DECLARE_PRIMITIVE_DICT(g_StringDictFloatExport, BSFixedString, float, StringDictFloatTypeId, 1, String, Float)
	DECLARE_PRIMITIVE_DICT(g_StringDictStringExport, BSFixedString, BSFixedString, StringDictStringTypeId, 1, String, String)
	DECLARE_PRIMITIVE_DICT(g_StringDictFormExport, BSFixedString, FORM_TYPE, StringDictFormTypeId, 1, String, Form)

	DECLARE_STRUCT_DICT(g_StringDictStructExport, BSFixedString, StringDictStructTypeId, 1, String)


	DECLARE_PRIMITIVE_DICT(g_FormDictIntExport, FORM_TYPE, SInt32, FormDictIntTypeId, 1, Form, Int)
	DECLARE_PRIMITIVE_DICT(g_FormDictFloatExport, FORM_TYPE, float, FormDictFloatTypeId, 1, Form, Float)
	DECLARE_PRIMITIVE_DICT(g_FormDictStringExport, FORM_TYPE, BSFixedString, FormDictStringTypeId, 1, Form, String)
	DECLARE_PRIMITIVE_DICT(g_FormDictFormExport, FORM_TYPE, FORM_TYPE, FormDictFormTypeId, 1, Form, Form)

	DECLARE_STRUCT_DICT(g_FormDictStructExport, FORM_TYPE, FormDictStructTypeId, 1, Form)

	void DumpAll();
	void Clear();
	bool Save(const F4SESerializationInterface* serializer);
	void RegisterLoaders();
	bool Register(VirtualMachine* vm);
};

#undef DECLARE_STRUCT_DICT
#undef DECLARE_PRIMITIVE_DICT
#undef DECLARE_STRUCT_GLOBAL
#undef DECLARE_VARIABLE
#undef BUILD_SCRIPT_NAME