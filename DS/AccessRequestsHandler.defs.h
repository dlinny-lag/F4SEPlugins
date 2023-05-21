#pragma once

#include "CollectionAccessor.h"
#include "ArrayAPIExport.h"
#include "SetAPIExport.h"
#include "ValueDictAPIExport.h"
#include "ArrayDictAPIExport.h"

namespace DSAPI
{

// common

#define METHOD_NAME(ElementType, CollectionType, MethodName) \
	AS_IS(ElementType)AS_IS(CollectionType)AS_IS(_)AS_IS(MethodName)

#define DECLARE_METHOD0(ElementType, CollectionType, RetType, Method) \
	RetType METHOD_NAME(ElementType,CollectionType,Method) () \
	{ \
		return AS_IS(CollectionType)APIExport::AS_IS(g_)AS_IS(ElementType)AS_IS(CollectionType)Export.Method();\
	}
#define DECLARE_METHOD1(ElementType, CollectionType, RetType, Method, ArgumentType1, ArgumentName1) \
	RetType METHOD_NAME(ElementType,CollectionType,Method) (ArgumentType1 ArgumentName1) \
	{ \
		return AS_IS(CollectionType)APIExport::AS_IS(g_)AS_IS(ElementType)AS_IS(CollectionType)Export.Method(ArgumentName1);\
	}
#define DECLARE_METHOD2(ElementType, CollectionType, RetType, Method, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2) \
	RetType METHOD_NAME(ElementType,CollectionType,Method) (ArgumentType1 ArgumentName1, ArgumentType2 ArgumentName2) \
	{ \
		return AS_IS(CollectionType)APIExport::AS_IS(g_)AS_IS(ElementType)AS_IS(CollectionType)Export.Method(ArgumentName1, ArgumentName2);\
	}
#define DECLARE_METHOD3(ElementType, CollectionType, RetType, Method, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2, ArgumentType3, ArgumentName3) \
	RetType METHOD_NAME(ElementType,CollectionType,Method) (ArgumentType1 ArgumentName1, ArgumentType2 ArgumentName2, ArgumentType3 ArgumentName3) \
	{ \
		return AS_IS(CollectionType)APIExport::AS_IS(g_)AS_IS(ElementType)AS_IS(CollectionType)Export.Method(ArgumentName1, ArgumentName2, ArgumentName3);\
	}
#define DECLARE_METHOD4(ElementType, CollectionType, RetType, Method, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2, ArgumentType3, ArgumentName3, ArgumentType4, ArgumentName4) \
	RetType METHOD_NAME(ElementType,CollectionType,Method) (ArgumentType1 ArgumentName1, ArgumentType2 ArgumentName2, ArgumentType3 ArgumentName3, ArgumentType4 ArgumentName4) \
	{ \
		return AS_IS(CollectionType)APIExport::AS_IS(g_)AS_IS(ElementType)AS_IS(CollectionType)Export.Method(ArgumentName1, ArgumentName2, ArgumentName3, ArgumentName4);\
	}

#define INIT_RET_FIELD(varName, ElementType, CollectionType, Method) \
	varName.Method = &METHOD_NAME(ElementType, CollectionType, Method);

// arrays

#define DECLARE_BASE_ARRAY_METHODS(ElementName, ElementType) \
	DECLARE_METHOD1(ElementName, Array, bool, Delete, BGSKeyword*, identifier)\
	DECLARE_METHOD1(ElementName, Array, UInt32, Size, BGSKeyword*, identifier)\
	DECLARE_METHOD2(ElementName, Array, bool, Add, BGSKeyword*, identifier, ElementType, value)\
	DECLARE_METHOD2(ElementName, Array, bool, AddRange, BGSKeyword*, identifier, transferable_vector<ElementType>&&, toAdd)\
	DECLARE_METHOD2(ElementName, Array, std::optional<ElementType>, Get, BGSKeyword*, identifier, SInt32, index)\
	DECLARE_METHOD3(ElementName, Array, bool, Set, BGSKeyword*, identifier, SInt32, index, ElementType, value)\
	DECLARE_METHOD3(ElementName, Array, bool, Insert, BGSKeyword*, identifier, ElementType, value, SInt32, index)\
	DECLARE_METHOD2(ElementName, Array, bool, AppendArray, BGSKeyword*, identifier, BGSKeyword*, toAdd)\
	DECLARE_METHOD2(ElementName, Array, bool, Remove, BGSKeyword*, identifier, SInt32, index)\
	DECLARE_METHOD1(ElementName, Array, UInt32, Clear, BGSKeyword*, identifier)\
	DECLARE_METHOD0(ElementName, Array, void, DumpAll)\
	DECLARE_METHOD1(ElementName, Array, void, Dump, BGSKeyword*, identifier)

#define DECLARE_PRIMITIVE_ARRAY_METHODS(ElementName, ElementType) \
	DECLARE_BASE_ARRAY_METHODS(ElementName, ElementType) \
	DECLARE_METHOD1(ElementName, Array, bool, Create, BGSKeyword*, identifier) \
	DECLARE_METHOD3(ElementName, Array, SInt32, IndexOf, BGSKeyword*, identifier, ElementType, value, SInt32, startIndex)\
	DECLARE_METHOD2(ElementName, Array, bool, Sort, BGSKeyword*, identifier, UInt32, mode)\
	DECLARE_METHOD1(ElementName, Array, std::optional<transferable_vector<ElementType>>, ToArray, BGSKeyword*, identifier)\

#define INIT_RET_ARRAY_FIELD(varName, ElementType, Method) \
	INIT_RET_FIELD(varName, ElementType, Array, Method)

#define DECLARE_BASE_ARRAY_ACCESSOR_BODY(varName, ElementName) \
	varName.Identifier = AS_IS(ElementName)ArrayTypeId;\
	INIT_RET_ARRAY_FIELD(varName, ElementName, Delete); \
	INIT_RET_ARRAY_FIELD(varName, ElementName, Size);\
	INIT_RET_ARRAY_FIELD(varName, ElementName, Add);\
	INIT_RET_ARRAY_FIELD(varName, ElementName, AddRange);\
	INIT_RET_ARRAY_FIELD(varName, ElementName, Get);\
	INIT_RET_ARRAY_FIELD(varName, ElementName, Set);\
	INIT_RET_ARRAY_FIELD(varName, ElementName, Insert);\
	INIT_RET_ARRAY_FIELD(varName, ElementName, AppendArray);\
	INIT_RET_ARRAY_FIELD(varName, ElementName, Remove);\
	INIT_RET_ARRAY_FIELD(varName, ElementName, Clear);\
	INIT_RET_ARRAY_FIELD(varName, ElementName, DumpAll);\
	INIT_RET_ARRAY_FIELD(varName, ElementName, Dump);\

#define DECLARE_PRIMITIVE_ARRAY_ACCESSOR_BODY(varName, ElementName) \
	DECLARE_BASE_ARRAY_ACCESSOR_BODY(varName, ElementName) \
	INIT_RET_ARRAY_FIELD(varName, ElementName, Create);\
	INIT_RET_ARRAY_FIELD(varName, ElementName, IndexOf);\
	INIT_RET_ARRAY_FIELD(varName, ElementName, Sort);\
	INIT_RET_ARRAY_FIELD(varName, ElementName, ToArray);


#define DECLARE_STRUCT_ARRAY_METHODS() \
	DECLARE_BASE_ARRAY_METHODS(Struct, VMValue) \
	DECLARE_METHOD2(Struct, Array, bool, Create, BGSKeyword*, identifier, BSFixedString, structName)\
	DECLARE_METHOD4(Struct, Array, SInt32, IndexOf,BGSKeyword*, identifier, BSFixedString, fieldName, VMValue, value, SInt32, startIndex)\
	DECLARE_METHOD3(Struct, Array, bool, Sort, BGSKeyword*, identifier, BSFixedString, fieldName, UInt32, mode)

#define DECLARE_STRUCT_ARRAY_ACCESSOR_BODY(varName) \
	DECLARE_BASE_ARRAY_ACCESSOR_BODY(varName, Struct);\
	INIT_RET_ARRAY_FIELD(varName, Struct, Create);\
	INIT_RET_ARRAY_FIELD(varName, Struct, IndexOf);\
	INIT_RET_ARRAY_FIELD(varName, Struct, Sort);


// sets

#define DECLARE_SET_METHODS(ElementName, ElementType) \
	DECLARE_METHOD1(ElementName, Set, bool, Create, BGSKeyword*, identifier)\
	DECLARE_METHOD1(ElementName, Set, bool, Delete, BGSKeyword*, identifier)\
	DECLARE_METHOD1(ElementName, Set, SInt32, Size, BGSKeyword*, identifier)\
	DECLARE_METHOD2(ElementName, Set, bool, Add, BGSKeyword*, identifier, ElementType, value)\
	DECLARE_METHOD2(ElementName, Set, SInt32, AddRange, BGSKeyword*, identifier, transferable_vector<ElementType>, toAdd)\
	DECLARE_METHOD2(ElementName, Set, SInt32, RemoveRange, BGSKeyword*, identifier, transferable_vector<ElementType>, toRemove)\
	DECLARE_METHOD2(ElementName, Set, bool, Intersect, BGSKeyword*, identifier, transferable_vector<ElementType>, other)\
	DECLARE_METHOD2(ElementName, Set, bool, Contains, BGSKeyword*, identifier, ElementType, value)\
	DECLARE_METHOD2(ElementName, Set, bool, Remove, BGSKeyword*, identifier, ElementType, value)\
	DECLARE_METHOD2(ElementName, Set, bool, CopyToArray, BGSKeyword*, identifier, BGSKeyword*, arrayId)\
	DECLARE_METHOD2(ElementName, Set, SInt32, AddArray, BGSKeyword*, identifier, BGSKeyword*, arrayId)\
	DECLARE_METHOD2(ElementName, Set, SInt32, AddSet, BGSKeyword*, identifier, BGSKeyword*, setId)\
	DECLARE_METHOD2(ElementName, Set, bool, IntersectArray, BGSKeyword*, identifier, BGSKeyword*, arrayId)\
	DECLARE_METHOD2(ElementName, Set, bool, IntersectSet, BGSKeyword*, identifier, BGSKeyword*, setId)\
	DECLARE_METHOD2(ElementName, Set, SInt32, RemoveArray, BGSKeyword*, identifier, BGSKeyword*, arrayId)\
	DECLARE_METHOD2(ElementName, Set, SInt32, RemoveSet, BGSKeyword*, identifier, BGSKeyword*, setId)\
	DECLARE_METHOD1(ElementName, Set, UInt32, Clear, BGSKeyword*, identifier)\
	DECLARE_METHOD1(ElementName, Set, std::optional<transferable_vector<ElementType>>, ToArray, BGSKeyword*, identifier)\
	DECLARE_METHOD0(ElementName, Set, void, DumpAll)\
	DECLARE_METHOD1(ElementName, Set, void, Dump, BGSKeyword*, identifier)\

#define INIT_RET_SET_FIELD(varName, ElementType, Method) \
	INIT_RET_FIELD(varName, ElementType, Set, Method)

#define DECLARE_SET_ACCESSOR_BODY(varName, ElementName) \
	varName.Identifier = AS_IS(ElementName)SetTypeId;\
	INIT_RET_SET_FIELD(varName, ElementName, Create); \
	INIT_RET_SET_FIELD(varName, ElementName, Delete); \
	INIT_RET_SET_FIELD(varName, ElementName, Size); \
	INIT_RET_SET_FIELD(varName, ElementName, Add); \
	INIT_RET_SET_FIELD(varName, ElementName, AddRange); \
	INIT_RET_SET_FIELD(varName, ElementName, RemoveRange); \
	INIT_RET_SET_FIELD(varName, ElementName, Intersect); \
	INIT_RET_SET_FIELD(varName, ElementName, Contains); \
	INIT_RET_SET_FIELD(varName, ElementName, Remove); \
	INIT_RET_SET_FIELD(varName, ElementName, CopyToArray); \
	INIT_RET_SET_FIELD(varName, ElementName, AddArray); \
	INIT_RET_SET_FIELD(varName, ElementName, AddSet); \
	INIT_RET_SET_FIELD(varName, ElementName, IntersectArray); \
	INIT_RET_SET_FIELD(varName, ElementName, IntersectSet); \
	INIT_RET_SET_FIELD(varName, ElementName, RemoveArray); \
	INIT_RET_SET_FIELD(varName, ElementName, RemoveSet); \
	INIT_RET_SET_FIELD(varName, ElementName, Clear); \
	INIT_RET_SET_FIELD(varName, ElementName, ToArray); \
	INIT_RET_SET_FIELD(varName, ElementName, DumpAll); \
	INIT_RET_SET_FIELD(varName, ElementName, Dump); \


// dicts

#define DICT_NAME(KeyTypeName, ValueTypeName) AS_IS(KeyTypeName)AS_IS(Dict)AS_IS(ValueTypeName)
#define DICT_METHOD_NAME(KeyTypeName, ValueTypeName, Method) DICT_NAME(KeyTypeName, ValueTypeName)AS_IS(_)AS_IS(Method)


#define DECLARE_DICT_METHOD0(DictType, KeyTypeName, ValueTypeName, RetType, Method) \
	RetType DICT_METHOD_NAME(KeyTypeName,ValueTypeName,Method) () \
	{ \
		return AS_IS(DictType)DictAPIExport::AS_IS(g_)DICT_NAME(KeyTypeName, ValueTypeName)Export.Method();\
	}

#define DECLARE_DICT_METHOD1(DictType, KeyTypeName, ValueTypeName, RetType, Method, ArgumentType1, ArgumentName1) \
	RetType DICT_METHOD_NAME(KeyTypeName,ValueTypeName,Method) (ArgumentType1 ArgumentName1) \
	{ \
		return AS_IS(DictType)DictAPIExport::AS_IS(g_)DICT_NAME(KeyTypeName, ValueTypeName)Export.Method(ArgumentName1);\
	}

#define DECLARE_DICT_METHOD2(DictType, KeyTypeName, ValueTypeName, RetType, Method, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2) \
	RetType DICT_METHOD_NAME(KeyTypeName,ValueTypeName,Method) (ArgumentType1 ArgumentName1, ArgumentType2 ArgumentName2) \
	{ \
		return AS_IS(DictType)DictAPIExport::AS_IS(g_)DICT_NAME(KeyTypeName, ValueTypeName)Export.Method(ArgumentName1, ArgumentName2);\
	}

#define DECLARE_DICT_METHOD3(DictType, KeyTypeName, ValueTypeName, RetType, Method, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2, ArgumentType3, ArgumentName3) \
	RetType DICT_METHOD_NAME(KeyTypeName,ValueTypeName,Method) (ArgumentType1 ArgumentName1, ArgumentType2 ArgumentName2, ArgumentType3 ArgumentName3) \
	{ \
		return AS_IS(DictType)DictAPIExport::AS_IS(g_)DICT_NAME(KeyTypeName, ValueTypeName)Export.Method(ArgumentName1, ArgumentName2, ArgumentName3);\
	}

#define DECLARE_DICT_METHOD4(DictType, KeyTypeName, ValueTypeName, RetType, Method, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2, ArgumentType3, ArgumentName3, ArgumentType4, ArgumentName4) \
	RetType DICT_METHOD_NAME(KeyTypeName,ValueTypeName,Method) (ArgumentType1 ArgumentName1, ArgumentType2 ArgumentName2, ArgumentType3 ArgumentName3, ArgumentType4 ArgumentName4) \
	{ \
		return AS_IS(DictType)DictAPIExport::AS_IS(g_)DICT_NAME(KeyTypeName, ValueTypeName)Export.Method(ArgumentName1, ArgumentName2, ArgumentName3, ArgumentName4);\
	}
#define DECLARE_DICT_METHOD5(DictType, KeyTypeName, ValueTypeName, RetType, Method, ArgumentType1, ArgumentName1, ArgumentType2, ArgumentName2, ArgumentType3, ArgumentName3, ArgumentType4, ArgumentName4, ArgumentType5, ArgumentName5) \
	RetType DICT_METHOD_NAME(KeyTypeName,ValueTypeName,Method) (ArgumentType1 ArgumentName1, ArgumentType2 ArgumentName2, ArgumentType3 ArgumentName3, ArgumentType4 ArgumentName4, ArgumentType5 ArgumentName5) \
	{ \
		return AS_IS(DictType)DictAPIExport::AS_IS(g_)DICT_NAME(KeyTypeName, ValueTypeName)Export.Method(ArgumentName1, ArgumentName2, ArgumentName3, ArgumentName4, ArgumentName5);\
	}

#define DECLARE_BASE_DICT_METHODS(DictType, KeyTypeName, ValueTypeName, KeyType) \
	DECLARE_DICT_METHOD1(DictType, KeyTypeName, ValueTypeName, bool, Delete, BGSKeyword*, identifier) \
	DECLARE_DICT_METHOD1(DictType, KeyTypeName, ValueTypeName, SInt32, Size, BGSKeyword*, identifier) \
	DECLARE_DICT_METHOD2(DictType, KeyTypeName, ValueTypeName, bool, Contains, BGSKeyword*, identifier, KeyType, key) \
	DECLARE_DICT_METHOD2(DictType, KeyTypeName, ValueTypeName, bool, Remove, BGSKeyword*, identifier, KeyType, key) \
	DECLARE_DICT_METHOD1(DictType, KeyTypeName, ValueTypeName, void, Dump, BGSKeyword*, identifier) \
	DECLARE_DICT_METHOD0(DictType, KeyTypeName, ValueTypeName, void, DumpAll) \
	DECLARE_DICT_METHOD2(DictType, KeyTypeName, ValueTypeName, bool, CopyKeysToArray, BGSKeyword*, identifier, BGSKeyword*, arrayId) \
	DECLARE_DICT_METHOD2(DictType, KeyTypeName, ValueTypeName, bool, CopyKeysToSet, BGSKeyword*, identifier, BGSKeyword*, setId) \
	DECLARE_DICT_METHOD2(DictType, KeyTypeName, ValueTypeName, SInt32, RemoveKeysByArray, BGSKeyword*, identifier, BGSKeyword*, arrayId) \
	DECLARE_DICT_METHOD2(DictType, KeyTypeName, ValueTypeName, SInt32, RemoveKeysBySet, BGSKeyword*, identifier, BGSKeyword*, setId) \
	DECLARE_DICT_METHOD1(DictType, KeyTypeName, ValueTypeName, std::optional<transferable_vector<KeyType>>, KeysToArray, BGSKeyword*, identifier)

#define DECLARE_BASE_VALUE_DICT_METHODS(KeyTypeName, ValueTypeName, KeyType, ValueType) \
	DECLARE_BASE_DICT_METHODS(Value, KeyTypeName, ValueTypeName, KeyType) \
	DECLARE_DICT_METHOD3(Value, KeyTypeName, ValueTypeName, bool, Add, BGSKeyword*, identifier, KeyType, key, ValueType, value) \
	DECLARE_DICT_METHOD3(Value, KeyTypeName, ValueTypeName, bool, Set, BGSKeyword*, identifier, KeyType, key, ValueType, value) \
	DECLARE_DICT_METHOD2(Value, KeyTypeName, ValueTypeName, std::optional<ValueType>, Get, BGSKeyword*, identifier, KeyType, key) \
	DECLARE_DICT_METHOD3(Value, KeyTypeName, ValueTypeName, bool, Replace, BGSKeyword*, identifier, KeyType, key, ValueType, newValue) \

#define DECLARE_PRIMITIVE_VALUE_DICT_METHODS(KeyTypeName, ValueTypeName, KeyType, ValueType) \
	DECLARE_BASE_VALUE_DICT_METHODS(KeyTypeName, ValueTypeName, KeyType, ValueType) \
	DECLARE_DICT_METHOD1(Value, KeyTypeName, ValueTypeName, bool, Create, BGSKeyword*, identifier)

#define DECLARE_STRUCT_VALUE_DICT_METHODS(KeyTypeName, KeyType) \
	DECLARE_BASE_VALUE_DICT_METHODS(KeyTypeName, Struct, KeyType, VMValue) \
	DECLARE_DICT_METHOD2(Value, KeyTypeName, Struct, bool, Create, BGSKeyword*, identifier, BSFixedString, structName)

#define DECLARE_BASE_ARRAY_DICT_METHODS(KeyTypeName, ArrayValueTypeName, KeyType, ValueType) \
	DECLARE_BASE_DICT_METHODS(Array, KeyTypeName, ArrayValueTypeName, KeyType) \
	DECLARE_DICT_METHOD3(Array, KeyTypeName, ArrayValueTypeName, std::optional<ValueType>, GetElement, BGSKeyword*, identifier, KeyType, key, UInt32, index) \
	DECLARE_DICT_METHOD3(Array, KeyTypeName, ArrayValueTypeName, bool, AddElement, BGSKeyword*, identifier, KeyType, key, ValueType, value) \
	DECLARE_DICT_METHOD3(Array, KeyTypeName, ArrayValueTypeName, bool, AddRange, BGSKeyword*, identifier, KeyType, key, transferable_vector<ValueType>, toAdd) \
	DECLARE_DICT_METHOD4(Array, KeyTypeName, ArrayValueTypeName, bool, SetElement, BGSKeyword*, identifier, KeyType, key, ValueType, value, SInt32, index) \
	DECLARE_DICT_METHOD4(Array, KeyTypeName, ArrayValueTypeName, bool, InsertElement, BGSKeyword*, identifier, KeyType, key, ValueType, value, SInt32, index) \
	DECLARE_DICT_METHOD3(Array, KeyTypeName, ArrayValueTypeName, bool, RemoveAtIndex, BGSKeyword*, identifier, KeyType, key, SInt32, index) \
	DECLARE_DICT_METHOD2(Array, KeyTypeName, ArrayValueTypeName, SInt32, ArrayLength, BGSKeyword*, identifier, KeyType, key) \
	DECLARE_DICT_METHOD3(Array, KeyTypeName, ArrayValueTypeName, bool, AppendArray, BGSKeyword*, identifier, KeyType, key, BGSKeyword*, arrayId) \
	DECLARE_DICT_METHOD3(Array, KeyTypeName, ArrayValueTypeName, bool, CopyToArray, BGSKeyword*, identifier, KeyType, key, BGSKeyword*, targetArrayId)

#define DECLARE_PRIMITIVE_ARRAY_DICT_METHODS(KeyTypeName, ValueTypeName, KeyType, ValueType) \
	DECLARE_BASE_ARRAY_DICT_METHODS(KeyTypeName, AS_IS(ValueTypeName)AS_IS(Array), KeyType, ValueType) \
	DECLARE_DICT_METHOD1(Array, KeyTypeName, AS_IS(ValueTypeName)AS_IS(Array), bool, Create, BGSKeyword*, identifier) \
	DECLARE_DICT_METHOD4(Array, KeyTypeName, AS_IS(ValueTypeName)AS_IS(Array), SInt32, IndexOf, BGSKeyword*, identifier, KeyType, key, ValueType, value, SInt32, startIndex) \
	DECLARE_DICT_METHOD3(Array, KeyTypeName, AS_IS(ValueTypeName)AS_IS(Array), bool, SortArray, BGSKeyword*, identifier, KeyType, key, UInt32, mode) \

#define DECLARE_STRUCT_ARRAY_DICT_METHODS(KeyTypeName, KeyType) \
	DECLARE_BASE_ARRAY_DICT_METHODS(KeyTypeName, StructArray, KeyType, VMValue) \
	DECLARE_DICT_METHOD2(Array, KeyTypeName, StructArray, bool, Create, BGSKeyword*, identifier, BSFixedString, structName) \
	DECLARE_DICT_METHOD5(Array, KeyTypeName, StructArray, SInt32, IndexOf, BGSKeyword*, identifier, KeyType, key, BSFixedString, fieldName, VMValue, value, SInt32, startIndex) \
	DECLARE_DICT_METHOD4(Array, KeyTypeName, StructArray, bool, SortArray, BGSKeyword*, identifier, KeyType, key, BSFixedString, fieldName, UInt32, mode) \



#define INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, Method) \
	varName.Method = &DICT_METHOD_NAME(KeyTypeName, ValueTypeName, Method);

#define INIT_BASE_DICT_RET_FIELDS(varName, KeyTypeName, ValueTypeName) \
	varName.Identifier = DICT_NAME(KeyTypeName, ValueTypeName)TypeId;\
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, Delete) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, Size) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, Contains) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, Remove) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, Dump) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, DumpAll) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, CopyKeysToArray) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, CopyKeysToSet) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, RemoveKeysByArray) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, RemoveKeysBySet) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, KeysToArray)

