#pragma once
#include "f4se/Serialization.h"
#include "f4se/GameForms.h"
class Identifiable
{
	BGSKeyword* identifier;
public:
	Identifiable(BGSKeyword* identifier):
		identifier(identifier)
	{

	}
	~Identifiable() = default;
	BGSKeyword* Id() const noexcept;
	UInt32 FormId() const noexcept;
	bool SaveId(const F4SESerializationInterface* serializer, bool allowNoId) const;
	static bool LoadId(const F4SESerializationInterface* serializer, UInt32* outId);
	static BGSKeyword* TryResolveId(const F4SESerializationInterface* serializer, UInt32 id);
protected:
	void SetId(BGSKeyword* id);
};

