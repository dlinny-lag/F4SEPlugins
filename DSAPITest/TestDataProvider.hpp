#pragma once
#include <array>
#include "f4se/GameForms.h"
#include "StructHelper.hpp"

namespace Testers
{
	template <typename ELEMENT_TYPE>
	struct TestDataProvider
	{
		static constexpr size_t TestDataSize = 5;
		const std::array<ELEMENT_TYPE, TestDataSize>& GetData() const
		{
			if (!obtained)
			{
				data = std::move(GenerateData());
				obtained = true;
			}
			return data;
		}
		virtual ~TestDataProvider() = default;
	protected:
		TestDataProvider()
			:obtained(false)
		{
			
		}
		virtual std::array<ELEMENT_TYPE, TestDataSize> GenerateData() const = 0;
	private:
		mutable bool obtained;
		mutable std::array<ELEMENT_TYPE, TestDataSize> data;
	};

	struct IntDataProvider final : TestDataProvider<SInt32>
	{
		std::array<SInt32, TestDataSize> GenerateData() const override
		{
			return {5,4,3,2,1};
		}
	};

	struct FloatDataProvider final : TestDataProvider<float>
	{
		std::array<float, TestDataSize> GenerateData() const override
		{
			return {5.0f,4.0f,3.0f,2.0f,10.f};
		}
	};

	struct StringDataProvider final : TestDataProvider<BSFixedString>
	{
		std::array<BSFixedString, TestDataSize> GenerateData() const override
		{
			return {"5","4","3","2","1"};
		}
	};

	
	struct FormDataProvider final : TestDataProvider<FORM_TYPE>
	{
		std::array<FORM_TYPE, TestDataSize> GenerateData() const override
		{
			return
			{
				LookupFormByID(0x00000820), // ALCH:HC_SleepEffect_Incapacitated
				LookupFormByID(0x0000081E), // ALCH:HC_SleepEffect_Exhausted
				LookupFormByID(0x0000081C), // ALCH:HC_SleepEffect_Weary
				LookupFormByID(0x0000081A), // ALCH:HC_SleepEffect_Overtired
				LookupFormByID(0x00000818), // ALCH:HC_SleepEffect_Tired
			};
		}
	};

	template <typename StructObjectType>
	struct StructDataProvider : TestDataProvider<VMValue>
	{
		static_assert(std::is_base_of_v<StructObject, StructObjectType>);
	};

	struct StatAchievementProvider final : StructDataProvider<StatAchievement>
	{
		std::array<VMValue, TestDataSize> GenerateData() const override
		{
			return
			{
				StatAchievement("Five", 5, 5).AsVMValue(),
				StatAchievement("Four", 4, 4).AsVMValue(),
				StatAchievement("Three", 3, 3).AsVMValue(),
				StatAchievement("Two", 2, 2).AsVMValue(),
				StatAchievement("One", 1, 1).AsVMValue()
			};
		}
	};

}