#define INIT_BASE_VALUE_DICT_RET_FIELDS(varName, KeyTypeName, ValueTypeName) \
	INIT_BASE_DICT_RET_FIELDS(varName, KeyTypeName, ValueTypeName) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, Add) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, Set) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, Get) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, Replace)

#define DECLARE_PRIMITIVE_VALUE_DICT_BODY(varName, KeyTypeName, ValueTypeName) \
	INIT_BASE_VALUE_DICT_RET_FIELDS(varName, KeyTypeName, ValueTypeName) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ValueTypeName, Create)

#define DECLARE_STRUCT_VALUE_DICT_BODY(varName, KeyTypeName) \
	INIT_BASE_VALUE_DICT_RET_FIELDS(varName, KeyTypeName, Struct) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, Struct, Create)

#define INIT_BASE_ARRAY_DICT_RET_FIELDS(varName, KeyTypeName, ArrayValueTypeName) \
	INIT_BASE_DICT_RET_FIELDS(varName, KeyTypeName, ArrayValueTypeName) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ArrayValueTypeName, GetElement) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ArrayValueTypeName, AddElement) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ArrayValueTypeName, AddRange) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ArrayValueTypeName, SetElement) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ArrayValueTypeName, InsertElement) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ArrayValueTypeName, RemoveAtIndex) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ArrayValueTypeName, ArrayLength) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ArrayValueTypeName, AppendArray) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, ArrayValueTypeName, CopyToArray)

