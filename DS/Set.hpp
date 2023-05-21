#pragma once
#include "f4se/Serialization.h"
#include "f4se/GameForms.h"
#include "f4se/PluginAPI.h"
#include <unordered_set>
#include <tuple>
#include <functional>
#include <vector>
#include "MyTrace.h"
#include "Indent.h"
#include "Identifiable.h"
#include "ValueHelper.hpp"
#include "KeyValidator.hpp"

namespace DS
{
	template <typename T, typename SetType>
	class Set final : public Identifiable
	{
		UInt64 version;
		BSReadWriteLock dataLock;
		SetType data;

	public:
		Set(BGSKeyword* identifier) 
			:Identifiable(identifier)
			,version(0)
		{

		}

		UInt64 Version()
		{
			BSReadLocker lock(&dataLock);
			return version;
		}

		UInt32 Length()
		{
			BSReadLocker lock(&dataLock);
			return data.size();
		}
		bool Add(T& val)
		{
			if (!KeyValidator<T>::IsValid(val))
				return FD("Invalid element for set");
			BSWriteLocker lock(&dataLock);
			const bool inserted = data.insert(val).second;
			if (inserted)
				version++;
			return inserted;
		}

		UInt32 AddRange(const std::vector<T>& toAdd)
		{
			BSWriteLocker lock(&dataLock);
			UInt32 insertedCount = 0;
			for (auto& val : toAdd)
			{
				if (KeyValidator<T>::IsValid(val))
					insertedCount += static_cast<UInt32>(data.insert(val).second);
			}
			if (insertedCount)
				version++;

			return insertedCount;
		}
		UInt32 RemoveRange(std::vector<T>& toRemove)
		{
			BSWriteLocker lock(&dataLock);
			UInt32 removedCount = 0;
			for (auto& val : toRemove)
			{
				removedCount += data.erase(val);
			}
			if (removedCount)
				version++;
			return removedCount;
		}

		bool Intersect(std::unordered_set<T>& other)
		{
			bool changed = false;
			BSWriteLocker lock(&dataLock);
			std::vector<T> toRemove;
			for (auto& val : data)
			{
				if (other.find(val) == other.end())
				{ // if our element not found in other collection
				  // then remove it from our set
					toRemove.push_back(val);
				}
			}

			D("Intersect: removing %d elements", toRemove.size());
			for (auto& val : toRemove)
			{
				data.erase(val);
				changed = true;
			}
			if (changed)
				version++;

			return changed;
		}

		bool Contains(T& val)
		{
			BSReadLocker lock(&dataLock);
			return data.find(val) != data.end();
		}
		bool Remove(T& val)
		{
			BSWriteLocker lock(&dataLock);
			const bool removed = data.erase(val);
			if (removed)
				version++;
			return removed;
		}
		UInt32 Clear()
		{
			BSWriteLocker lock(&dataLock);
			const UInt32 size = data.size();
			if (size)
			{
				data.clear();
				version++;
			}
			return size;
		}

		bool SaveData(const F4SESerializationInterface* serializer, bool allowNoId)
		{
			Validate();
			const UInt32 len = data.size();
			if (!serializer->WriteRecordData(&len, sizeof(len)))
				return FE("Set(%08X): Failed to write length", FormId());

			if (!serializer->WriteRecordData(&version, sizeof(version)))
				return FE("Set(%08X): Failed to write data version", FormId());

			for (auto& val : data)
			{
				if (!ValueHelper::Save(serializer, &val))
					return FE("Set: Failed to save value (%08X)", FormId());
			}
			return true;
		}

		bool LoadData(const F4SESerializationInterface* serializer, bool allowNoId)
		{
			UInt32 len;

			if (!serializer->ReadRecordData(&len, sizeof(len)))
				return FE("Set(%08X): Failed to read length", FormId());

			if (!serializer->ReadRecordData(&version, sizeof(version)))
				return FE("Set(%08X): Failed to read data version", FormId());

			for (UInt32 i = 0; i < len; i++)
			{
				T val;
				if (!ValueHelper::Load(serializer, &val))
				{
					version++; // thin thing - collection is actually changed, so we have to change data version to be sure that iterators will fail to continue
					W("Set(%08X): Failed to load value", FormId());
					continue;
				}
				if (KeyValidator<T>::IsValid(val))
					data.insert(val);
				else
				{
					version++;
					W("Set(%08X): Invalid key read", FormId());
				}
			}

			return true;
		}

		void Validate()
		{
			BSReadLocker lock(&dataLock);
			std::vector<T> toRemove;
			for (auto& val : data)
			{
				if (!KeyValidator<T>::IsValid(val))
				{
					toRemove.emplace_back(val);
					++version;
					W("Invalid value found in Set %08X", FormId());
				}
			}
			for (const auto invalid : toRemove)
			{
				data.erase(invalid);
			}
		}

		void GetState(std::vector<T>& keys, UInt64& curVersion)
		{
			BSWriteLocker lock(&dataLock);
			std::vector<T> toRemove;
			for (auto& val : data)
			{
				if (KeyValidator<T>::IsValid(val))
					keys.push_back(val);
				else
				{
					toRemove.emplace_back(val);
					++version;
					W("Invalid value found in Set %08X", FormId());
				}
			}
			for (const auto invalid : toRemove)
			{
				data.erase(invalid);
			}
			curVersion = version;
		}

		void GetContent(SetType& outVal)
		{
			BSReadLocker lock(&dataLock);
			outVal = data;
		}

		void DumpAll(int indent)
		{
			Dmp("%sSet (%08X). Version %d", Indent::Get(indent), FormId(), version);
			indent += 2;
			BSReadLocker lock(&dataLock);
			for (auto& val : data)
			{
				ValueHelper::Dump(indent, val);
			}
		}
	};
}

