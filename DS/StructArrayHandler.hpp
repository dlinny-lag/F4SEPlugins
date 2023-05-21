#pragma once
#include "StructArray.hpp"
#include "ValueHandler.hpp"

class StructArrayHandler : public ValueHandler<DS::StructArray>
{
	BSFixedString structName;
public:
	StructArrayHandler(BSFixedString& structName)
		:ValueHandler()
		,structName(structName.c_str())
	{

	}
	StructArrayHandler() 
		:ValueHandler()
		,structName("")
	{

	}
	void SetStructName(BSFixedString& newStructName)
	{
		BSFixedString newVal(newStructName.c_str());
		structName = newVal;
	}

	bool SaveValue(const F4SESerializationInterface* serializer, DS::StructArray* val) override
	{
		return val->SaveData(serializer, true);
	}

	bool LoadValue(const F4SESerializationInterface* serializer, DS::StructArray* outVal, bool& valid) override
	{
		valid = true;
		return outVal->LoadData(serializer);
	}

	void DumpValue(int indent, DS::StructArray& value) override
	{
		value.DumpAll(indent);
	}
};