#define DECLARE_PRIMITIVE_ARRAY_DICT_BODY(varName, KeyTypeName, ValueTypeName) \
	INIT_BASE_ARRAY_DICT_RET_FIELDS(varName, KeyTypeName, AS_IS(ValueTypeName)AS_IS(Array)) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, AS_IS(ValueTypeName)AS_IS(Array), Create) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, AS_IS(ValueTypeName)AS_IS(Array), IndexOf) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, AS_IS(ValueTypeName)AS_IS(Array), SortArray)

#define DECLARE_STRUCT_ARRAY_DICT_BODY(varName, KeyTypeName) \
	INIT_BASE_ARRAY_DICT_RET_FIELDS(varName, KeyTypeName, StructArray) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, StructArray, Create) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, StructArray, IndexOf) \
	INIT_DICT_RET_FIELD(varName, KeyTypeName, StructArray, SortArray)
}


namespace DSAPI
{ // declarations

	// arrays
	DECLARE_PRIMITIVE_ARRAY_METHODS(Int, SInt32)
	DECLARE_PRIMITIVE_ARRAY_METHODS(Float, float)
	DECLARE_PRIMITIVE_ARRAY_METHODS(String, BSFixedString)
	DECLARE_PRIMITIVE_ARRAY_METHODS(Form, FORM_TYPE)
	DECLARE_STRUCT_ARRAY_METHODS()

