#pragma once
#include "f4se/GameForms.h"
#include <array>
#include "Shared/MyTrace.h"
#include "TestDataProvider.hpp"

namespace Testers
{
	template <typename Set, typename ELEMENT_TYPE>
	struct SetTester
	{
	protected:
		std::string elementName;
		BGSKeyword* id;
		Set set;
		const TestDataProvider<ELEMENT_TYPE>& provider;
		const bool apiValid;
	public:
		SetTester(std::string elementName, BGSKeyword* id, const Set& accessor, const TestDataProvider<ELEMENT_TYPE>& provider)
			:elementName(std::move(elementName))
			,id(id)
			,set(accessor)
			,provider(provider)
			,apiValid(set.Version > 0)
		{
			
		}
		virtual ~SetTester() = default;

		void Perform(const volatile bool* needCancel)
		{
			if (!apiValid)
			{
				E("%sSet: Can't test with no methods initialized", elementName.c_str());
				return;
			}
			PerformInternal(needCancel);
			if (*needCancel) return;
			CleanUp();
		}
		virtual void PerformInternal(const volatile bool* needCancel)
		{
			if (*needCancel) return;
			bool result = Create();
			if (!result)
				I("%sSet: Create failed", elementName.c_str());
			if (!result || *needCancel) return;

			result = Add();
			if (!result)
				I("%sSet: Add failed", elementName.c_str());
			if (!result || *needCancel) return;

			result = Length1();
			if (!result)
				I("%sSet: Length1 failed", elementName.c_str());
			if (!result || *needCancel) return;

			result = AddAll();
			if (!result)
				I("%sSet: AddAll failed", elementName.c_str());
			if (!result || *needCancel) return;

			result = LengthAll();
			if (!result)
				I("%sSet: LengthAll failed", elementName.c_str());
			if (!result || *needCancel) return;

			result = ShouldContainAll();
			if (!result)
				I("%sSet: ShouldContainAll failed", elementName.c_str());
			if (!result || *needCancel) return;

			result = Remove1();
			if (!result)
				I("%sSet: Remove1 failed", elementName.c_str());
			if (!result || *needCancel) return;

			result = ShouldNotContain1();
			if (!result)
				I("%sSet: ShouldNotContain1 failed", elementName.c_str());
			if (!result || *needCancel) return;

			result = RemoveRange();
			if (!result)
				I("%sSet: RemoveRange failed", elementName.c_str());
			if (!result || *needCancel) return;
		}

		bool Add()
		{
			return set.Add(id, provider.GetData()[0]);
		}
		bool Length1()
		{
			return 1 == set.Size(id);
		}
		bool AddAll()
		{
			std::vector<ELEMENT_TYPE> toAdd(provider.GetData().begin(), provider.GetData().end());
			return provider.TestDataSize - 1 == set.AddRange(id, toAdd);
		}
		bool LengthAll()
		{
			return provider.TestDataSize == set.Size(id);
		}
		bool ShouldContainAll()
		{
			for (const auto& val : provider.GetData())
			{
				if (!set.Contains(id, val))
					return false;
			}
			return true;
		}
		bool Remove1()
		{
			return set.Remove(id, provider.GetData()[0]);
		}
		bool ShouldNotContain1()
		{
			return !set.Contains(id, provider.GetData()[0]);
		}

		bool RemoveRange()
		{
			std::vector<ELEMENT_TYPE> toRemove(provider.GetData().begin(), provider.GetData().end());
			return provider.TestDataSize - 1 ==  set.RemoveRange(id, toRemove); // we removed 1 element already
		}

		void CleanUp()
		{
			const bool result = Delete();
			I("%sSet: Delete = %s", elementName.c_str(), S(result));
		}
		bool Create()
		{
			return set.Create(id);
		}
		bool Delete()
		{
			return set.Delete(id);
		}
	};
}