#pragma once
#include "f4se/GameForms.h"
#include "f4se/PapyrusActorBase.h"
#include "AttributeInfo.h"
#include <limits>
#include "MyTrace.h"

struct ModifyResult
{
	float PrevValue;
	float NewValue;
	float Exceed;
	bool Increase;
	void Print(const char* eventType) const
	{
		D("%s: new=%f, prev=%f, ex=%f", eventType, NewValue, PrevValue, Exceed);
	}
};

struct InitConfiguration
{
	AttributeInitMode Mode;
	float ExactValue;
};

class AttributeModifier
{

	static void RestoreValue(Actor* a, ActorValueInfo* av, float delta)
	{
		a->actorValueOwner.Unk_08(av, delta); // Unk_08 = RestoreActorValue
	}

	// no arguments checks!
	static void DamageValue(Actor* a, ActorValueInfo* av, float delta)
	{
		a->actorValueOwner.Mod(2, av, -delta);
	}
public:

	static bool Increase(Actor* actor, ActorValueInfo* attribute, AttributeInfo& ai, float delta, ModifyResult& retVal, AttributeInitMode mode, bool capped)
	{
		if (!actor)
			return FD("Increase - actor is null");
		if (!attribute)
			return FD("Increase - attribute is null");
		if (delta <= 0)
			return FD("Increase - delta %f <= 0", delta);

		if (ai.AV)
			return BoundedIncrease(actor, attribute, ai, delta, retVal, mode, capped);

		return UnboundedIncrease(actor, attribute, delta, retVal);
	}


	static bool Decrease(Actor* actor, ActorValueInfo* attribute, AttributeInfo& ai, float delta, ModifyResult& retVal, AttributeInitMode mode, bool capped)
	{
		if (!actor)
			return FD("Decrease - actor is null");
		if (!attribute)
			return FD("Decrease - attribute is null");
		if (delta <= 0)
			return FD("Decrease - delta %f <= 0", delta);

		if (ai.AV)
			return BoundedDecrease(actor, attribute, ai, delta, retVal, mode, capped);

		return UnboundedDecrease(actor, attribute, delta, retVal);
	}

	static bool Set(Actor* actor, ActorValueInfo* attribute, AttributeInfo& ai, float value, ModifyResult& retVal, AttributeInitMode mode)
	{
		if (!actor)
			return FD("Set - actor is null");
		if (!attribute)
			return FD("Set - attribute is null");
		if (ai.AV)
			return BoundedSet(actor, attribute, ai, value, retVal, mode);

		return UnboundedSet(actor, attribute, value, retVal);
	}

	static float Get(Actor* actor, ActorValueInfo* attribute, AttributeInfo& ai, AttributeInitMode mode)
	{
		if (!actor)
		{
			D("Get -  actor is null");
			return 0;
		}
		if (!attribute)
		{
			D("Get - attribute is null");
			return 0;
		}
		if (!ai.AV)
		{
			D("Get - Unmanaged attribute %08X", attribute->formID);
			return actor->actorValueOwner.GetValue(attribute);
		}

		return BoundedGet(actor, attribute, ai, mode);
	}

	static float GetMax(Actor* actor, ActorValueInfo* attribute, const AttributeInfo& ai)
	{
		if (!actor)
		{
			D("GetMax -  actor is null");
			return 0;
		}
		if (!attribute)
		{
			D("GetMax - attribute is null");
			return 0;
		}
		if (!ai.AV)
		{
			D("GetMax - Unmanaged attribute %08X", attribute->formID);
			return actor->actorValueOwner.GetMaximum(attribute);
		}
		return BoundedGetMax(actor, attribute, ai);
	}

	static std::vector<float> GetValues(Actor* actor, VMArray<ActorValueInfo*>& avis, std::vector<AttributeInfo>& infos, AttributeInitMode mode)
	{// TODO: optimize
		std::vector<float> retVal;
		retVal.reserve(avis.Length());
		for (UInt32 i = 0; i < avis.Length(); i++)
		{
			ActorValueInfo* avi;
			avis.Get(&avi, i);
			float val = Get(actor, avi, infos[i], mode);
			retVal.emplace_back(val);
		}
		return retVal;
	}

