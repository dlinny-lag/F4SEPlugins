#pragma once
#include "f4se/GameForms.h"
#include "../DS/CollectionAccessor.h"
#include <array>

#include "f4se/PapyrusVM.h"
#include "Shared/BSFixedStringEqual.hpp"
#include "StructHelper.hpp"
#include "TestDataProvider.hpp"

namespace Testers
{
	template <typename Array, typename ELEMENT_TYPE>
	struct ArrayTester
	{
	protected:
		std::string arrayName;
		BGSKeyword* id;
		Array array;
		bool dataValid;
		const TestDataProvider<ELEMENT_TYPE>& provider;
	public:
		ArrayTester(std::string arrayName, BGSKeyword* id, const Array& accessor, const TestDataProvider<ELEMENT_TYPE>& provider)
			:arrayName(std::move(arrayName))
			,id(id)
			,array(accessor)
			,dataValid(true)
			,provider(provider)
		{
			
		}
		virtual ~ArrayTester() = default;

		
		void Perform(const volatile bool* needCancel)
		{
			if (!dataValid)
			{
				E("%sArray: Can't test with invalid data", arrayName.c_str());
				return;
			}
			// 1. create array
			// 2. add element, get by index
			// 3. add range, get all by index, IndexOf
			// 4. remove element, IndexOf should not found it
			// 5. sort, should have correct index
			// 5.1 for primitive array - call ToArray and compare 
			// 6. clear array, size should be 0
			// 7. remove array, it should not be found
			PerformInternal(needCancel);
			if (*needCancel) return;
			CleanUp();
		}

		virtual void PerformInternal(const volatile bool* needCancel)
		{
			if (*needCancel) return;
			bool result = Create();
			if (!result)
				I("%s: Create failed",arrayName.c_str());
			if (!result || *needCancel) return;

			result = AddOne();
			if (!result)
				I("%s: AddOne failed",arrayName.c_str());
			if (!result || *needCancel) return;

			result = GetFirst();
			if (!result)
				I("%s: GetFirst failed",arrayName.c_str());
			if (!result || *needCancel) return;

			result = AddRange();
			if (!result)
				I("%s: AddRange failed",arrayName.c_str());
			if (!result || *needCancel) return;

			result = Size();
			if (!result)
				I("%s: Size failed",arrayName.c_str());
			if (!result || *needCancel) return;

			result = GetAllByIndex();
			if (!result)
				I("%s: GetAllByIndex failed",arrayName.c_str());
			if (!result || *needCancel) return;

			result = IndexOfShouldBeFound();
			if (!result)
				I("%s: IndexOfShouldBeFound failed",arrayName.c_str());
			if (!result || *needCancel) return;

			result = Remove();
			if (!result)
				I("%s: Remove failed",arrayName.c_str());
			if (!result || *needCancel) return;

			result = IndexOfShouldNotBeFound();
			if (!result)
				I("%s: IndexOfShouldNotBeFound failed",arrayName.c_str());
			if (!result || *needCancel) return;
		}
		void CleanUp()
		{
			const bool result = Delete();
			I("%s: Delete = %s" ,arrayName.c_str(), S(result));
		}

		virtual bool Create() = 0;

		bool AddOne()
		{
			return array.Add(id, provider.GetData()[0]);
		}
		bool GetFirst()
		{
			std::optional<ELEMENT_TYPE> first = array.Get(id, 0);
			if (!first.has_value())
				return false;
			return std::equal_to<ELEMENT_TYPE>()(first.value(), provider.GetData()[0]);
		}
		bool AddRange()
		{
			std::vector<ELEMENT_TYPE> toAdd(provider.GetData().begin()+1, provider.GetData().end());
			return array.AddRange(id, toAdd);
		}
		bool Size()
		{
			return 5 == array.Size(id);
		}
		bool GetAllByIndex()
		{
			for(int i = 0; i < provider.TestDataSize; i++)
			{
				const std::optional<ELEMENT_TYPE> val = array.Get(id, i);
				if (!val.has_value())
					return false;
				if (!std::equal_to<ELEMENT_TYPE>()(val.value(), provider.GetData()[i]))
					return false;
			}
			return true;
		}
		virtual bool IndexOfShouldBeFound() = 0;
		bool Remove()
		{
			return array.Remove(id, 0);
		}
		virtual bool IndexOfShouldNotBeFound() = 0;

