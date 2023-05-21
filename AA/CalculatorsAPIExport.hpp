#pragma once
#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/GameReferences.h"
#include "f4se/PapyrusEvents.h"
#include "InternalAPIExport.hpp"
#include "CalculatorsHolder.h"
#include "CalculationSession.h"
#include "AVChangesAPIExport.hpp"
#include "AttributesAPIExport.hpp"
#include "CalculatorAttributesIndices.hpp"

#include <unordered_set>
#include "MyTrace.h"
#include "shared/ScriptHandle.hpp"

namespace CalculatorsAPIExport
{
	BSReadWriteLock calculatorsLock;
	CalculatorsHolder calculators;

	void DumpAll()
	{
		BSReadLocker lock(&calculatorsLock);
		calculators.DumpAll();
	}

	constexpr UInt32 rootTag = 'CALC';
	bool Save(const F4SESerializationInterface* intfc)
	{
		if (intfc->OpenRecord(rootTag, CalculatorsHolder::currentDataStructureVersion))
		{
			BSReadLocker lock(&calculatorsLock);
			return calculators.Save(intfc);
		}
		return FE("Failed to open record (%.4s)", &rootTag);
	}

	bool Load(const F4SESerializationInterface* intfc)
	{
		UInt32 recType, length, curVersion;
		if (intfc->GetNextRecordInfo(&recType, &curVersion, &length))
		{
			if (recType != rootTag)
			{
				return FE("Invalid record type (%.4s). Expected (%.4s)", &recType, &rootTag);
			}
			BSWriteLocker lock(&calculatorsLock);
			return calculators.Load(intfc, curVersion);
		}
		return FE("CalculatorsAPIExport. Failed to get record.");
	}

	void Clear()
	{
		BSWriteLocker lock(&calculatorsLock);
		calculators.Clear();
		CalculationSession::Clear();
	}

	bool __RegisterCalculator(StaticFunctionTag* _, VMObject* calculator, VMArray<ActorValueInfo*> attributes)
	{
		if (!calculator)
			return FD("Attempt to register empty calculator");

		const ScriptHandle script(calculator);

		if (script.IsInvalid())
			return FW("Invalid script");
		if (attributes.Length() == 0)
			return FD("Attempt to register empty AV list for calculator [%s]", script.Name);
		std::unordered_set<UInt32> valid;
		for (const auto attribute : g_AttributesManager.GetRegisteredAttributes())
		{
			if (attribute)
				valid.insert(attribute->formID);
		}

		std::vector<ActorValueInfo*> toRegister;
		toRegister.reserve(attributes.Length());
		for (UInt32 i = 0; i < attributes.Length(); i++)
		{
			ActorValueInfo* avi;
			attributes.Get(&avi, i);
			if (!avi)
				return FD("Empty AV for calculator [%s]", script.Name);
			if (valid.find(avi->formID) == valid.end())
				return FD("Unregistered AV %08X", avi->formID);
			toRegister.push_back(avi);
		}

		bool res;
		{
			BSWriteLocker lock(&calculatorsLock);
			res = calculators.Register(script, toRegister);
		}
		D("__RegisterCalculator: [%s] with result=%s", script.Name, S(res));
		return res;
	}
	bool __UnregisterCalculator(StaticFunctionTag* _, VMObject* calculator)
	{
		if (!calculator)
			return FD("Attempt to unregister empty calculator");
		const ScriptHandle script(calculator);
		if (script.IsInvalid())
			return FW("Invalid script");
		bool res;
		{
			BSWriteLocker lock(&calculatorsLock);
			res = calculators.Unregister(script);
		}
		D("__UnregisterCalculator: [%s] with result=%s", script.Name.c_str(), S(res));
		return res;
	}

	bool __IsCalculatorRegistered(StaticFunctionTag* _, VMObject* calculator)
	{
		BSWriteLocker lock(&calculatorsLock);
		const ScriptHandle script(calculator);
		if (script.IsInvalid())
			return FD("Invalid script");
		return calculators.IsRegistered(script);
	}

	VMArray<VMObject> __GetCalculators(StaticFunctionTag* _)
	{
		VirtualMachine * vm = (*g_gameVM)->m_virtualMachine;
		BSWriteLocker lock(&calculatorsLock);
		const auto sorted = calculators.GetSorted();
		VMArray<VMObject> retVal;
		for(auto script : sorted)
		{
			VMObject obj = script.AsObject(vm);
			retVal.Push(&obj);
		}
		return retVal;
	}

