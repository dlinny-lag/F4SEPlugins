#pragma once
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/GameReferences.h"
#include "f4se/PapyrusEvents.h"
#include "FormsSet.hpp"
#include "AttributeInfoSet.h"
#include "AttributeModifier.hpp"
#include "AttributesManager.h"
#include "SubscriptionsHolder.h"

#define AA_AV_PAPYRUS_SCRIPT "AA:AV"

namespace AttributesAPIExport
{
	const char* EXPORT_PAPYRUS_SCRIPT = AA_AV_PAPYRUS_SCRIPT;

	bool Save(const F4SESerializationInterface* intfc)
	{
		return g_AttributesManager.Save(intfc);
	}
	bool Load(const F4SESerializationInterface* intfc)
	{
		return g_AttributesManager.Load(intfc);
	}
	void Clear()
	{
		g_AttributesManager.Clear();
	}
	void DumpAll()
	{
		g_AttributesManager.DumpAll();
	}


	bool RegisterAttribute(StaticFunctionTag* _, ActorValueInfo* av, float defaultValue, float minValue, float defaultMax, float randomMin, float randomMax, float mean, float deviation)
	{
		if (!av)
			return FD("Empty AV");
		if ((av->avFlags & ActorValueInfo::kFlag_Hardcoded) != 0)
		{
			return FD("Hardcoded AV %08X", av->formID); // AV is hardcoded, we can't support this 
		}
		if ((av->avFlags & ActorValueInfo::kFlag_DamageIsPositive) != 0)
		{
			return FD("DamageIsPositive AV %08X", av->formID); // don't want to support this idea. it looks redundant
		}
		const bool res = g_AttributesManager.Add(av, defaultValue, minValue, defaultMax, randomMin, randomMax, mean, deviation);
		
		D("RegisterAttribute: %08X with result=%s", av->formID, S(res));
		return res;
	}
	bool UpdateAttributeRegistration(StaticFunctionTag* _, ActorValueInfo* av, float defaultValue, float randomMin, float randomMax, float mean, float deviation)
	{
		if (!av)
			return FD("Empty AV");
		if ((av->avFlags & ActorValueInfo::kFlag_Hardcoded) != 0)
		{
			return FD("Hardcoded AV %08X", av->formID); // AV is hardcoded, we can't support this 
		}
		if ((av->avFlags & ActorValueInfo::kFlag_DamageIsPositive) != 0)
		{
			return FD("DamageIsPositive AV %08X", av->formID); // don't want to support this idea. it looks redundant
		}
		const bool res = g_AttributesManager.Update(av, defaultValue, randomMin, randomMax, mean, deviation);
		
		D("UpdateAttributeRegistration: %08X with result=%s", av->formID, S(res));
		return res;
	}

	bool UnregisterAttribute(StaticFunctionTag* _, ActorValueInfo* av)
	{
		if (!av)
			return FD("Empty AV");
		const bool res = g_AttributesManager.Remove(av);
		D("UnregisterAttribute: %08X with result=%s", av->formID, S(res));
		return res;		
	}
	bool IsAttributeRegistered(StaticFunctionTag* _, ActorValueInfo* av)
	{
		if (!av)
			return FD("Empty AV");
		const bool res = g_AttributesManager.Contains(av);
		D("IsAttributeRegistered: %08X with result=%s", av->formID, S(res));
		return res;		
	}

