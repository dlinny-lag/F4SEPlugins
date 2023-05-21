#pragma once
#include "f4se/PluginAPI.h"
#include "f4se/GameForms.h"
#include "f4se/PapyrusValue.h"
#include "f4se/PapyrusArgs.h"
struct StructField
{
	enum FieldType : UInt8
	{
		None = VMValue::kType_None,
		String = VMValue::kType_String,
		Int = VMValue::kType_Int,
		Float = VMValue::kType_Float,
		Bool = VMValue::kType_Bool,
		Form = 100
	};

	BSFixedString Name;
	UInt32 Index;
	FieldType Type;
	// non null if Type is Form
	VMObjectTypeInfo* ComplexType;

	bool Save(const F4SESerializationInterface* serializer) const;
	bool Load(const F4SESerializationInterface* serializer);
	void Dump(int indent);

	struct Less
	{
		bool operator()(const StructField& a, const StructField& b) const
		{
			return a.Index < b.Index;
		}
	};
};