		bool Delete()
		{
			return array.Delete(id);
		}
	};

	template <typename Array, typename ELEMENT_TYPE>
	struct PrimitiveArrayTester : ArrayTester<Array, ELEMENT_TYPE>
	{
		PrimitiveArrayTester(std::string elementName, BGSKeyword* id, const Array& accessor, const TestDataProvider<ELEMENT_TYPE>& provider)
			: ArrayTester(std::move(elementName + "Array"), id, accessor, provider)
		{

		}

		bool Create() override
		{
			return array.Create(id);
		}

		void PerformInternal(const volatile bool* needCancel) override
		{
			ArrayTester::PerformInternal(needCancel);
			const bool result = ToArray();
			if (!result)
				I("%s: ToArray failed", ArrayTester::arrayName.c_str());
		}
		bool IndexOfShouldBeFound() override
		{
			for(SInt32 i = 0; i < ArrayTester::provider.TestDataSize; i++)
			{
				const SInt32 index = ArrayTester::array.IndexOf(id, ArrayTester::provider.GetData()[i], 0);
				if (index != i)
					return false;
			}
			return true;
		}
		bool IndexOfShouldNotBeFound() override
		{
			return -1  == array.IndexOf(id, ArrayTester::provider.GetData()[0], 0);
		}
		bool ToArray()
		{
			// first element was removed
			std::optional<DSAPI::transferable_vector<ELEMENT_TYPE>> result = ArrayTester::array.ToArray(id);
			if (!result.has_value())
				return false;
			std::vector<ELEMENT_TYPE> data = result.value();
			for(int i = 0; i < ArrayTester::provider.TestDataSize-1; i++)
			{
				if (!std::equal_to<ELEMENT_TYPE>()(data[i], ArrayTester::provider.GetData()[i+1]))
					return false;
			}
			return true;
		}
	};
}



namespace Testers
{
	template <typename Array, typename StructObjectType>
	struct StructArrayTester : ArrayTester<Array, VMValue>
	{
		BSFixedString fieldName0; // name of field with index = 0
		
		StructArrayTester(BGSKeyword* id, const Array& accessor, const StructDataProvider<StructObjectType>& provider)
			: ArrayTester(std::move(std::string("StructArray-") + std::string(StructObjectType::StructName)), id, accessor, provider)
		{
			for(const auto& val : provider.GetData())
			{
				if (!StructHelper::ValidateObject(val, StructObjectType::StructName))
				{
					ArrayTester::dataValid = false;
					E("Invalid struct object");
				}
			}
			VMStructTypeInfo* info = StructValueHelper::GetStructType(provider.GetData()[0]);

			info->m_members.ForEach([this](const VMStructTypeInfo::MemberItem* item)
			{
				if (item->index == 0)
				{
					fieldName0 = item->name;
					return false; // terminate
				}
				return true;
			});
		}

		bool Create() override
		{
			return ArrayTester::array.Create(id, StructObjectType::StructName);
		}
		bool IndexOfShouldBeFound() override
		{
			for(SInt32 i = 0; i < ArrayTester::provider.TestDataSize; i++)
			{
				const SInt32 index = ArrayTester::array.IndexOf(id, fieldName0, *(ArrayTester::provider.GetData()[i].data.strct->GetStruct()), 0);
				if (index != i)
					return false;
			}
			return true;
		}
		bool IndexOfShouldNotBeFound() override
		{
			return -1  == ArrayTester::array.IndexOf(id, fieldName0, *(ArrayTester::provider.GetData()[0].data.strct->GetStruct()), 0);
		}
	};
}