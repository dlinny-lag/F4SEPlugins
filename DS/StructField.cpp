#include "StructField.h"
#include "ValueHelper.hpp"
#include "Shared/MyTrace.h"
#include "Shared/Indent.h"

bool StructField::Save(const F4SESerializationInterface* serializer) const
{
	if (!ValueHelper::Save(serializer, &Name))
		return FE("Failed to save struct field name");
	UInt8 type = static_cast<UInt8>(Type);
	if (!ValueHelper::Save(serializer, &type))
		return FE("Failed to save struct field type id");
	return true;
}
bool StructField::Load(const F4SESerializationInterface* serializer)
{
	if (!ValueHelper::Load(serializer, &Name))
		return FE("Failed to load struct field name");
	UInt8 type;
	if (!ValueHelper::Load(serializer, &type))
		return FE("Failed to load struct field type id");
	Type = static_cast<StructField::FieldType>(type);
	return true;
}

const char* ToString(StructField::FieldType type)
{
	switch (type)
	{
	case StructField::FieldType::None:
		return "None";
	case StructField::FieldType::String:
		return "string";
	case StructField::FieldType::Int:
		return "int";
	case StructField::FieldType::Float:
		return "float";
	case StructField::FieldType::Bool:
		return "bool";
	case StructField::FieldType::Form:
		return "Form";
	}
	return "undefined";
}

void StructField::Dump(int indent)
{
	Dmp("%s%s %s", Indent::Get(indent), ToString(Type), Name.c_str());
}