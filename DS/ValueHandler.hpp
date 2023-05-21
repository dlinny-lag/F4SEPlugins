#pragma once
#include "f4se/PluginAPI.h"

template <typename TValue>
class ValueHandler
{
public:
	virtual bool SaveValue(const F4SESerializationInterface* serializer, TValue* val) = 0;
	virtual bool LoadValue(const F4SESerializationInterface* serializer, TValue* outVal, bool& valid) = 0;
	virtual void DumpValue(int indent, TValue& value) = 0;
	virtual ~ValueHandler() = default;
};