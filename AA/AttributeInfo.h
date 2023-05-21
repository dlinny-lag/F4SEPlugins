#pragma once
#include <random>

#include "f4se/GameForms.h"
#include "f4se/PluginAPI.h"
struct AttributeInfo
{
	enum InitMode
	{
		Exact = -1,
		Default = 0,
		Uniform = 1,
		Normal = 2,
	};
	ActorValueInfo* AV = nullptr;
	UInt32 Id = 0;
	float DefaultValue = 0;
	float MinValue = 0;
	float DefaultBase = 0;
	float RandomMin = 0;
	float RandomMax = 0;
	float RandomMean = 0;
	float RandomDeviation = 0;
	void InitDistributions();
	static bool Load(const F4SESerializationInterface* serializer, AttributeInfo& retVal);
	bool Save(const F4SESerializationInterface* serializer) const;
	[[nodiscard]] bool IsValid() const;
	[[nodiscard]] float Crop(float value) const;
	[[nodiscard]] float GetRandom(InitMode mode);  // value is cropped
	[[nodiscard]] float GetRandomUniform(float min, float max) const;
	[[nodiscard]] static float GetRandomNormal(float mean, float deviation); // value not cropped
	void Print(UInt32 id) const;

	[[nodiscard]] static InitMode ModeFromInt(UInt32 mode)
	{
		if (mode < Default || mode > Normal)
			mode = Default;
		return static_cast<InitMode>(mode);
	}

private:
	std::normal_distribution<float> normal;
	std::uniform_real_distribution<float> uniform;
};

typedef AttributeInfo::InitMode AttributeInitMode;


