#include "CalculationSession.h"
#include "SerializationHelper.h"
#include "f4se/GameForms.h"
#include "AttributesManager.h"

#include "MyTrace.h"

std::unordered_map<UInt32, CalculationSession> CalculationSession::ActiveSessions;
BSReadWriteLock CalculationSession::sessionsLock;
UInt32 CalculationSession::currentSessionId = 1; // non zero

constexpr UInt32 maxSessionId = 0x8FFFFFFE; // probably will never be reached
constexpr int dataStructure1 = 1;
UInt32 CalculationSession::currentDataStructureVersion = dataStructure1;


void CalculationSession::ValidateSessionId()
{
	if (currentSessionId >= maxSessionId)
		currentSessionId = 1;
}

constexpr UInt32 rootTag = 'CASE';
bool CalculationSession::Save(const F4SESerializationInterface* serializer)
{ 
	if (serializer->OpenRecord(rootTag, currentDataStructureVersion))
	{
		BSReadLocker lock(&sessionsLock);
		return SaveInternal(serializer);
	}
	else
	{
		return FE("Failed to open record (%.4s)", &rootTag);
	}

}
bool CalculationSession::SaveInternal(const F4SESerializationInterface* serializer)
{
	UInt32 size = ActiveSessions.size();
	if (!serializer->WriteRecordData(&size, sizeof(size)))
		return FE("Failed to write size of active sessions");
	for (auto& pair : ActiveSessions)
	{
		CalculationSession& s = pair.second;
		if (!serializer->WriteRecordData(&s.SessionId, sizeof(s.SessionId)))
			return FE("Failed to write session ID %d", s.SessionId);
		UInt32 actorId = s.CalculatingActor ? s.CalculatingActor->formID : 0;
		D("Writing sessions's actor %08X", actorId);
		if (!serializer->WriteRecordData(&actorId, sizeof(actorId)))
			return FE("Failed to write session's actor");
		size = s.AllAttributes.size();
		if (!serializer->WriteRecordData(&size, sizeof(size)))
			return FE("Failed to write sessions's attributes size");
		for (auto& avPair : s.AllAttributes)
		{
			UInt32 avId = avPair.AV ? avPair.AV->formID : 0;
			D("Writing session's attribute %08X", avId);
			if (!serializer->WriteRecordData(&avId, sizeof(avId)))
				return FE("Failed to write AV id");
			if (!serializer->WriteRecordData(&avPair.InitialValue, sizeof(avPair.InitialValue)))
				return FE("Failed to write AV initial value");;
			if (!serializer->WriteRecordData(&avPair.Value, sizeof(avPair.Value)))
				return FE("Failed to write AV value");;
		}
	}

	return true;
}


bool CalculationSession::Load(const F4SESerializationInterface* serializer)
{
	UInt32 recType, length, curVersion;
	if (serializer->GetNextRecordInfo(&recType, &curVersion, &length))
	{
		if (recType != rootTag)
		{
			return FE("Invalid record type (%.4s). Expected (%.4s)", &recType, &rootTag);
		}
		BSWriteLocker lock(&sessionsLock);
		return LoadInternal(serializer, curVersion);
	}
	return FE("CalculationSession. Failed to get record.");
}

