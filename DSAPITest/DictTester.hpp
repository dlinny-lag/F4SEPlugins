#pragma once
#include "TestDataProvider.hpp"

namespace Testers
{
	template <typename Dict, typename KeyType, typename ValueType>
	struct BaseDictTester
	{
		bool dataValid;
		std::string keyName;
		std::string valueName;
		BGSKeyword* id;
		Dict dict;
		const TestDataProvider<KeyType>& keysProvider;
		const TestDataProvider<ValueType>& valuesProvider;
		const bool apiValid;
		BaseDictTester(std::string keyName, std::string valueName, BGSKeyword* id, Dict accessor, const TestDataProvider<KeyType>& keysProvider, const TestDataProvider<ValueType>& valuesProvider)
			:dataValid(true)
			,keyName(keyName)
			,valueName(valueName)
			,id(id)
			,dict(accessor)
			,keysProvider(keysProvider)
			,valuesProvider(valuesProvider)
			,apiValid(dict.Version > 0)
		{
			
		}
		virtual ~BaseDictTester() = default;

		void Perform(const volatile bool* needCancel)
		{
			if (!dataValid)
			{
				E("%sDict%s: Can't test with invalid data", keyName.c_str(), valueName.c_str());
				return;
			}
			if (!apiValid)
			{
				E("%sDict%s: Can't test with no methods initialized", keyName.c_str(), valueName.c_str());
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
				I("%sDict%s: Create failed", keyName.c_str(), valueName.c_str());
			if (!result || *needCancel) return;

			result = Add1();
			if (!result)
				I("%sDict%s: Add1 failed", keyName.c_str(), valueName.c_str());
			if (!result || *needCancel) return;

			result = Size1();
			if (!result)
				I("%sDict%s: Size1 failed", keyName.c_str(), valueName.c_str());
			if (!result || *needCancel) return;

			result = Contains1();
			if (!result)
				I("%sDict%s: Contains1 failed", keyName.c_str(), valueName.c_str());
			if (!result || *needCancel) return;


			// TODO: implement tests
		}

		virtual bool Add1() = 0;

		bool Size1()
		{
			return 1 == dict.Size(id);
		}
		bool Contains1()
		{
			return dict.Contains(id, keysProvider.GetData()[0]);
		}

		void CleanUp()
		{
			const bool result = Delete();
			I("%sDict%s: Delete = %s", keyName.c_str(), valueName.c_str(), S(result));
		}

		virtual bool Create() = 0;

		bool Delete()
		{
			return dict.Delete(id);
		}
	};

	template <typename Dict, typename KeyType, typename ValueType>
	struct ValueDictTester : BaseDictTester<Dict, KeyType, ValueType>
	{
		ValueDictTester(std::string keyName, std::string valueName, BGSKeyword* id, Dict accessor, const TestDataProvider<KeyType>& keysProvider, const TestDataProvider<ValueType>& valuesProvider)
			:BaseDictTester(std::move(keyName), std::move(valueName), id, accessor, keysProvider, valuesProvider)
		{
			
		}
		bool Add1() override
		{
			return BaseDictTester::dict.Add(id, BaseDictTester::keysProvider.GetData()[0], BaseDictTester::valuesProvider.GetData()[0]);
		}
	};

	template <typename Dict, typename KeyType, typename ValueType>
	struct PrimitiveValueDictTester : ValueDictTester<Dict, KeyType, ValueType>
	{
		PrimitiveValueDictTester(std::string keyName, std::string valueName, BGSKeyword* id, Dict accessor, const TestDataProvider<KeyType>& keysProvider, const TestDataProvider<ValueType>& valuesProvider)
			:ValueDictTester(std::move(keyName), std::move(valueName), id, accessor, keysProvider, valuesProvider)
		{
			
		}
		bool Create() override
		{
			return BaseDictTester::dict.Create(BaseDictTester::id);
		}
	};

	template <typename Dict, typename KeyType, typename StructObjectType>
	struct StructValueDictTester : ValueDictTester<Dict, KeyType, VMValue>
	{
		StructValueDictTester(std::string keyName, BGSKeyword* id, Dict accessor, const TestDataProvider<KeyType>& keysProvider, const StructDataProvider<StructObjectType>& valuesProvider)
			:ValueDictTester(std::move(keyName), std::string("Struct-") + std::string(StructObjectType::StructName), id, accessor, keysProvider, valuesProvider)
		{
			
		}
		bool Create() override
		{
			return BaseDictTester::dict.Create(BaseDictTester::id, StructObjectType::StructName);
		}
	};

	template <typename Dict, typename KeyType, typename ValueType>
	struct ArrayDictTester : BaseDictTester<Dict, KeyType, ValueType>
	{
		ArrayDictTester(std::string keyName, std::string arrayName, BGSKeyword* id, Dict accessor, const TestDataProvider<KeyType>& keysProvider, const TestDataProvider<ValueType>& valuesProvider)
			:BaseDictTester(std::move(keyName), std::move(arrayName), id, accessor, keysProvider, valuesProvider)
		{
			
		}
		bool Add1() override
		{
			return BaseDictTester::dict.AddElement(id, BaseDictTester::keysProvider.GetData()[0], BaseDictTester::valuesProvider.GetData()[0]);
		}
	};

	template <typename Dict, typename KeyType, typename ValueType>
	struct PrimitiveArrayDictTester : ArrayDictTester<Dict, KeyType, ValueType>
	{
		PrimitiveArrayDictTester(std::string keyName, std::string valueName, BGSKeyword* id, Dict accessor, const TestDataProvider<KeyType>& keysProvider, const TestDataProvider<ValueType>& valuesProvider)
			:ArrayDictTester(std::move(keyName), std::move(valueName+"Array"), id, accessor, keysProvider, valuesProvider)
		{
			
		}
		bool Create() override
		{
			return BaseDictTester::dict.Create(BaseDictTester::id);
		}
	};

	template <typename Dict, typename KeyType, typename StructObjectType>
	struct StructArrayDictTester : ArrayDictTester<Dict, KeyType, VMValue>
	{
		StructArrayDictTester(std::string keyName, BGSKeyword* id, Dict accessor, const TestDataProvider<KeyType>& keysProvider, const StructDataProvider<StructObjectType>& valuesProvider)
			:ArrayDictTester(keyName, std::string("StructArray-") + std::string(StructObjectType::StructName), id, accessor, keysProvider, valuesProvider)
		{
			
		}
		bool Create() override
		{
			return BaseDictTester::dict.Create(BaseDictTester::id, StructObjectType::StructName);
		}
	};

}