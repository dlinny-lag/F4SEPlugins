#pragma once
#include "f4se/GameForms.h"
#include "f4se/PapyrusValue.h"
#include "f4se/PapyrusArgs.h"
#include "f4se/PluginAPI.h"
#include <vector>
#include "StructField.h"

struct StructInfo
{
	BSFixedString Name;
	VMStructTypeInfo* Type;
	std::vector<StructField> Fields;
	static bool ExtractStructInfoFromName(const BSFixedString& structName, StructInfo* out);
	static bool ValidateObject(const VMValue& val, const BSFixedString& expectedStructName);

	// TODO: Save/Load can be used to detect changes in struct definition. but most likely it is not necessary and Save/Load should be removed
	bool Save(const F4SESerializationInterface* serializer) const;
	bool Load(const F4SESerializationInterface* serializer);
};