	float Get(StaticFunctionTag* _, Actor* actor, ActorValueInfo* attribute, UInt32 mode)
	{
		AttributeInfo ai = g_AttributesManager.GetRegistrationInfo(attribute);
		return AttributeModifier::Get(actor, attribute, ai, AttributeInfo::ModeFromInt(mode));
	}
	float GetMax(StaticFunctionTag* _, Actor* actor, ActorValueInfo* attribute)
	{
		const AttributeInfo ai = g_AttributesManager.GetRegistrationInfo(attribute);
		return AttributeModifier::GetMax(actor, attribute, ai);		
	}
	VMArray<float> GetValues(StaticFunctionTag* _, Actor* actor, VMArray<ActorValueInfo*> avis, UInt32 mode)
	{
		std::vector<AttributeInfo> attrs = g_AttributesManager.GetRegistrationInfo(avis);
		const std::vector<float> values = AttributeModifier::GetValues(actor, avis, attrs, AttributeInfo::ModeFromInt(mode));
		VMArray<float> retVal;
		for (float val : values)
		{
			retVal.Push(&val);
		}
		return retVal;		
	}
	VMArray<float> GetMaximums(StaticFunctionTag* _, Actor* actor, VMArray<ActorValueInfo*> avis)
	{
		const std::vector<AttributeInfo> attrs = g_AttributesManager.GetRegistrationInfo(avis);
		const std::vector<float> values = AttributeModifier::GetMaximums(actor, avis, attrs);
		VMArray<float> retVal;
		for (float val : values)
		{
			retVal.Push(&val);
		}
		return retVal;		
	}
	void EnsureInitialized(StaticFunctionTag* _, Actor* actor, VMArray<ActorValueInfo*> attributes, VMArray<SInt32> modes, VMArray<float> exactValues)
	{
		if (!actor)
			return;
		if (attributes.IsNone())
			return;
		if (modes.IsNone())
			return;

		const UInt32 attrsLength = attributes.Length();
		if (attrsLength == 0)
			return;
		const UInt32 modesLength = modes.Length();
		if (modesLength == 0)
			return;
		const UInt32 valsLength = exactValues.IsNone() ? 0 : exactValues.Length();

		const UInt32 minLen = std::min(attrsLength, modesLength);

		std::vector<InitConfiguration> configs;
		configs.reserve(minLen);

		std::vector<ActorValueInfo*> attrs;
		attrs.reserve(minLen);

		for (UInt32 i = 0; i < minLen; i++)
		{
			ActorValueInfo* av;
			SInt32 mode;

			attributes.Get(&av, i);
			if (!av)
				continue;
			modes.Get(&mode, i);

			InitConfiguration config;
			
			if (mode == -1)
			{
				if (i < valsLength)
				{
					float exactValue;
					exactValues.Get(&exactValue, i);
					config.Mode = AttributeInitMode::Exact;
					config.ExactValue = exactValue;
				}
				else
				{
					config.Mode = AttributeInitMode::Default;
				}
			}
			else
				config.Mode = AttributeInfo::ModeFromInt(mode);

			configs.emplace_back(config);
			attrs.emplace_back(av);
		}

		std::vector<AttributeInfo> infos;
		{
			
			infos = g_AttributesManager.GetValues(attrs);
		}
		AttributeModifier::EnsureInitialized(actor, infos, configs);		
	}


	
	inline std::vector<TESForm*> ToArray(TESForm* keyword)
	{
		return std::vector {keyword};
	}

	bool DecreaseInternal(std::vector<TESForm*>& senders, Actor* actor, ActorValueInfo* attribute, float delta, UInt32 mode, AttributeInfo ai, bool capped)
	{
		if ((attribute->avFlags & ActorValueInfo::kFlag_DamageIsPositive) != 0)
		{
			return FD("DamageIsPositive AV %08X", attribute->formID); // don't want to support this idea. it looks redundant
		}

		ModifyResult data;
		const bool res = AttributeModifier::Decrease(actor, attribute, ai, delta, data, AttributeInfo::ModeFromInt(mode), capped);
		if (!res)
			return FD("DecreaseInternal can't decrease");
		g_Subscriptions.NotifyDecreased(senders, actor, attribute, data.NewValue, data.PrevValue, data.Exceed);
		return true;
	}

	bool DecreaseCommon(TESForm* sender, Actor* actor, ActorValueInfo* attribute, float delta, UInt32 mode, bool capped)
	{
		if (!attribute)
			return FD("attribute is empty");

		const AttributeInfo ai = g_AttributesManager.GetRegistrationInfo(attribute);
		auto senders = ToArray(sender);
		return DecreaseInternal(senders, actor, attribute, delta, mode, ai, capped);
	}

	bool Decrease(StaticFunctionTag* _, TESForm* sender, Actor* actor, ActorValueInfo* attribute, float delta, UInt32 mode)
	{
		const bool res = DecreaseCommon(sender, actor, attribute, delta, mode, false);
		D("Decrease %08X: called with result %s", attribute?attribute->formID: 0, S(res));
		return res;
	}
	bool DecreaseCapped(StaticFunctionTag* _, TESForm* sender, Actor* actor, ActorValueInfo* attribute, float delta, UInt32 mode)
	{
		const bool res = DecreaseCommon(sender, actor, attribute, delta, mode, true);
		D("DecreaseCapped: called with result %s", S(res));
		return res;
	}


