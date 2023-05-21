#pragma once
#include "f4se/GameForms.h"
#include "f4se/PluginAPI.h"
#include <vector>
#include "Identifiable.h"
#include "MyTrace.h"
#include "Indent.h"
#include "ValueHandler.hpp"
#include "KeyValidator.hpp"
#include <optional>

namespace DS
{
	template <typename T, typename ValueHandler = ValueHandler<T>>
	class Array : public Identifiable
	{
	public:
		Array(BGSKeyword* identifier)
			:Identifiable(identifier)
			,version(0)
		{
			
		}

		Array()
			:Identifiable(nullptr)
			,version(0)
		{

		}

		SInt32 Length()
		{
			BSReadLocker lock(&dataLock);
			return data.size();
		}
		virtual bool Add(const T& value)
		{
			BSWriteLocker lock(&dataLock);
			data.emplace_back(value);
			version++;
			return true;
		}

		virtual void AddRange(const std::vector<T>& toAdd)
		{
			BSWriteLocker lock(&dataLock);
			data.insert(data.end(), toAdd.begin(), toAdd.end());
			version++;
		}
		UInt64 Version()
		{
			BSReadLocker lock(&dataLock);
			return version;
		}

		std::optional<T> Get(SInt32 index)
		{
			if (index < 0 || index >= data.size())
				return std::nullopt;
			BSReadLocker lock(&dataLock);
			return std::optional<T>(data[index]);
		}
		virtual bool Set(SInt32 index, T& value)
		{
			if (index >= data.size())
				return false;
			BSWriteLocker lock(&dataLock);
			data[index] = value;
			version++;
			return true;
		}

		virtual bool Insert(T& value, SInt32 index)
		{
			if (index >= data.size())
				return false;
			BSWriteLocker lock(&dataLock);
			auto pos = data.begin() + index;
			data.insert(pos, value);
			version++;
			return true;
		}

		UInt32 Clear()
		{
			BSWriteLocker lock(&dataLock);
			const UInt32 retVal = data.size();
			data.clear();
			version++;
			return retVal;
		}

		bool Remove(SInt32 index)
		{
			if ( index >= data.size())
				return false;
			BSWriteLocker lock(&dataLock);
			auto pos = data.begin() + index;
			data.erase(pos);
			version++;
			return true;
		}

		bool SaveData(const F4SESerializationInterface* serializer, bool allowNoId)
		{
			Validate();
			const UInt32 len = data.size();
			if (!serializer->WriteRecordData(&len, sizeof(len)))
				return FE("Array(%08X): Failed to write length", Id());

			if (!serializer->WriteRecordData(&version, sizeof(version)))
				return FE("Array(%08X): Failed to write data version", Id());

			for (auto& val : data)
			{
				if (!valueHandler.SaveValue(serializer, &val))
					return FE("Array(%08X): Failed to save value", Id());
			}
			return true;
		}

		bool LoadData(const F4SESerializationInterface* serializer)
		{
			UInt32 len;
			
			if (!serializer->ReadRecordData(&len, sizeof(len)))
				return FE("Array(%08X): Failed to read length", FormId());

			if (!serializer->ReadRecordData(&version, sizeof(version)))
				return FE("Array(%08X): Failed to read data version", FormId());

			for (UInt32 i = 0; i < len; i++)
			{
				T val;
				bool valueValid = false;
				if (!valueHandler.LoadValue(serializer, &val, valueValid))
				{
					return FW("Array(%08X): Failed to load value", FormId());
				}

				if (!valueValid)
					version++; // thin thing - collection is actually changed, so we have to change data version to be sure that iterators will fail to continue

				data.push_back(val);
			}
			return true;
		}

		void Validate()
		{
			BSReadLocker lock(&dataLock);
			for(auto it = data.begin(); it != data.end();)
			{
				if (Validator<T>::IsValid(*it))
					++it;
				else
				{
					it = data.erase(it);
					++version;
					W("Found invalid value in array %08X", FormId());
				}
			}
		}

		void GetState(std::vector<T>& keys, UInt64& curVersion)
		{
			BSReadLocker lock(&dataLock);
			keys.reserve(data.size());
			for(auto it = data.begin(); it != data.end();)
			{
				if (Validator<T>::IsValid(*it))
				{
					keys.emplace_back(*it);
					++it;
				}
				else
				{
					it = data.erase(it);
					++version;
					W("Found invalid value in array %08X", FormId());
				}
			}

			curVersion = version;
		}

		void DumpAll(int indent)
		{
			Dmp("%sArray (%08X). Version %d", Indent::Get(indent), FormId(), version);
			indent += 2;
			BSReadLocker lock(&dataLock);
			for (auto& e : data)
			{
				valueHandler.DumpValue(indent, e);
			}
		}

	protected:
		UInt64 version;
		std::vector<T> data;
		BSReadWriteLock dataLock;
		ValueHandler valueHandler;
	};

}
