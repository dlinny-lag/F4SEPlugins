#pragma once
#include "Shared/VMValueEqual.hpp"
#include "f4se/PapyrusVM.h"

namespace Testers
{
	struct StructObject
	{
		StructObject() = delete;
		virtual ~StructObject() = default;
		static VMValue GetNone()
		{
			VMValue retVal;
			retVal.SetNone();
			return retVal;
		}
		VMValue AsVMValue()
		{
			VMValue::StructData* structObject = nullptr;
			VirtualMachine* vm = (*g_gameVM)->m_virtualMachine;
			if (vm->CreateStruct(&StructName, &structObject))
			{
				// Setup all field types and values
				structTypeInfo->m_members.ForEach([&structObject, this](const VMStructTypeInfo::MemberItem* fInfo)
				{
					structObject->GetStruct()[fInfo->index].type.value = structTypeInfo->m_data.entries[fInfo->index].m_type;
					SetValueForField(fInfo->name.c_str(), structObject->GetStruct()[fInfo->index]);
					return true;
				});
			}
			VMValue retVal;
			retVal.SetComplexType(structTypeInfo);
			retVal.data.strct = structObject;
			return retVal;
		}
	protected:
		const BSFixedString StructName;
		StructObject(const BSFixedString& structName)
			:StructName(structName)
			,structTypeInfo(nullptr)
		{
			VirtualMachine* vm = (*g_gameVM)->m_virtualMachine;
			// it is test, no checks for null or check for return value is needed
			vm->GetStructTypeInfo(&structName, &structTypeInfo);
			structTypeInfo->Release();
		}
		virtual void SetValueForField(const char* fieldName, VMValue& toSet) = 0;
	private:
		VMStructTypeInfo* structTypeInfo;
	};

	// struct is defined in AchievementsScript.psc
	struct StatAchievement : StructObject
	{
		static constexpr char StructName[] = "AchievementsScript#StatAchievement";
		const std::string StatName;
		const SInt32 Threshold;
		const SInt32 AchievementNumber;
		StatAchievement() = delete;
		StatAchievement(std::string statName, SInt32 threshold, SInt32 achievementNumber)
			:StructObject(StructName)
			,StatName(std::move(statName))
			,Threshold(threshold)
			,AchievementNumber(achievementNumber)
		{
			
		}

	protected:
		void SetValueForField(const char* fieldName, VMValue& toSet) override
		{
			if (_stricmp("StatName", fieldName) == 0)
				toSet.SetString(StatName.c_str());
			else if (_stricmp("Threshold", fieldName) == 0)
				toSet.SetInt(Threshold);
			else if (_stricmp("AchievementNumber", fieldName) == 0)
				toSet.SetInt(AchievementNumber);
		}
	};

	struct StructHelper
	{
		static bool ValidateObject(const VMValue& value, const BSFixedString& expectedName)
		{
			VMStructTypeInfo* info = StructValueHelper::GetStructType(value);
			if (!info)
				return false;
			return std::equal_to()(info->m_typeName, expectedName);
		}
	};
}