	bool IncreaseInternal(std::vector<TESForm*>& senders, Actor* actor, ActorValueInfo* attribute, float delta, UInt32 mode, AttributeInfo ai, bool capped)
	{
		if ((attribute->avFlags & ActorValueInfo::kFlag_DamageIsPositive) != 0)
		{
			return FD("DamageIsPositive AV %08X", attribute->formID); // don't want to support this idea. it looks redundant
		}

		ModifyResult data;
		const bool res = AttributeModifier::Increase(actor, attribute, ai, delta, data, AttributeInfo::ModeFromInt(mode), capped);
		if (!res)
			return FD("IncreaseInternal can't increase");
		
		g_Subscriptions.NotifyIncreased(senders, actor, attribute, data.NewValue, data.PrevValue, data.Exceed);
		return true;
	}

	bool IncreaseCommon(TESForm* sender, Actor* actor, ActorValueInfo* attribute, float delta, UInt32 mode, bool capped)
	{
		if (!attribute)
			return FD("attribute is empty");
		const AttributeInfo ai = g_AttributesManager.GetRegistrationInfo(attribute);
		auto senders = ToArray(sender);
		return IncreaseInternal(senders, actor, attribute, delta, mode, ai, capped);
	}

	bool Increase(StaticFunctionTag* _, TESForm* sender, Actor* actor, ActorValueInfo* attribute, float delta, UInt32 mode)
	{
		const bool res = IncreaseCommon(sender, actor, attribute, delta, mode, false);
		D("Increase %08X: called with result %s", attribute?attribute->formID: 0, S(res));
		return res;
	}
	bool IncreaseCapped(StaticFunctionTag* _, TESForm* sender, Actor* actor, ActorValueInfo* attribute, float delta, UInt32 mode)
	{
		const bool res = IncreaseCommon(sender, actor, attribute, delta, mode, true);
		D("IncreaseCapped: called with result %s", S(res));
		return res;
	}

	bool ModifyCommon(TESForm* sender, Actor* actor, ActorValueInfo* attribute, float delta, UInt32 mode, bool capped)
	{
		if (!attribute)
			return FD("attribute is empty");
		const AttributeInfo ai = g_AttributesManager.GetRegistrationInfo(attribute);
		auto senders = ToArray(sender);
		if (delta < 0)
			return DecreaseInternal(senders, actor, attribute, -delta, mode, ai, capped);
		if (delta > 0)
			return IncreaseInternal(senders, actor, attribute, delta, mode, ai, capped);
		return false;
	}

	bool Modify(StaticFunctionTag* _, TESForm* sender, Actor* actor, ActorValueInfo* attribute, float delta, UInt32 mode)
	{
		return ModifyCommon(sender, actor, attribute, delta, mode, false);
	}
	bool ModifyCapped(StaticFunctionTag* _, TESForm* sender, Actor* actor, ActorValueInfo* attribute, float delta, UInt32 mode)
	{
		return ModifyCommon(sender, actor, attribute, delta, mode, true);
	}

	bool ModifyValuesCommon(TESForm* sender, Actor* actor, VMArray<ActorValueInfo*>& avis, VMArray<float>& deltas, UInt32 mode, bool capped)
	{
		if (!actor)
			return false;
		if (avis.IsNone())
			return false;
		if (deltas.IsNone())
			return false;
		const UInt32 deltasSize = deltas.Length();
		auto senders = ToArray(sender);
		const std::vector<AttributeInfo> infos = g_AttributesManager.GetRegistrationInfo(avis);
		for (UInt32 i = 0; i < infos.size(); i++)
		{
			ActorValueInfo* attribute;
			avis.Get(&attribute, i);
			if (!attribute)
			{
				D("None attribute at index %d", i);
				continue;
			}
			if (i >= deltasSize)
				return TD("deltas length (%d) < attributes length (%d)", deltasSize, infos.size()); // no more data
			float delta;
			deltas.Get(&delta, i);
			if (delta < 0)
				DecreaseInternal(senders, actor, attribute, -delta, mode, infos[i], capped);
			else if (delta > 0)
				IncreaseInternal(senders, actor, attribute, delta, mode, infos[i], capped);
		}
		return true;
	}