	// sets
	
	DECLARE_SET_METHODS(Int, SInt32)
	DECLARE_SET_METHODS(Float, float)
	DECLARE_SET_METHODS(String, BSFixedString)
	DECLARE_SET_METHODS(Form, FORM_TYPE)

	// dicts

	DECLARE_PRIMITIVE_VALUE_DICT_METHODS(Int, Int, SInt32, SInt32)
	DECLARE_PRIMITIVE_VALUE_DICT_METHODS(Int, Float, SInt32, float)
	DECLARE_PRIMITIVE_VALUE_DICT_METHODS(Int, String, SInt32, BSFixedString)
	DECLARE_PRIMITIVE_VALUE_DICT_METHODS(Int, Form, SInt32, FORM_TYPE)
	DECLARE_STRUCT_VALUE_DICT_METHODS(Int, SInt32)

	DECLARE_PRIMITIVE_VALUE_DICT_METHODS(String, Int, BSFixedString, SInt32)
	DECLARE_PRIMITIVE_VALUE_DICT_METHODS(String, Float, BSFixedString, float)
	DECLARE_PRIMITIVE_VALUE_DICT_METHODS(String, String, BSFixedString, BSFixedString)
	DECLARE_PRIMITIVE_VALUE_DICT_METHODS(String, Form, BSFixedString, FORM_TYPE)
	DECLARE_STRUCT_VALUE_DICT_METHODS(String, BSFixedString)