	static std::vector<float> GetMaximums(Actor* actor, VMArray<ActorValueInfo*>& avis, const std::vector<AttributeInfo>& infos)
	{// TODO: optimize
		std::vector<float> retVal;
		retVal.reserve(avis.Length());
		for (UInt32 i = 0; i < avis.Length(); i++)
		{
			ActorValueInfo* avi;
			avis.Get(&avi, i);
			float val = GetMax(actor, avi, infos[i]);
			retVal.emplace_back(val);
		}
		return retVal;
	}

	static void EnsureInitialized(Actor* actor, std::vector<AttributeInfo>& infos, const std::vector<InitConfiguration>& configs)
	{ // TODO: optimize
		BSWriteLocker lock(&actor->avLock);
		UInt64 baseIndex, modIndex;
		for (size_t i = 0; i < infos.size(); i++)
		{
			if (infos[i].AV)
			{
				FindOrInsert(actor, infos[i].AV->formID, infos[i],baseIndex, modIndex, configs[i]);
			}
		}
	}

private:

	static bool UnboundedSet(Actor* actor, ActorValueInfo* attribute, float value, ModifyResult& retVal)
	{
		retVal.PrevValue = actor->actorValueOwner.GetValue(attribute);
		if (retVal.PrevValue < value)
		{
			retVal.Increase = true;
			RestoreValue(actor, attribute, value - retVal.PrevValue);
		}
		else if (retVal.PrevValue > value)
		{
			retVal.Increase = false;
			DamageValue(actor, attribute, retVal.PrevValue - value);
		}
		else
			return false;

		retVal.Exceed = 0;
		retVal.NewValue = actor->actorValueOwner.GetValue(attribute);
		return true;
	}

	static bool UnboundedIncrease(Actor* actor, ActorValueInfo* attribute, float delta, ModifyResult& retVal)
	{
		retVal.PrevValue = actor->actorValueOwner.GetValue(attribute);
		retVal.Exceed = 0;
		RestoreValue(actor, attribute, delta);
		retVal.NewValue = actor->actorValueOwner.GetValue(attribute);
		return true;
	}

	static bool UnboundedDecrease(Actor* actor, ActorValueInfo* attribute, float delta, ModifyResult& retVal)
	{
		retVal.PrevValue = actor->actorValueOwner.GetValue(attribute);
		retVal.Exceed = 0;
		DamageValue(actor, attribute, delta);
		retVal.NewValue = actor->actorValueOwner.GetValue(attribute);
		return true;
	}