	bool ModifyValues(StaticFunctionTag* _, TESForm* sender, Actor* actor, VMArray<ActorValueInfo*> avis, VMArray<float> deltas, UInt32 mode)
	{
		return ModifyValuesCommon(sender, actor, avis, deltas, mode, false);
	}
	bool ModifyValuesCapped(StaticFunctionTag* _, TESForm* sender, Actor* actor, VMArray<ActorValueInfo*> avis, VMArray<float> deltas, UInt32 mode)
	{
		return ModifyValuesCommon(sender, actor, avis, deltas, mode, true);
	}

	bool SetInternal(std::vector<TESForm*>& senders, Actor* actor, ActorValueInfo* attribute, float value, UInt32 mode, AttributeInfo ai)
	{
		if ((attribute->avFlags & ActorValueInfo::kFlag_DamageIsPositive) != 0)
		{
			return FD("DamageIsPositive AV %08X", attribute->formID); // don't want to support this idea. it looks redundant
		}
		ModifyResult data;
		const bool res = AttributeModifier::Set(actor, attribute, ai, value, data, AttributeInfo::ModeFromInt(mode));
		if (!res)
			return false;
		if (data.Increase)
			g_Subscriptions.NotifyIncreased(senders, actor, attribute, data.NewValue, data.PrevValue, data.Exceed);
		else
			g_Subscriptions.NotifyDecreased(senders, actor, attribute, data.NewValue, data.PrevValue, data.Exceed);
		return true;
	}

	bool Set(StaticFunctionTag* _, TESForm* sender, Actor* actor, ActorValueInfo* attribute, float value, UInt32 mode)
	{
		if (!attribute)
			return FD("attribute is empty");

		const AttributeInfo ai = g_AttributesManager.GetRegistrationInfo(attribute);
		auto senders = ToArray(sender);
		const bool res = SetInternal(senders, actor, attribute, value, mode, ai);
		D("Set: called with result %s", S(res));
		return res;
	}

	bool SetValues(StaticFunctionTag* _, TESForm* sender, Actor* actor, VMArray<ActorValueInfo*> avis, VMArray<float> values, UInt32 mode)
	{
		if (!actor)
			return FD("Actor is None");;
		if (avis.IsNone())
			return false;
		if (values.IsNone())
			return false;
		const UInt32 valuesSize = values.Length();
		auto senders = ToArray(sender);
		const std::vector<AttributeInfo> infos = g_AttributesManager.GetRegistrationInfo(avis);
		for (UInt32 i = 0; i < infos.size(); i++)
		{
			ActorValueInfo* attribute;
			avis.Get(&attribute, i);
			if (!attribute)
				continue;
			if (i >= valuesSize)
				return true; // no more data
			float value;
			values.Get(&value, i);
			SetInternal(senders, actor, attribute, value, mode, infos[i]);
		}
		return true;
	}

	DECLARE_STRUCT(Parameters, AA_AV_PAPYRUS_SCRIPT);

	bool SetRandomInternal(std::vector<TESForm*>& senders, Actor* actor, ActorValueInfo* attribute, AttributeInfo& ai, Parameters& param)
	{
		UInt32 modeInt;
		if (!param.Get<UInt32>("Mode", &modeInt))
			return FD("Failed to get Mode");

		auto const initMode = AttributeInfo::ModeFromInt(modeInt);
		if (initMode == AttributeInitMode::Default)
		{
			return SetInternal(senders, actor, attribute, ai.DefaultValue, AttributeInitMode::Default, ai);
		}
		if (initMode == AttributeInitMode::Uniform)
		{
			float min, max;
			if (!param.Get<float>("Min", &min))
				return FD("Failed to get Min");
			if (!param.Get<float>("Max", &max))
				return FD("Failed to get Max");

			const float rnd = ai.GetRandomUniform(min, max);
			return SetInternal(senders, actor, attribute, rnd, AttributeInitMode::Default, ai);
		}
		if (initMode == AttributeInitMode::Normal)
		{
			float mean, deviation;
			if (!param.Get<float>("Mean", &mean))
				return FD("Failed to get Mean");
			if (!param.Get<float>("Deviation", &deviation))
				return FD("Failed to get Deviation");
			const float rnd = AttributeInfo::GetRandomNormal(mean, deviation);
			return SetInternal(senders, actor, attribute, rnd, AttributeInitMode::Default, ai);
		}
		return FE("Internal error. Invalid init mode");
	}