	DECLARE_PRIMITIVE_VALUE_DICT_METHODS(Form, Int, FORM_TYPE, SInt32)
	DECLARE_PRIMITIVE_VALUE_DICT_METHODS(Form, Float, FORM_TYPE, float)
	DECLARE_PRIMITIVE_VALUE_DICT_METHODS(Form, String, FORM_TYPE, BSFixedString)
	DECLARE_PRIMITIVE_VALUE_DICT_METHODS(Form, Form, FORM_TYPE, FORM_TYPE)
	DECLARE_STRUCT_VALUE_DICT_METHODS(Form, FORM_TYPE)


	DECLARE_PRIMITIVE_ARRAY_DICT_METHODS(Int, Int, SInt32, SInt32)
	DECLARE_PRIMITIVE_ARRAY_DICT_METHODS(Int, Float, SInt32, float)
	DECLARE_PRIMITIVE_ARRAY_DICT_METHODS(Int, String, SInt32, BSFixedString)
	DECLARE_PRIMITIVE_ARRAY_DICT_METHODS(Int, Form, SInt32, FORM_TYPE)
	DECLARE_STRUCT_ARRAY_DICT_METHODS(Int, SInt32)

	DECLARE_PRIMITIVE_ARRAY_DICT_METHODS(String, Int, BSFixedString, SInt32)
	DECLARE_PRIMITIVE_ARRAY_DICT_METHODS(String, Float, BSFixedString, float)
	DECLARE_PRIMITIVE_ARRAY_DICT_METHODS(String, String, BSFixedString, BSFixedString)
	DECLARE_PRIMITIVE_ARRAY_DICT_METHODS(String, Form, BSFixedString, FORM_TYPE)
	DECLARE_STRUCT_ARRAY_DICT_METHODS(String, BSFixedString)

	DECLARE_PRIMITIVE_ARRAY_DICT_METHODS(Form, Int, FORM_TYPE, SInt32)
	DECLARE_PRIMITIVE_ARRAY_DICT_METHODS(Form, Float, FORM_TYPE, float)
	DECLARE_PRIMITIVE_ARRAY_DICT_METHODS(Form, String, FORM_TYPE, BSFixedString)
	DECLARE_PRIMITIVE_ARRAY_DICT_METHODS(Form, Form, FORM_TYPE, FORM_TYPE)
	DECLARE_STRUCT_ARRAY_DICT_METHODS(Form, FORM_TYPE)

}