	static void FindOrInsert(Actor* actor, UInt32 attributeId, AttributeInfo& ai, UInt64& baseIndex, UInt64& modIndex, InitConfiguration config)
	{
		// search for base value index
		baseIndex = std::numeric_limits<UInt64>::max();
		bool baseFound = false;
		float foundBaseValue = 0;
		for (UInt64 i = 0; i < actor->actorValueData.count; i++)
		{
			if (attributeId == actor->actorValueData[i].avFormID)
			{
				baseIndex = i;
				baseFound = true;
				foundBaseValue = actor->actorValueData[i].value;
				//D("Actor %08X, attribute %08X: found base value = %f when Max should be %f", actor->formID, attributeId, foundBaseValue, ai.DefaultBase);
				actor->actorValueData[i].value = ai.DefaultBase; // fix potentially wrong value
				break;
			}
		}

		// search for modifiers
		modIndex = std::numeric_limits<UInt64>::max();
		for (UInt64 i = 0; i < actor->unk350.count; i++)
		{
			if (attributeId == actor->unk350[i].avFormID)
			{
				modIndex = i;
				break;
			}
		}
		
		if (baseIndex == std::numeric_limits<UInt64>::max())
		{ // not found. insert new
			Actor::ActorValueData baseValue;
			baseValue.avFormID = attributeId;
			baseValue.value = ai.DefaultBase;
			baseIndex = actor->actorValueData.count;
			actor->actorValueData.Push(baseValue);
		}



		if (modIndex == std::numeric_limits<UInt64>::max())
		{ // not found. insert new - it will be definitely used
			const float max = actor->actorValueData[baseIndex].value;
			Actor::Data350 modifiers;
			modifiers.avFormID = attributeId;
			modifiers.unk04 = modifiers.unk08 = 0;
			switch (config.Mode)
			{
			case AttributeInitMode::Exact:
			case AttributeInitMode::Default:
				// very thin thing - value could be defined in ESP/ESL/ESM or by script, but 
				// it was assigned to BaseValue that probably 
				// not equal to registered DefaultBase
				// so if BaseValue was found above then apply modifier
				// in a way to keep assigned value as current value
				if (baseFound)
				{  // ignore defined value for Exact mode and use passed value
					modifiers.unk0C = (config.Mode == AttributeInitMode::Default ? foundBaseValue : ai.Crop(config.ExactValue)) - ai.DefaultBase;
					if (modifiers.unk0C > 0)
						modifiers.unk0C = 0; // handle invalid initial value 
				}
				else
				{ // value was not defined - use DefaultValue/ExactValue
					modifiers.unk0C = (config.Mode == AttributeInitMode::Default ? ai.DefaultValue : ai.Crop(config.ExactValue)) - max;
				}
				break;
			case AttributeInitMode::Uniform:
			case AttributeInitMode::Normal:
				modifiers.unk0C = ai.GetRandom(config.Mode) - max;
				break;
			default:
				modifiers.unk0C = ai.DefaultValue - max; // fallback for wrong mode
				break;
			}
			modIndex = actor->unk350.count;
			actor->unk350.Push(modifiers);
		}
	}

	static float GetValue(Actor* actor, UInt64 baseIndex, UInt64 modIndex)
	{
		return actor->actorValueData[baseIndex].value +
			actor->unk350[modIndex].unk04 + // effect with archetype = PeakValueModifier
			actor->unk350[modIndex].unk08 + // effect with archetype = ValueModifier
			actor->unk350[modIndex].unk0C;  // Damage/Restore modifier
	}

	static float GetMax(Actor* actor, UInt64 baseIndex, UInt64 modIndex)
	{
		return	actor->actorValueData[baseIndex].value +
				actor->unk350[modIndex].unk04; // effect with archetype = PeakValueModifier
	}

	static void RestoreValue(Actor* actor, UInt64 modIndex, float delta)
	{
		actor->unk350[modIndex].unk0C += delta;
		if (actor->unk350[modIndex].unk0C > 0)
			actor->unk350[modIndex].unk0C = 0; // must not exceed 0
	}
	static void DamageValue(Actor* actor, UInt64 modIndex, float delta)
	{
		actor->unk350[modIndex].unk0C -= delta;
	}

	static bool BoundedSet(Actor* actor, const ActorValueInfo* attribute, AttributeInfo& ai, float value, ModifyResult& retVal, AttributeInitMode mode)
	{
		BSWriteLocker lock(&actor->avLock);
		UInt64 baseIndex, modIndex;
		FindOrInsert(actor, attribute->formID, ai, baseIndex, modIndex, {mode, 0});
		const float prevVal = GetValue(actor, baseIndex, modIndex);
		if (prevVal < value)
		{
			retVal.Increase = true;
			IndexedIncrease(actor, ai, value - prevVal, retVal, baseIndex, modIndex, false);
		}
		else if (prevVal > value)
		{
			retVal.Increase = false;
			IndexedDecrease(actor, ai, prevVal - value, retVal, baseIndex, modIndex, false);
		}
		else
			return false;

		retVal.PrevValue = prevVal;
		return true;
	}

	static float BoundedGet(Actor* actor, const ActorValueInfo* attribute, AttributeInfo& ai, AttributeInitMode mode)
	{
		BSWriteLocker lock(&actor->avLock);
		UInt64 baseIndex, modIndex;
		FindOrInsert(actor, attribute->formID, ai, baseIndex, modIndex, {mode, 0});
		return GetValue(actor, baseIndex, modIndex);
	}

