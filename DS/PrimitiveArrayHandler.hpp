#pragma once
#include "PrimitiveArray.hpp"
#include "ValueHandler.hpp"

template <typename TValue>
class PrimitiveArrayHandler : public ValueHandler<DS::PrimitiveArray<TValue>>
{
public:
	bool SaveValue(const F4SESerializationInterface* serializer, DS::PrimitiveArray<TValue>* val) override
	{
		return val->SaveData(serializer, true);
	}

	bool LoadValue(const F4SESerializationInterface* serializer, DS::PrimitiveArray<TValue>* outVal, bool& valid) override
	{
		valid = true;
		return outVal->LoadData(serializer);
	}

	void DumpValue(int indent, DS::PrimitiveArray<TValue>& value) override
	{
		value.DumpAll(indent);
	}
};