bool CalculationSession::LoadInternal(const F4SESerializationInterface* serializer, UInt32 version)
{
	if (version != currentDataStructureVersion)
		return FE("Unexpected CalculationSession version %d. Expected %d", version, currentDataStructureVersion);

	UInt32 size;
	if (!serializer->ReadRecordData(&size, sizeof(size)))
		return FE("Failed to read size of sessions");
	for (int i = 0; i < size; i++)
	{
		CalculationSession s;
		if (!serializer->ReadRecordData(&s.SessionId, sizeof(s.SessionId)))
			return FE("Failed to read session's id");
		UInt32 actorId;
		if (!serializer->ReadRecordData(&actorId, sizeof(actorId)))
			return FE("Failed to read session's Actor");

		UInt32 actualActorId;
		if (serializer->ResolveFormId(actorId, &actualActorId))
		{
			s.CalculatingActor = static_cast<Actor*>(LookupFormByID(actualActorId));
			if (!s.CalculatingActor)
			{
				W("Actor %08X not found", actualActorId);
			}
		}
		else
		{
			D("Failed to resolve Actor ID %08X", actorId);
			// leave actor = null and read rest data
			// this record will be ignored later
		}
		UInt32 avSize;
		if (!serializer->ReadRecordData(&avSize, sizeof(avSize)))
			return FE("Failed to read session's size of AV");
		for (int j = 0; j < avSize; j++)
		{
			UInt32 avId;
			if (!serializer->ReadRecordData(&avId, sizeof(avId)))
				return FE("Failed to read session's AV id");
			AVValues avVals;
			UInt32 actualAVId;
			if (serializer->ResolveFormId(avId, &actualAVId))
			{
				avVals.AV = static_cast<ActorValueInfo*>(LookupFormByID(actualAVId));
				if (!avVals.AV)
				{
					D("AV %08X not found", actualAVId);
				}
			}
			else
			{
				D("Failed to resolve AV ID %08X", avId);
				// leave AV = null and read rest data
				// missing AV is not a reason for session termination
			}
			if (!serializer->ReadRecordData(&avVals.InitialValue, sizeof(avVals.InitialValue)))
				return FE("Failed to read session's AV initial value");
			if (!serializer->ReadRecordData(&avVals.Value, sizeof(avVals.Value)))
				return FE("Failed to read session's AV value");
			s.AllAttributes.push_back(avVals);
		}

		// it is assumed that Manager script instance
		// can not live without Actor
		// so if we haven't actor, then session initiated by disappeared Manager script 
		// would not be processed by this script and we can drop session
		if (s.CalculatingActor)
			ActiveSessions.insert(std::make_pair(s.SessionId, s));
		else
		{
			D("Actor for session %d disappeared", s.SessionId);
		}
	}
	for (auto& pair : ActiveSessions)
	{
		if (currentSessionId <= pair.second.SessionId)
			currentSessionId = pair.second.SessionId + 1;
	}
	ValidateSessionId();
	return true;
}


void CalculationSession::Clear()
{
	BSWriteLocker lock(&sessionsLock);
	ActiveSessions.clear();
}

UInt32 CalculationSession::Open(Actor* actor, const std::vector<ActorValueInfo*>& attributes, AttributeInitMode mode)
{
	if (!actor)
	{
		D("Attempt to open session for empty actor");
		return -1;
	}
	CalculationSession s;
	s.CalculatingActor = actor;
	D("Collecting attributes data...");
	for (auto a : attributes)
	{
		AttributeInfo ai = g_AttributesManager.GetRegistrationInfo(a);
		AVValues avVals;
		avVals.AV = a;
		avVals.InitialValue = avVals.Value = AttributeModifier::Get(actor, a, ai, mode);
		s.AllAttributes.push_back(avVals);
	}
	BSWriteLocker lock(&sessionsLock);
	UInt32 sessionId = ++currentSessionId;
	ValidateSessionId();
	D("Opening session %d for Actor %08X", sessionId, actor->formID);
	s.SessionId = sessionId;
	ActiveSessions.insert(std::make_pair(sessionId, s));
	return sessionId;
}

CalculationSession CalculationSession::Close(UInt32 sessionId)
{
	if (sessionId == 0 || sessionId > maxSessionId)
	{
		D("Attempt to close invalid session %d", sessionId);
		return CalculationSession();
	}
	BSWriteLocker lock(&sessionsLock);
	auto iter = ActiveSessions.find(sessionId);
	if (iter == ActiveSessions.end())
	{
		D("Closing session %d not found", sessionId);
		return CalculationSession();
	}
	//D("Closing session %d", sessionId);
	//iter->second.Dump();
	CalculationSession retVal = iter->second;
	ActiveSessions.erase(iter);
	D("Closed session %d", sessionId);
	//retVal.Dump();
	return retVal;
}

bool CalculationSession::IsValid() const
{
	return SessionId > 0 && CalculatingActor && !AllAttributes.empty();
}