	static float BoundedGetMax(Actor* actor, const ActorValueInfo* attribute, const AttributeInfo& ai)
	{
		BSWriteLocker lock(&actor->avLock);
		// do not try to modify value or modifiers
		// just search for them
		float retVal;
		const UInt32 attributeId = attribute->formID;
		bool baseFound = false;
		for (UInt64 i = 0; i < actor->actorValueData.count; i++)
		{
			if (attributeId == actor->actorValueData[i].avFormID)
			{
				baseFound = true;
				retVal = actor->actorValueData[i].value;
				if (retVal > ai.DefaultBase)
					retVal = ai.DefaultBase;
				break;
			}
		}
		if (!baseFound)
			return ai.DefaultBase;

		// search for modifiers block
		for (UInt64 i = 0; i < actor->unk350.count; i++)
		{
			if (attributeId == actor->unk350[i].avFormID)
			{
				retVal += actor->unk350[i].unk04; // see GetMax()
				break;
			}
		}
		return retVal;
	}

	static void IndexedIncrease(Actor* actor, const AttributeInfo& ai, float delta, ModifyResult& retVal, UInt64 baseIndex, UInt64 modIndex, bool capped)
	{
		retVal.PrevValue = GetValue(actor, baseIndex, modIndex);
		if (retVal.PrevValue < ai.MinValue)
		{ // recover invalid value
			RestoreValue(actor, modIndex, ai.MinValue - retVal.PrevValue);
			retVal.PrevValue = ai.MinValue;
		}
		const float max = GetMax(actor, baseIndex, modIndex);
		retVal.NewValue = retVal.PrevValue + delta;
		retVal.Exceed = 0;
		if (retVal.NewValue > max)
		{
			if (!capped)
				retVal.Exceed = retVal.NewValue - max;
			retVal.NewValue = max;
		}
		RestoreValue(actor, modIndex, delta);
	}

	static bool BoundedIncrease(Actor* actor, const ActorValueInfo* attribute, AttributeInfo& ai, float delta, ModifyResult& retVal, AttributeInitMode mode, bool capped)
	{
		BSWriteLocker lock(&actor->avLock);
		UInt64 baseIndex, modIndex;
		FindOrInsert(actor, attribute->formID, ai, baseIndex, modIndex, {mode, 0});

		IndexedIncrease(actor, ai, delta, retVal, baseIndex, modIndex, capped);
		// it is possible that all modifiers of attribute are equal to 0 now
		// Bethesda's code purges such modifier block
		// but hey, purging will force redundant re-initialization next time
		// just keep finger crossing that it will not break anything in Creation Engine workflow %)
		return true;
	}

	static void IndexedDecrease(Actor* actor, const AttributeInfo& ai, float delta, ModifyResult& retVal, UInt64 baseIndex, UInt64 modIndex, bool capped)
	{
		retVal.PrevValue = GetValue(actor, baseIndex, modIndex);
		if (retVal.PrevValue < ai.MinValue)
		{ // recover invalid value
			RestoreValue(actor, modIndex, ai.MinValue - retVal.PrevValue);
			retVal.PrevValue = ai.MinValue;
		}
		retVal.NewValue = retVal.PrevValue - delta;
		retVal.Exceed = 0;
		if (retVal.NewValue < ai.MinValue)
		{
			if (!capped)
				retVal.Exceed = ai.MinValue - retVal.NewValue;
			retVal.NewValue = ai.MinValue;
		}
		DamageValue(actor, modIndex, delta - retVal.Exceed);
	}

	static bool BoundedDecrease(Actor* actor, const ActorValueInfo* attribute, AttributeInfo& ai, float delta, ModifyResult& retVal, AttributeInitMode mode, bool capped)
	{
		BSWriteLocker lock(&actor->avLock);
		UInt64 baseIndex, modIndex;
		FindOrInsert(actor, attribute->formID, ai, baseIndex, modIndex, {mode, 0});

		IndexedDecrease(actor, ai, delta, retVal, baseIndex, modIndex, capped);
		return true;
	}

};