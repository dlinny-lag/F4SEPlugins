#pragma once
#include "f4se/Serialization.h"
#include "f4se/GameForms.h"
#include "f4se/PapyrusValue.h"
#include "f4se/PapyrusArgs.h"
#include "f4se/PluginAPI.h"
#include "ValueHelper.hpp"
#include "StructInfo.h"

// TODO: handle structure changes between save/load
namespace StructSerialization
{
	bool SaveFieldValue(const F4SESerializationInterface* serializer, const VMValue* val);

	bool SaveStruct(const F4SESerializationInterface* serializer, const VMValue* val);

	bool LoadFieldValue(const F4SESerializationInterface* serializer, VMValue* outVal, VirtualMachine* vm);
	bool LoadStruct(const F4SESerializationInterface* serializer, VMValue* outVal, bool& allValid);
}