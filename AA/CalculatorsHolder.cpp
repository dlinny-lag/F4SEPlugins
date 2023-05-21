#include "CalculatorsHolder.h"
#include "SerializationHelper.h"
#include <algorithm>
#include <unordered_set>
#include "Limits.h"
#include "MyTrace.h"
#include "Shared\ScriptHandle.hpp"

const int dataStructure1 = 1;
UInt32 CalculatorsHolder::currentDataStructureVersion = dataStructure1;

void CalculatorsHolder::DumpAll()
{
	Dmp("Registered calculators:");
	for (auto& pair : calculators)
	{
		Dmp("  [%s] (%016X)", pair.first.Name.c_str(), pair.first.Handle);
		for (auto a : pair.second)
		{
			Dmp("    %08X", a ? a->formID : 0);
		}
	}
}


const std::vector<ActorValueInfo*> CalculatorsHolder::Get(const ScriptHandle& script) const
{
	const auto result = calculators.find(script);
	if (result == calculators.end())
		return std::vector<ActorValueInfo*>();
	return result->second;
}

bool CalculatorsHolder::Register(const ScriptHandle& script, const std::vector<ActorValueInfo*>& toRegister)
{
	if (calculators.find(script) != calculators.end())
		return FD("Calculator [%s] already registered", script.Name.c_str());

	if (calculators.size() >= MAX_SCRIPT_ARRAY_LENGTH)
		return FD("Maximum array size limit 128 reached for calculators");

	if (!calculators.insert(std::make_pair(script, toRegister)).second)
		return FE("Failed to insert calculator [%s]", script.Name.c_str());
	IObjectHandlePolicy* policy = (*g_gameVM)->m_virtualMachine->GetHandlePolicy();
	policy->AddRef(script.Handle);
	FireChanged();
	return true;
}

bool CalculatorsHolder::Unregister(const ScriptHandle& script)
{
	auto iter = calculators.find(script);
	if (iter == calculators.end())
		return FD("Calculator [%s] not found", script.Name.c_str());

	calculators.erase(iter);
	VirtualMachine* vm = (*g_gameVM)->m_virtualMachine;
	vm->GetHandlePolicy()->Release(script.Handle);
	FireChanged();
	return true;
}

bool CalculatorsHolder::IsRegistered(const ScriptHandle& script)
{
	return calculators.find(script) != calculators.end();
}

bool CalculatorsHolder::Save(const F4SESerializationInterface* serializer) const
{
	UInt32 size = calculators.size();
	if (!serializer->WriteRecordData(&size, sizeof(size)))
		return FE("Failed to write size of calculators");
	for (auto& pair : calculators)
	{
		if (!SerializationHelper::WriteString(serializer, &pair.first.Name))
			return FE("Failed to write name [%s] of calculator", pair.first.Name.c_str());;
		if (!serializer->WriteRecordData(&pair.first.Handle, sizeof(pair.first.Handle)))
			return FE("Failed to write calculator's handle");
		UInt32 valuesCount = pair.second.size();
		if (!serializer->WriteRecordData(&valuesCount, sizeof(valuesCount)))
			return FE("Failed to write size of calculator's AVs");
		for (const auto v : pair.second)
		{
			UInt32 avId = v ? v->formID : 0;
			D("Writing calculator's AV %08X", avId);
			if (!serializer->WriteRecordData(&avId, sizeof(avId)))
				return FE("Failed to write calculator's AV");;
		}
	}
	return true;
}

bool CalculatorsHolder::Load(const F4SESerializationInterface* serializer, UInt32 version)
{
	if (version != currentDataStructureVersion)
		return FE("Unexpected CalculatorsHolder version %d. Expected %d", version, currentDataStructureVersion);

	UInt32 count;
	if (!serializer->ReadRecordData(&count, sizeof(count)))
		return FE("Failed to read size of calculators");
	if (count < 0)
		return FE("Negative amount of calculators");
	std::unordered_map<ScriptHandle, std::vector<ActorValueInfo*>> readCalculators;

	for (int i = 0; i < count; i++)
	{
		ScriptHandle script;
		std::vector<ActorValueInfo*> values;
		if (!SerializationHelper::ReadString(serializer, &script.Name))
			return FE("Failed to read calculator's script name");
		if (!Serialization::ReadData(serializer, &script.Handle))
			return FE("Failed to read calculator's script handle");
		SInt32 valuesCount;
		if (!Serialization::ReadData(serializer, &valuesCount))
			return FE("Failed to read calculator's AV count");
		if (valuesCount < 0)
			return FE("Negative calculator's script AV count");
		values.reserve(valuesCount);
		for (int j = 0; j < valuesCount; j++)
		{
			UInt32 savedId;
			if (!Serialization::ReadData(serializer, &savedId))
				return FE("Failed to read calculator's AV");
			UInt32 actualId;
			ActorValueInfo* avi;
			if (serializer->ResolveFormId(savedId, &actualId))
			{
				avi = static_cast<ActorValueInfo*>(LookupFormByID(actualId));
				if (!avi)
				{
					W("AV %08X not found", actualId);
				}
			}
			else
			{
				W("ActorValue %08X not found for [%s] calculator", savedId, script.Name.c_str());
			}

			values.push_back(avi); // get it even if it is null. 0 will be passed to calculator for missing actor value
		}
		if (SerializationHelper::ResolveHandle(serializer, script.Handle, &script.Handle))
		{
			readCalculators.insert(std::make_pair(script, values));
		}
		else
		{
			W("Failed to resolve calculator [%s] (%016X)", script.Name.c_str(), script.Handle);
		}
	}
	calculators = readCalculators;
	FireChanged();
	return true;
}

constexpr UInt64 INT32_MASK = 0x00000000FFFFFFFF;
std::vector<ScriptHandle>& CalculatorsHolder::GetSorted()
{
	if (sorted.size() == calculators.size())
		return sorted;

	std::vector<ScriptHandle> ids;
	ids.reserve(calculators.size());
	for (auto& pair : calculators)
	{
		ids.push_back(pair.first);
	}
	sort(ids.begin(), ids.end(), [](const ScriptHandle& a, const ScriptHandle& b)
		{
			// order in same way how mods with scripts are ordered
			return (a.Handle & INT32_MASK) < (b.Handle & INT32_MASK);
		});

	sorted = ids;
	return sorted;
}

void CalculatorsHolder::Clear()
{
	calculators.clear();
	FireChanged();
}

void CalculatorsHolder::FireChanged()
{
	sorted.clear();
	Changeable::FireChanged();
}