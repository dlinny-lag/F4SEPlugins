#pragma once
#include "f4se/Serialization.h"
#include "f4se/GameForms.h"
#include "f4se/PapyrusValue.h"
#include "f4se/PapyrusArgs.h"
#include "f4se/PluginAPI.h"
#include "ValueHelper.hpp"
#include "StructInfo.h"
#include "StructSerialization.h"

bool StructSerialization::SaveFieldValue(const F4SESerializationInterface* serializer, const VMValue* val)
{
	D("Saving field type %016X", val->type.value);
	if (!ValueHelper::Save(serializer, &val->type.value))
		return FE("Failed to save type of field");

	if (val->type.value > VMValue::kType_ArrayEnd)
	{
		UInt64 handle = 0;
		BSFixedString typeName;

		VMIdentifier* object = val->data.id;
		if (object)
		{
			handle = object->GetHandle();
			VMObjectTypeInfo* typeObject = object->m_typeInfo;
			if (typeObject)
				typeName = typeObject->m_typeName;
		}

		D("Saving [%s] object with id = %016X", typeName.c_str(), handle);

		if (!ValueHelper::Save(serializer, &typeName))
			return FE("Failed to save identifier type name");
		if (!ValueHelper::Save(serializer, &handle))
			return FE("Failed to save identifier handle");
		return true;
	}

	const UInt8 typeId = val->GetTypeEnum();
	switch (typeId)
	{
		case VMValue::kType_None:
		{
			D("None object. Nothing to save");
			return true; // nothing to write
		}
		case VMValue::kType_String:
		{
			D("Saving string value %s", val->data.GetStr()->c_str());
			if (!ValueHelper::Save(serializer, val->data.GetStr()))
				return FE("Failed to save string field value");
			return true;
		}
		case VMValue::kType_Int:
		{
			D("Saving int value %d", val->data.i);
			if (!ValueHelper::Save(serializer, &val->data.i))
				return FE("Failed to save int field value");
			return true;
		}
		case VMValue::kType_Float:
		{
			D("Saving float value %f", val->data.f);
			if (!ValueHelper::Save(serializer, &val->data.f))
				return FE("Failed to save float field value");
			return true;
		}
		case VMValue::kType_Bool:
		{
			D("Saving bool value %s", S(val->data.b));
			if (!ValueHelper::Save(serializer, &val->data.b))
				return FE("Failed to save bool field value");
			return true;
		}
	}
	return FE("SaveFieldValue: Unexpected type id %" PRIu8, typeId);
}

bool StructSerialization::SaveStruct(const F4SESerializationInterface* serializer, const VMValue* val)
{
	const UInt8 typeId = val->GetTypeEnum();

	if (typeId != VMValue::kType_Struct && typeId != VMValue::kType_None)
		return FE("SaveStruct: invalid argument type %" PRIu8, typeId);
		
	if (!ValueHelper::Save(serializer, &typeId))
		return FE("Failed to save struct object type");

	if (typeId == VMValue::kType_None)
	{
		D("None struct written");
		return true; // Nothing to save
	}
	VMValue::StructData* structObject = val->data.strct;
	VMStructTypeInfo* structTypeInfo = structObject->m_type;

	D("Saving structure name [%s]", structTypeInfo->m_typeName.c_str());
	if (!ValueHelper::Save(serializer, &structTypeInfo->m_typeName))
		return FE("Failed to save struct name");

	UInt32 size = structTypeInfo->m_members.FillCount();
	D("Saving structure members size (%d)", size);
	if (!ValueHelper::Save(serializer, &size))
		return FE("Failed to save struct fields size");

	VMValue* structValue = structObject->GetStruct();
	bool succeed = true;
	structTypeInfo->m_members.ForEach([&serializer, &structValue, &succeed](VMStructTypeInfo::MemberItem* fInfo)
	{
		D("Saving field [%s] name", fInfo->name.c_str());
		if (!ValueHelper::Save(serializer, &fInfo->name)) 		// field name
		{
			succeed = false;
			return FE("Failed to save struct field name");
		}
		D("Saving field [%s] value...", fInfo->name.c_str());
		if (!SaveFieldValue(serializer, &structValue[fInfo->index]))	// field value
		{
			succeed = false;
			return FE("Failed to save struct field value");
		}
		return true;
	});
	return succeed;
}

