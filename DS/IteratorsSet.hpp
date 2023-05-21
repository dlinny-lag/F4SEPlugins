#pragma once
#include <unordered_map>
#include "Iterator.hpp"


template <typename T, typename IteratorType>
class IteratorsSet
{
	// main key - collection's keyword form id
	// second key - id of iterator
	std::unordered_map<UInt32, std::unordered_map<UInt32, IteratorType>> iterators;
	BSReadWriteLock iteratorsLock;
	UInt32 lastIteratorId = 0; 
	UInt32 typeId;
public:
	IteratorsSet(UInt32 typeId)
		:typeId(typeId)
	{

	}
	template <typename CollectionType>
	UInt32 CreateIterator(CollectionType& collection)
	{
		BSWriteLocker lock(&iteratorsLock);
		++lastIteratorId;
		if (lastIteratorId > 0x7FFFFFFEL)
			lastIteratorId = 1L;

		IteratorType newIterator(collection.Id(), typeId);
		newIterator.Init(collection, lastIteratorId);
		const UInt32 collectionFormId = collection.FormId();

		const auto collectionIterators = iterators.find(collectionFormId);
		if (collectionIterators == iterators.end())
		{
			std::unordered_map<UInt32, IteratorType> collIters;
			collIters.insert(std::make_pair(lastIteratorId, newIterator));
			iterators.insert(std::make_pair(collectionFormId, collIters));
			return lastIteratorId;
		}

		collectionIterators->second.insert(std::make_pair(lastIteratorId, newIterator));
		return lastIteratorId;
	}

	bool RemoveIterator(UInt32 collectionFormId, UInt32 iteratorId)
	{
		BSWriteLocker lock(&iteratorsLock);
		const auto collectionIterators = iterators.find(collectionFormId);
		if (collectionIterators != iterators.end())
		{
			return collectionIterators->second.erase(iteratorId) > 0;
		}
		return false;
	}

	void RemoveAllIterators(UInt32 collectionFormId)
	{
		BSWriteLocker lock(&iteratorsLock);
		iterators.erase(collectionFormId);
	}

	// return true if value is available
	bool Next(UInt32 collectionFormId, UInt32 iteratorId, T& outVal, bool& isFinished, bool& needValidation)
	{
		needValidation = false;
		BSWriteLocker lock(&iteratorsLock);
		const auto collectionIterators = iterators.find(collectionFormId);
		if (collectionIterators == iterators.end())
		{
			D("Next: collection %08X not found", collectionFormId);
			return false;
		}
		const auto iter = collectionIterators->second.find(iteratorId);
		if (iter == collectionIterators->second.end())
		{
			D("Next: iterator %d of collection %08X not found", iteratorId, collectionFormId);
			return false;
		}
		if (iter->second.IsInvalid())
		{
			D("Next: iterator %d of collection %08X is invalid", iteratorId, collectionFormId);
			isFinished = true;
			return false;
		}
		
		iter->second.Next();

		if (!iter->second.Available())
		{ 
			D("Next: iterator %d of collection %08X is unavailable", iteratorId, collectionFormId);
			iter->second.Dump(2);
			isFinished = true;
			return false;
		}
		D("Next: iterator %d of collection %08X moved to %d", iteratorId, collectionFormId, iter->second.Position());
		auto cur = iter->second.Get();
		if (!KeyValidator<T>::IsValid(cur))
		{
			W("Found invalid value in collection %08X", collectionFormId);
			iterators.erase(collectionFormId);
			isFinished = true;
			needValidation = true;
			outVal = {};
			return false;
		}
		outVal = cur;
		isFinished = iter->second.IsFinished();
		return true;
	}

	void Clear()
	{
		BSWriteLocker lock(&iteratorsLock);
		iterators.clear();
	}

	bool Save(const F4SESerializationInterface* serializer) const
	{
		const UInt32 size = iterators.size();
		if (!serializer->WriteRecordData(&size, sizeof(size)))
			return FE("Failed to write collections size for iterators");

		for (const auto& colIters : iterators)
		{
			const UInt32 collectionFormId = colIters.first;
			if (!serializer->WriteRecordData(&collectionFormId, sizeof(collectionFormId)))
				return FE("Failed to write collection id for iterators");

			const UInt32 itersSize = colIters.second.size();
			if (!serializer->WriteRecordData(&itersSize, sizeof(itersSize)))
				return FE("Failed to write iterators size");
			for (const auto& iter : colIters.second)
			{
				if (!iter.second.Save(serializer))
				{
					const UInt32 iterId = iter.first;
					return FE("Failed to write iterator %d", iterId);
				}
			}
		}
		return true;
	}
	bool Load(const F4SESerializationInterface* serializer, const std::unordered_map<UInt32, UInt64>& collectionsVersions)
	{
		UInt32 size;
		if (!serializer->ReadRecordData(&size, sizeof(size)))
			return  FE("Failed to read collections size for iterators");
		for (UInt32 c = 0; c < size; c++)
		{
			UInt32 collectionFormId;
			if (!serializer->ReadRecordData(&collectionFormId, sizeof(collectionFormId)))
				return FE("Failed to read collection id for iterators");
			
			bool collectionValid = true;
			UInt32 actualCollectionFormId;
			if (!serializer->ResolveFormId(collectionFormId, &actualCollectionFormId))
			{
				collectionValid = false;
			}

			std::unordered_map<UInt32, IteratorType> colIters;
			UInt32 itersSize;
			if (!serializer->ReadRecordData(&itersSize, sizeof(itersSize)))
				return  FE("Failed to write iterators size");

			for (UInt32 i = 0; i < itersSize; i++)
			{
				IteratorType iterator(nullptr, typeId);
				if (!iterator.Load(serializer)) // TODO: collection identifier should be resolved once
				{
					W("Failed to load iterator");
					continue;
				}

				if (!collectionValid)
					continue;  // don't waste CPU if collection is invalid
				
				UInt64 version = DS::Iterator<nullptr_t>::InvalidVersion;
				const auto cvPtr = collectionsVersions.find(actualCollectionFormId);
				if (cvPtr != collectionsVersions.end())
				{
					version = cvPtr->second;
				}

				if (iterator.IsInvalid(version))
				{
					W("Read invalid iterator");
					continue;
				}

				if (iterator.IsFinished())
				{
					W("Iterator for collection %08X is finished already:", actualCollectionFormId);
					iterator.Dump(0);
					continue;
				}

				// insert valid iterator
				colIters.insert(std::make_pair(iterator.IteratorId(), iterator));
			}
			
			if (collectionValid)
			{
				if (!colIters.empty())
					iterators.insert(std::make_pair(actualCollectionFormId, colIters));
			}
		}
		return true;
	}

	void DumpAll(int indent)
	{
		BSReadLocker lock(&iteratorsLock);
		for (auto& colPair : iterators)
		{
			Dmp("%sIterators for collection %08X", Indent::Get(indent), colPair.first);
			for (auto& iterPair : colPair.second)
			{
				iterPair.second.Dump(indent + 2);
			}
		}
	}
	void Dump(UInt32 collectionFormId, int indent)
	{
		BSReadLocker lock(&iteratorsLock);
		const auto colPair = iterators.find(collectionFormId);
		if (colPair != iterators.end())
		{
			Dmp("%sIterators:", Indent::Get(indent));
			for (auto& iterPair : colPair->second)
			{
				iterPair.second.Dump(indent + 2);
			}
		}
	}
};