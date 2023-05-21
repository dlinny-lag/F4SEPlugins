#include "StructHandler.h"
#include "StructSerialization.h"

void StructHandler::SetStructName(BSFixedString& newStructName)
{
	D("SetStructName: %s", newStructName.c_str());
	structName = newStructName;
	StructInfo info;
	if (StructsInfoCache::Cache.GetInfoByName(structName, info))
	{
		structType = info.Type;
	}
	else
	{
		E("Critical: can't get structure type for %s", structName.c_str());
		StructsInfoCache::Cache.Dump();
	}
}

bool StructHandler::SaveValue(const F4SESerializationInterface* serializer, VMValue* val)
{
	return StructSerialization::SaveStruct(serializer, val);
}
bool StructHandler::LoadValue(const F4SESerializationInterface* serializer, VMValue* outVal, bool& valid)
{
	return StructSerialization::LoadStruct(serializer, outVal, valid);
}
void StructHandler::DumpValue(int indent, VMValue& value)
{

	if (value.GetTypeEnum() != VMValue::kType_Struct)
	{
		if (value.GetTypeEnum() == VMValue::kType_None)
		{
			Dmp("%s = None", Indent::Get(indent));
		}
		else
		{
			Dmp("%s: Invalid value", Indent::Get(indent));
		}
		return;
	}
	StructInfo info;
	VMStructTypeInfo* type = value.data.strct->m_type;
	if (!type)
	{
		Dmp("%s: null struct type. Expected %s", Indent::Get(indent), structName.c_str());
		return;
	}
	if (type != structType)
	{
		Dmp("%s: struct type mismatch. Expected %016X but got %016X", Indent::Get(indent), structType, type);
		return;
	}
	if (!StructsInfoCache::Cache.GetInfoByName(structName, info))
	{
		Dmp("%s: Invalid struct type. Expected %s", Indent::Get(indent), structName.c_str());
		return;
	}

	Dmp("%s%s:", Indent::Get(indent), structName.c_str());
	indent++;
	for (auto& field : info.Fields)
	{
		VMValue& fv = value.data.strct->GetStruct()[field.Index];

		if (fv.type.value > VMValue::kType_ArrayEnd)
		{ // It is Form
			VMIdentifier* object = fv.data.id;
			if (object)
			{
				const UInt32 formId = object->GetHandle();
				char* typeName = object->m_typeInfo ? object->m_typeInfo->m_typeName.c_str() : "Unknown type";
				if (!typeName)
					typeName = "";
				Dmp("%s%s:%s %08X", Indent::Get(indent), field.Name.c_str(), typeName, formId);
			}
			else
			{
				Dmp("%s%s:Form None", Indent::Get(indent), field.Name.c_str());
			}
			continue;
		}

		switch (fv.type.value)
		{
		case VMValue::kType_None:
			Dmp("%s%s:None", Indent::Get(indent), field.Name.c_str());
			break;
		case VMValue::kType_Int:
			Dmp("%s%s:%d", Indent::Get(indent), field.Name.c_str(), fv.data.i);
			break;
		case VMValue::kType_Float:
			Dmp("%s%s:%f", Indent::Get(indent), field.Name.c_str(), fv.data.f);
			break;
		case VMValue::kType_Bool:
			Dmp("%s%s:%s", Indent::Get(indent), field.Name.c_str(), S(fv.data.b));
			break;
		case VMValue::kType_String:
			{
				const char* str = fv.data.str ? fv.data.str->Get<char>() : nullptr;
				if (!str)
					str = "";
				Dmp("%s%s:%s", Indent::Get(indent), field.Name.c_str(), str);
			}
			break;
		default:
			Dmp("%s%s:Unknown", Indent::Get(indent), field.Name.c_str());
			break;
		}
	}
}