bool StructSerialization::LoadFieldValue(const F4SESerializationInterface* serializer, VMValue* outVal, VirtualMachine* vm)
{
	UInt64 typeId = -1;
	D("Loading field type");
	if (!ValueHelper::Load(serializer, &typeId))
		return FE("Failed to load field type id");
	D("Loading value for field with type %016X", typeId);
	if (typeId > VMValue::kType_ArrayEnd)
	{
		BSFixedString typeName;
		D("Loading field type namme");
		if (!ValueHelper::Load(serializer, &typeName))
			return FE("Failed to load identifier type name");

		D("Loading handle for [%s] object", typeName.c_str());
		UInt64 handle;
		if (!ValueHelper::Load(serializer, &handle))
			return FE("Failed to load identifier handle");

		D("Resolving handle %016X", handle);
		if (handle != 0 && !SerializationHelper::ResolveHandle(serializer, handle, &handle))
		{
			handle = 0;
		}

		GetIdentifier(outVal, handle, &typeName, vm);
		return true;
	}

	switch (typeId)
	{
		case VMValue::kType_None:
		{
			outVal->SetNone();
			D("None value. Nothing to load");
			return true;
		}
		case VMValue::kType_String:
		{
			BSFixedString str;
			D("Loading string value");
			if (!ValueHelper::Load(serializer, &str))
				return FE("Failed to load string field value");
			outVal->SetString(str);
			D("Loaded [%s] value", str.c_str());
			return true;
		}
		case VMValue::kType_Int:
		{
			D("Loading int value");
			SInt32 i = 0;
			if (!ValueHelper::Load(serializer, &i))
				return FE("Failed to load int field value");
			outVal->SetInt(i);
			D("Loaded [%d] value", i);
			return true;
		}
		case VMValue::kType_Float:
		{
			D("Loading float value");
			float f = 0.0f;
			if (!ValueHelper::Load(serializer, &f))
				return FE("Failed to load float field value");
			outVal->SetFloat(f);
			D("Loaded [%f] value", f);
			return true;
		}
		case VMValue::kType_Bool:
		{
			D("Loading bool value");
			bool b = false;
			if (!ValueHelper::Load(serializer, &b))
				return FE("Failed to load bool field value");
			outVal->SetBool(b);
			D("Loaded [%s] value", S(b));
			return true;
		}
	}
	return FE("LoadFieldValue: Unexpected type id %" PRIu8, typeId);
}
bool StructSerialization::LoadStruct(const F4SESerializationInterface* serializer, VMValue* outVal, bool& allValid)
{
	allValid = false;
	UInt8 typeId;
	if (!ValueHelper::Load(serializer, &typeId))
		return FE("Failed to load struct object type");

	if (typeId == VMValue::kType_None)
	{
		outVal->SetNone();
		D("None struct read");
		allValid = true; // None was saved, so it is valid value
		return true; // Nothing to load
	}

	BSFixedString structName;
	D("Loading structure name...");
	if (!ValueHelper::Load(serializer, &structName))
		return FE("Failed to load structure name");

	D("Loading [%s] structure members size", structName.c_str());
	UInt32 size;
	if (!ValueHelper::Load(serializer, &size))
		return FE("Failed to load structure fields size");

	VMStructTypeInfo* structTypeInfo = nullptr;
	VirtualMachine* vm = (*g_gameVM)->m_virtualMachine;
	if (vm->GetStructTypeInfo(&structName, &structTypeInfo))
	{
		if (structTypeInfo)
			structTypeInfo->Release();
	}
	else
	{
		structTypeInfo = nullptr;
	}

	D("Struct type resolved as %016X", structTypeInfo);

	outVal->SetComplexType(structTypeInfo);

	VMValue::StructData* structObject = nullptr;
	if (structTypeInfo)
	{
		D("Creating struct object...");
		if (vm->CreateStruct(&structName, &structObject))
		{
			// Setup all the empty types? Need to confirm whether CreateStruct already does this
			structTypeInfo->m_members.ForEach([&structObject, &structTypeInfo](const VMStructTypeInfo::MemberItem* fInfo)
				{
					structObject->GetStruct()[fInfo->index].type.value = structTypeInfo->m_data.entries[fInfo->index].m_type;
					return true;
				});
			D("Struct object instantiated");
		}
		else
		{
			E("Failed to create structure %s object", structName.c_str());
		}
	}
	else
	{
		W("Structure %s is missing", structName.c_str());
	}
	for (UInt32 i = 0; i < size; i++)
	{
		BSFixedString fieldName;
		D("Loading field name...");
		if (!ValueHelper::Load(serializer, &fieldName))
			return FE("Failed to load field name");

		D("Loading [%s.%s] field...", structName.c_str(), fieldName.c_str());
		VMValue fieldValue;
		if (!LoadFieldValue(serializer, &fieldValue, vm))
			return false;

		if (structObject)
		{
			auto fInfo = structTypeInfo->m_members.Find(&fieldName);
			if (fInfo)
				structObject->GetStruct()[fInfo->index] = fieldValue;
		}
	}

		
	if (structObject)
	{
		outVal->data.strct = structObject;
		allValid = true;
	}
	else
	{
		outVal->SetNone();
		allValid = false; // something non None was save, so it is not valid value
		D("Loaded %s struct value as None", structName.c_str());
	}
	return true;
}