	UInt32 __StartCalculationSession(StaticFunctionTag* _, Actor* actor, UInt32 mode)
	{
		const std::vector<ActorValueInfo*> allAttributes = g_AttributesManager.GetRegisteredAttributes();
		return CalculationSession::Open(actor, allAttributes, AttributeInfo::ModeFromInt(mode));
	}

	VMArray<float> __GetCalculatorValues(StaticFunctionTag* _, UInt32 sessionId, VMObject* calculator)
	{
		VMArray<float> retVal;
		retVal.SetNone(true);
		if (!calculator)
		{
			D("Empty calculator");
			return retVal;
		}
		const UInt64 handle = SafeGetHandle(calculator);
		if (handle == 0)
		{
			W("Invalid script");
			return retVal;
		}
		retVal.SetNone(false);

		const std::vector<ActorValueInfo*> allAttributes = CalculationSession::GetAllAttributes(sessionId);
		const ScriptHandle script(calculator);
		const std::vector<UInt32> idx = CalculatorAttributesIndices::Get(script, calculators.Get(script), allAttributes);
		const std::vector<float> values = CalculationSession::GetValues(sessionId, idx);
		for (float val : values)
		{
			retVal.Push(&val);
		}
		D("Got %d values for calculator %s in scope of session %d", retVal.Length(), script.Name.c_str(), sessionId);
		return retVal;
	}


	void __UpdateCalculatorValues(StaticFunctionTag* _, UInt32 sessionId, VMObject* calculator, VMArray<float> calculatedValues)
	{
		if (!calculator)
		{
			D("Empty calculator");
			return;
		}
		const UInt64 handle = SafeGetHandle(calculator);
		if (handle == 0)
		{
			W("Invalid script");
			return;
		}
		const ScriptHandle script(calculator);
		const std::vector<ActorValueInfo*> allAttributes = CalculationSession::GetAllAttributes(sessionId);
		const std::vector<UInt32> idx = CalculatorAttributesIndices::Get(script, calculators.Get(script), allAttributes);

		std::vector<float> newValues;
		newValues.reserve(calculatedValues.Length());
		for (UInt32 i = 0; i < calculatedValues.Length(); i++)
		{
			float val;
			calculatedValues.Get(&val, i);
			newValues.push_back(val);
		}
		D("Set %d values for calculator %s in scope of session %d", newValues.size(), script.Name.c_str(), sessionId);
		CalculationSession::SetValues(sessionId, idx, newValues);
	}

	void __CloseCalculationSession(StaticFunctionTag* _, VMArray<TESForm*> senders, UInt32 sessionId)
	{
		const auto session = CalculationSession::Close(sessionId);
		std::vector<TESForm*> sendersCopy;
		const UInt32 len = senders.Length();
		sendersCopy.reserve(len);
		for(UInt32 i = 0; i < len; i++)
		{
			TESForm* form;
			senders.Get(&form, i);
			sendersCopy.emplace_back(form);
		}
		//session.Dump();
		session.ForEach(AttributesAPIExport::ApplyChange, sendersCopy);
	}

	void Register(VirtualMachine* vm)
	{
		vm->RegisterFunction(
			new NativeFunction2("__RegisterCalculator", InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, __RegisterCalculator, vm));
		vm->SetFunctionFlags(InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, "__RegisterCalculator", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("__UnregisterCalculator", InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, __UnregisterCalculator, vm));
		vm->SetFunctionFlags(InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, "__UnregisterCalculator", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction1("__IsCalculatorRegistered", InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, __IsCalculatorRegistered, vm));
		vm->SetFunctionFlags(InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, "__IsCalculatorRegistered", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction2("__StartCalculationSession", InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, __StartCalculationSession, vm));
		vm->SetFunctionFlags(InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, "__StartCalculationSession", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction0("__GetCalculators", InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, __GetCalculators, vm));
		vm->SetFunctionFlags(InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, "__GetCalculators", IFunction::kFunctionFlag_NoWait);


		vm->RegisterFunction(
			new NativeFunction2("__GetCalculatorValues", InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, __GetCalculatorValues, vm));
		vm->SetFunctionFlags(InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, "__GetCalculatorValues", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction3("__UpdateCalculatorValues", InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, __UpdateCalculatorValues, vm));
		vm->SetFunctionFlags(InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, "__UpdateCalculatorValues", IFunction::kFunctionFlag_NoWait);

		vm->RegisterFunction(
			new NativeFunction2("__CloseCalculationSession", InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, __CloseCalculationSession, vm));
		vm->SetFunctionFlags(InternalAPIExport::EXPORT_PAPYRUS_SCRIPT, "__CloseCalculationSession", IFunction::kFunctionFlag_NoWait);
	}
}