void CalculationSession::ForEach(const std::function<void(std::vector<TESForm*>&, Actor*, ActorValueInfo*, float, float)>& functor, std::vector<TESForm*>& senders) const
{
	if (!CalculatingActor)
		return;
	
	for (auto& e : AllAttributes)
	{
		if (e.AV)
		{
			functor(senders, CalculatingActor, e.AV, e.InitialValue, e.Value);
		}

	}
}


std::vector<ActorValueInfo*> CalculationSession::GetAllAttributes(UInt32 sessionId)
{
	std::vector<ActorValueInfo*> retVal;
	CalculationSession session;

	BSReadLocker lock(&sessionsLock);
	auto iter = ActiveSessions.find(sessionId);
	if (iter != ActiveSessions.end())
		session = iter->second;
	else
	{
		D("Attempt to get attributes of absent session %d", sessionId);
		return retVal;
	}
	retVal.reserve(session.AllAttributes.size());
	for (auto& a : session.AllAttributes)
	{
		retVal.push_back(a.AV);
	}
	return retVal;
}

std::vector<float> CalculationSession::GetValues(UInt32 sessionId, const std::vector<UInt32>& indices)
{
	CalculationSession session;
	
	BSReadLocker lock(&sessionsLock);
	auto iter = ActiveSessions.find(sessionId);
	if (iter != ActiveSessions.end())
		session = iter->second;
	
	
	if (!session.IsValid())
	{
		D("Attempt to get values of invalid session %d", sessionId);
		return std::vector<float>();
	}
	std::vector<float> retVal;
	retVal.reserve(indices.size());
	for (UInt32 idx : indices)
	{
		float value = 0;
		if (session.IsValid())
		{
			if (idx >= 0 && idx < session.AllAttributes.size())
			{
				if (session.AllAttributes[idx].AV)
					value = session.AllAttributes[idx].Value;
			}
		}
		retVal.push_back(value);
	}
	return retVal;
}

void CalculationSession::SetValues(UInt32 sessionId, const std::vector<UInt32>& indices, const std::vector<float>& newValues)
{
	if (sessionId < 1)
	{
		D("Attempt to set values for invalid session %d", sessionId);
		return; // nothing to update
	}
	if (indices.size() != newValues.size())
	{
		D("Amout of values to set (%d) is not same to amount of attributes (%d) in session %d", newValues.size(), indices.size(), sessionId);
		return;
	}
	if (indices.size() == 0)
	{
		D("No indices, nothing to update");
		return;
	}
	CalculationSession* session;
	
	BSReadLocker lock(&sessionsLock);
	auto iter = ActiveSessions.find(sessionId);
	if (iter != ActiveSessions.end())
		session = &iter->second;
	else
	{
		D("Set - session %d not found", sessionId);
		return;
	}
	if (!session->IsValid())
	{
		W("Session %d is invalid", sessionId);
		return;
	}
	const UInt32 attrsSize = session->AllAttributes.size();
	for (int i = 0; i < indices.size(); i++)
	{
		const UInt32 aIdx = indices[i];
		if (aIdx < 0 || aIdx >= attrsSize)
		{
			D("Detected invalid index %d on %d position. Expected in range [0-%d]", aIdx, i, attrsSize-1);
			continue;
		}
		session->AllAttributes[indices[i]].Value = newValues[i];
		//D("%d: Val[%d]=%f", i, indices[i], session->AllAttributes[indices[i]].Value);
	}
	//session->Dump();
}

void CalculationSession::DumpAll()
{
	Dmp("Active sessions:");
	BSReadLocker lock(&sessionsLock);
	for (auto& s : ActiveSessions)
	{
		s.second.Dump();
	}
}
void CalculationSession::Dump() const
{
	Dmp("Session %d for Actor %08X", SessionId, CalculatingActor ? CalculatingActor->formID : 0);
	for (auto& a : AllAttributes)
	{
		Dmp("  AV %08X: %.6f -> %.6f", a.AV ? a.AV->formID : 0, a.InitialValue, a.Value);
	}
}