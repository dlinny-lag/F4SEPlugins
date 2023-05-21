#include "AttributeInfo.h"
#include "MyTrace.h"

thread_local std::mt19937 gen(std::random_device{}());

void AttributeInfo::InitDistributions()
{
	normal.param(std::normal_distribution<float>::param_type(RandomMean, 
		RandomDeviation > 0 ? RandomDeviation : std::numeric_limits<float>::epsilon())); // when RandomDeviation is invalid it will not be used, but we should initialize object
	uniform.param(std::uniform_real_distribution<float>::param_type(RandomMin, RandomMax));
}

bool AttributeInfo::IsValid() const
{ 
	if (DefaultValue < MinValue)
		return FD("DefaultValue < MinValue");
	if (DefaultBase <= MinValue)
		return FD("DefaultBase <= MinValue");
	if (DefaultValue > DefaultBase)
		return FD("DefaultValue > DefaultBas");
	if (RandomMin < MinValue)
		return FD("RandomMin < MinValue");
	if (RandomMax < RandomMin)
		return FD("RandomMax < RandomMin");
	if (RandomMax > DefaultBase)
		return FD("RandomMax > DefaultBase");
	if (RandomDeviation < 0)
		return FD("Deviation < 0");
	return true;
}

inline float AttributeInfo::Crop(const float value) const
{
	if (value < MinValue)
		return MinValue;
	if (value > DefaultBase)
		return DefaultBase;
	return value;
}

float AttributeInfo::GetRandom(const InitMode mode)
{
	if (mode == Uniform)
		return uniform(gen);
	if (mode != Normal)
		return DefaultValue;

	// normal distribution
	if (RandomDeviation <= 0)
		return Crop(RandomMean);
	return Crop(normal(gen));
}

float AttributeInfo::GetRandomUniform(float min, float max) const
{
	if (min > max)
		std::swap(min, max);
	if (min < MinValue)
		min = MinValue;
	if (max > DefaultBase)
		max = DefaultBase;
	const std::uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}

float AttributeInfo::GetRandomNormal(const float mean, const float deviation)
{
	if (deviation <= 0)
		return mean;
	std::normal_distribution<float> dist(mean, deviation);
	return dist(gen);
}

bool AttributeInfo::Load(const F4SESerializationInterface* serializer, AttributeInfo& retVal)
{
	if (!serializer->ReadRecordData(&retVal.Id, sizeof(retVal.Id)))
		return FE("Failed to read Id");
	if (!serializer->ReadRecordData(&retVal.DefaultValue, sizeof(retVal.DefaultValue)))
		return FE("Failed to read DefaultValue");
	if (!serializer->ReadRecordData(&retVal.MinValue, sizeof(retVal.MinValue)))
		return FE("Failed to read MinValue");
	if (!serializer->ReadRecordData(&retVal.DefaultBase, sizeof(retVal.DefaultBase)))
		return FE("Failed to read DefaultBase");
	if (!serializer->ReadRecordData(&retVal.RandomMin, sizeof(retVal.RandomMin)))
		return FE("Failed to read RandomMin");
	if (!serializer->ReadRecordData(&retVal.RandomMax, sizeof(retVal.RandomMax)))
		return FE("Failed to read RandomMax");
	if (!serializer->ReadRecordData(&retVal.RandomMean, sizeof(retVal.RandomMean)))
		return FE("Failed to read RandomMean");
	if (!serializer->ReadRecordData(&retVal.RandomDeviation, sizeof(retVal.RandomDeviation)))
		return FE("Failed to read RandomDeviation");
	return true;
}

bool AttributeInfo::Save(const F4SESerializationInterface* serializer) const
{
	const UInt32 id = AV->formID;

	if (!serializer->WriteRecordData(&id, sizeof(id)))
		return FE("Failed to write Id");
	if (!serializer->WriteRecordData(&DefaultValue, sizeof(DefaultValue)))
		return FE("Failed to write DefaultValue");
	if (!serializer->WriteRecordData(&MinValue, sizeof(MinValue)))
		return FE("Failed to write DefaultBase");
	if (!serializer->WriteRecordData(&DefaultBase, sizeof(DefaultBase)))
		return FE("Failed to write DefaultBase");
	if (!serializer->WriteRecordData(&RandomMin, sizeof(RandomMin)))
		return FE("Failed to write RandomMin");;
	if (!serializer->WriteRecordData(&RandomMax, sizeof(RandomMax)))
		return FE("Failed to write RandomMax");
	if (!serializer->WriteRecordData(&RandomMean, sizeof(RandomMean)))
		return FE("Failed to write RandomMean");
	if (!serializer->WriteRecordData(&RandomDeviation, sizeof(RandomDeviation)))
		return FE("Failed to write RandomDeviation");
	return true;
}

void AttributeInfo::Print(const UInt32 id) const
{
	const char* state = AV ? "valid" : "missing";
	Dmp("  %08X (%s): Def=%f, Min=%f, Base=%f, RMin=%f, RMax=%f, RMean=%f, RDev=%f", id, state, DefaultValue, MinValue, DefaultBase, RandomMin, RandomMax, RandomMean, RandomDeviation);
}