#include "StructInfo.h"
#include "MyTrace.h"
#include "f4se/PapyrusVM.h"
#include <functional>
#include "Shared/SerializationHelper.h"
#include "ValueHelper.hpp"
#include "Shared/BSFixedStringEqual.hpp"
#include "BSReferenceTypeUtils.h"
#include <algorithm>
#include "StructValueHelper.hpp"

bool ExtractStructInfoInternal(VMStructTypeInfo* structType, StructInfo* out)
{
	StructInfo retVal;
	retVal.Name = structType->m_typeName;
	retVal.Type = structType;
	bool succeed = true;
	structType->m_members.ForEach([&retVal, structType, &succeed](const VMStructTypeInfo::MemberItem* item)
	{
		StructField field;
		const UInt64 typeDescr = structType->m_data[item->index].m_type;
		D("Field %s with index %d has type %016X", item->name.c_str(), item->index, typeDescr);
		if (typeDescr >= VMValue::kType_ArrayEnd)
		{// it is not a primitive type
			VMObjectTypeInfo* fieldTypeInfo = reinterpret_cast<VMObjectTypeInfo*>(typeDescr & ~0x01ULL);
			const bool isForm = BSReferenceTypeUtils::IsForm(fieldTypeInfo);
			if (!isForm)
			{
				succeed = false;
				return TW("[%s.%s] is not form or primitive", retVal.Name.c_str(), item->name.c_str());
			}
			field.Type = StructField::FieldType::Form;
			field.ComplexType = fieldTypeInfo;
		}
		else
		{
			if (typeDescr >= StructField::FieldType::String && typeDescr <= StructField::FieldType::Bool)
			{
				field.Type = static_cast<StructField::FieldType>(typeDescr);
			}
			else
			{
				succeed = false;
				return TW("Unsupported type %d of [%s.%s]", (UInt32)typeDescr, retVal.Name.c_str(), item->name.c_str());
			}
		}

		field.Name = item->name;
		field.Index = item->index;
		retVal.Fields.push_back(field);
		return true;
	});

	if (succeed)
	{
		std::sort(retVal.Fields.begin(), retVal.Fields.end(), StructField::Less());
		*out = retVal;
	}
	return succeed;
}

bool StructInfo::ExtractStructInfoFromName(const BSFixedString& structName, StructInfo* out)
{
	VMStructTypeInfo* structType = nullptr;
	VirtualMachine* vm = (*g_gameVM)->m_virtualMachine;
	if (vm->GetStructTypeInfo(&structName, &structType))
	{
		structType->Release();
		return ExtractStructInfoInternal(structType, out);
	}
	return FE("Can't find type info of structure %s", structName.c_str());
}
bool StructInfo::ValidateObject(const VMValue& val, const BSFixedString& expectedStructName)
{
	const auto structType = StructValueHelper::GetStructType(val);
	if (!structType)
		return FE("Can't get structure type info");
	return std::equal_to()(structType->m_typeName, expectedStructName);
}

bool Equals(const StructInfo& a, const StructInfo& b)
{
	for (const auto& aField : a.Fields)
	{
		bool found = false;
		for (const auto& bField : b.Fields)
		{
			if (std::equal_to()(aField.Name, bField.Name))
			{
				if (aField.Type != bField.Type)
					return false;
				found = true;
				break;
			}
		}
		if (!found)
			return false;
	}
	return true;
}

bool StructInfo::Save(const F4SESerializationInterface* serializer) const
{
	if (!SerializationHelper::WriteString(serializer, &Name))
		return FE("Failed to write structure name");

	const UInt32 size = Fields.size();
	if (!ValueHelper::Save(serializer, &size))
		return FE("Failed to write struct fields size");

	for (const auto& field : Fields)
	{
		if (!field.Save(serializer))
			return false;
	}
	return true;
}

bool StructInfo::Load(const F4SESerializationInterface* serializer)
{
	if (!SerializationHelper::ReadString(serializer, &Name))
		return FE("Failed to read structure name");
	UInt32 size;
	if (!ValueHelper::Load(serializer, &size))
		return FE("Failed to read structure fields size");
	for (UInt32 i = 0; i < size; i++)
	{
		StructField field;
		if (!field.Load(serializer))
			continue;
		Fields.push_back(field);
	}
	// validate structure
	StructInfo actual;
	if (!ExtractStructInfoFromName(Name, &actual))
		return FE("Load: structure %s not found", Name.c_str());
	if (!Equals(*this, actual))
		return FE("Loaded struct %s is not equal to existing", Name.c_str());
	return true;
}