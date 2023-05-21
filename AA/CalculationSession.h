#pragma once
#include "f4se/GameReferences.h"
#include "f4se/PapyrusVM.h"
#include "f4se/PluginAPI.h"
#include "f4se/PapyrusArgs.h"
#include <vector>
#include <unordered_map>
#include "AttributeModifier.hpp"
#include "MyTrace.h"

struct AVValues
{
	ActorValueInfo* AV;
	float InitialValue;
	float Value;
};

class CalculationSession
{
	UInt32 SessionId;
	Actor* CalculatingActor;
	std::vector<AVValues> AllAttributes;

	static bool SaveInternal(const F4SESerializationInterface* serializer);
	static bool LoadInternal(const F4SESerializationInterface* serializer, UInt32 version);
	static UInt32 CalculationSession::currentDataStructureVersion;

	static std::unordered_map<UInt32, CalculationSession> ActiveSessions;
	static BSReadWriteLock sessionsLock;
	static UInt32 currentSessionId; // non zero
	static void ValidateSessionId();
public:
	

	CalculationSession():
		SessionId(-1)
		,CalculatingActor(nullptr)
	{

	}
	static bool Save(const F4SESerializationInterface* serializer);
	static bool Load(const F4SESerializationInterface* serializer);
	static void Clear();
	static UInt32 Open(Actor* actor, const std::vector<ActorValueInfo*>& attributes, AttributeInitMode mode);

	static CalculationSession Close(UInt32 sessionId);

	void ForEach(const std::function<void(std::vector<TESForm*>&, Actor*, ActorValueInfo*, float, float)>& functor, std::vector<TESForm*>& senders) const;
	bool IsValid() const;
	static std::vector<ActorValueInfo*> GetAllAttributes(UInt32 sessionId);
	static std::vector<float> GetValues(UInt32 sessionId, const std::vector<UInt32>& indices);
	static void SetValues(UInt32 sessionId, const std::vector<UInt32>& indices, const std::vector<float>& newValues);

	static void DumpAll();
	void Dump() const;
};