	bool SetRandom(StaticFunctionTag* _, TESForm* sender, Actor* actor, ActorValueInfo* attribute, Parameters param)
	{
		if (!attribute)
			return FD("attribute is empty");
		if (param.IsNone())
			return FD("param is None");
		AttributeInfo ai = g_AttributesManager.GetRegistrationInfo(attribute);
		if (!ai.AV)
			return FD("Attribute not registered");

		auto senders = ToArray(sender);
		return SetRandomInternal(senders, actor, attribute, ai, param);
	}

	bool SetRandomValues(StaticFunctionTag* _, TESForm* sender, Actor* actor, VMArray<ActorValueInfo*> attributes, VMArray<Parameters> params)
	{
		if (!actor)
			return FD("Actor is None");
		if (attributes.IsNone())
			return FD("Attributes is None");
		if (params.IsNone())
			return FD("params is None");

		const UInt32 paramsSize = params.Length();
		auto senders = ToArray(sender);
		std::vector<AttributeInfo> infos = g_AttributesManager.GetRegistrationInfo(attributes);
		for (UInt32 i = 0; i < infos.size(); i++)
		{
			AttributeInfo& info = infos[i];
			if (!info.AV)
				continue;

			ActorValueInfo* attribute;
			attributes.Get(&attribute, i);
			if (!attribute)
				continue;
			if (i >= paramsSize)
				return true; // no more data

			Parameters param;
			params.Get(&param, i);
			if (param.IsNone())
				continue;
			SetRandomInternal(senders, actor, attribute, info, param);
		}
		return true;
	}

	VMArray<float> GenerateRandomValues(StaticFunctionTag* _, VMArray<ActorValueInfo*> attributes, VMArray<Parameters> params)
	{
		VMArray<float> retVal;
		retVal.SetNone(true);
		if (attributes.IsNone())
		{
			D("attributes is None");
			return retVal;
		}
		if (params.IsNone())
		{
			D("params is None");
			return retVal;
		}

		const UInt32 paramsSize = params.Length();
		const std::vector<AttributeInfo> infos = g_AttributesManager.GetRegistrationInfo(attributes);
		retVal.SetNone(false);
		for (UInt32 i = 0; i < infos.size(); i++)
		{
			float val = 0;
			const AttributeInfo& info = infos[i];
			if (!info.AV)
			{
				ActorValueInfo* avi = nullptr;
				attributes.Get(&avi, i);
				if (avi)
					val = avi->defaultBase;

				retVal.Push(&val);
				continue;
			}
			
			if (i >= paramsSize)
			{
				val = info.DefaultValue;
				retVal.Push(&val);
				continue;
			}

			Parameters param;
			params.Get(&param, i);
			if (param.IsNone())
			{
				val = info.DefaultValue;
				retVal.Push(&val);
				continue;
			}

			UInt32 modeInt;
			if (!param.Get<UInt32>("Mode", &modeInt))
				modeInt = 0;
			auto const initMode = AttributeInfo::ModeFromInt(modeInt);

			if (initMode == AttributeInitMode::Default)
			{
				val = info.DefaultValue;
			}
			else if (initMode == AttributeInitMode::Uniform)
			{
				float min, max;
				if (!param.Get<float>("Min", &min))
					min = info.RandomMin;
				if (!param.Get<float>("Max", &max))
					max = info.RandomMax;
				val = info.GetRandomUniform(min, max);
			}
			else if (initMode == AttributeInitMode::Normal)
			{
				float mean, deviation;
				if (!param.Get<float>("Mean", &mean))
					mean = info.RandomMean;
				if (!param.Get<float>("Deviation", &deviation))
					deviation = info.RandomDeviation;
				val = AttributeInfo::GetRandomNormal(mean, deviation);
			}

			retVal.Push(&val);
			
		}
		return retVal;
	}

