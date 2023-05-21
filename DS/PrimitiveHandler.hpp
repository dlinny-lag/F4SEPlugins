#pragma once
#include "ValueHandler.hpp"
#include "ValueHelper.hpp"

template <typename T>
class PrimitiveHandler final : public ValueHandler<T>
{
public:
	bool SaveValue(const F4SESerializationInterface* serializer, T* val) override
	{
		return ValueHelper::Save(serializer, val);
	}

	bool LoadValue(const F4SESerializationInterface* serializer, T* outVal, bool& valid) override
	{
		valid = true; // primitive value is always valid
		return ValueHelper::Load(serializer, outVal);
	}

	void DumpValue(int indent, T& value) override
	{
		ValueHelper::Dump(indent, value);
	}
};

