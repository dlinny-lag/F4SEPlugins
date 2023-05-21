#pragma once
#include "ValueHandler.hpp"
#include "f4se/PapyrusValue.h"
#include "StructsInfoCache.h"

class StructHandler final : public ValueHandler<VMValue>
{
	BSFixedString structName;
	VMStructTypeInfo* structType;
public:
	StructHandler()
		:ValueHandler()
	    ,structName("")
		,structType(nullptr)

	{
	}
	StructHandler(BSFixedString& structName):
		ValueHandler()
	{
		SetStructName(structName);
	}
	void SetStructName(BSFixedString& newStructName);
	bool SaveValue(const F4SESerializationInterface* serializer, VMValue* val) override;
	bool LoadValue(const F4SESerializationInterface* serializer, VMValue* outVal, bool& valid) override;
	void DumpValue(int indent, VMValue& value) override;
};