	void ApplyChange(std::vector<TESForm*>& senders, Actor* actor, ActorValueInfo* attribute, float initialValue, float newValue)
	{
		D("ApplyChange for Actor %08X of AV %08X from %f to %f", actor->formID, attribute->formID, initialValue, newValue);
		AttributeInfo ai = g_AttributesManager.GetRegistrationInfo(attribute);
		if (initialValue > newValue)
		{
			ModifyResult data;
			if (AttributeModifier::Decrease(actor, attribute, ai, initialValue - newValue, data, AttributeInitMode::Default, false))
			{
				g_Subscriptions.NotifyDecreased(senders, actor, attribute, data.NewValue, data.PrevValue, data.Exceed);
			}
		}
		else if (initialValue < newValue)
		{
			ModifyResult data;
			if (AttributeModifier::Increase(actor, attribute, ai, newValue - initialValue, data, AttributeInitMode::Default, false))
			{
				g_Subscriptions.NotifyIncreased(senders, actor, attribute, data.NewValue, data.PrevValue, data.Exceed);
			}
		}
	}


	void Register(VirtualMachine* vm)
	{
		vm->RegisterFunction(
			new NativeFunction8("RegisterAttribute", EXPORT_PAPYRUS_SCRIPT, RegisterAttribute, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "RegisterAttribute", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction6("UpdateAttributeRegistration", EXPORT_PAPYRUS_SCRIPT, UpdateAttributeRegistration, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "UpdateAttributeRegistration", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("UnregisterAttribute", EXPORT_PAPYRUS_SCRIPT, UnregisterAttribute, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "UnregisterAttribute", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("IsAttributeRegistered", EXPORT_PAPYRUS_SCRIPT, IsAttributeRegistered, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "IsAttributeRegistered", IFunction::kFunctionFlag_NoWait);


		vm->RegisterFunction(
			new NativeFunction3("Get", EXPORT_PAPYRUS_SCRIPT, Get, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "Get", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction2("GetMax", EXPORT_PAPYRUS_SCRIPT, GetMax, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetMax", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction3("GetValues", EXPORT_PAPYRUS_SCRIPT, GetValues, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetValues", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction2("GetMaximums", EXPORT_PAPYRUS_SCRIPT, GetMaximums, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "GetMaximums", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction4("EnsureInitialized", EXPORT_PAPYRUS_SCRIPT, EnsureInitialized, vm));
		vm->SetFunctionFlags(EXPORT_PAPYRUS_SCRIPT, "EnsureInitialized", IFunction::kFunctionFlag_NoWait);


		// modifications/notifications related:
		
		vm->RegisterFunction(
			new NativeFunction5("Decrease", AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, Decrease, vm));
		vm->SetFunctionFlags(AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, "Decrease", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction5("Increase", AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, Increase, vm));
		vm->SetFunctionFlags(AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, "Increase", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction5("Modify", AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, Modify, vm));
		vm->SetFunctionFlags(AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, "Modify", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction5("DecreaseCapped", AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, DecreaseCapped, vm));
		vm->SetFunctionFlags(AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, "DecreaseCapped", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction5("IncreaseCapped", AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, IncreaseCapped, vm));
		vm->SetFunctionFlags(AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, "IncreaseCapped", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction5("ModifyCapped", AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, ModifyCapped, vm));
		vm->SetFunctionFlags(AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, "ModifyCapped", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction5("Set", AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, Set, vm));
		vm->SetFunctionFlags(AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, "Set", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction5("ModifyValues", AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, ModifyValues, vm));
		vm->SetFunctionFlags(AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, "ModifyValues", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction5("ModifyValuesCapped", AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, ModifyValuesCapped, vm));
		vm->SetFunctionFlags(AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, "ModifyValuesCapped", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction5("SetValues", AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, SetValues, vm));
		vm->SetFunctionFlags(AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, "SetValues", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction4("SetRandom", AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, SetRandom, vm));
		vm->SetFunctionFlags(AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, "SetRandom", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction4("SetRandomValues", AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, SetRandomValues, vm));
		vm->SetFunctionFlags(AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, "SetRandomValues", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction2("GenerateRandomValues", AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, GenerateRandomValues, vm));
		vm->SetFunctionFlags(AttributesAPIExport::EXPORT_PAPYRUS_SCRIPT, "GenerateRandomValues", IFunction::kFunctionFlag_NoWait);

	}
};