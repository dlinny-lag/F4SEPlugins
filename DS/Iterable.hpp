#pragma once
#include "MyTrace.h"
#include "IteratorsSet.hpp"

namespace DS
{
	template <typename ELEMENT_TYPE, typename Collection, typename CollectionsSet, typename Iterator, typename Pointer>
	class Iterable
	{
	private:
		CollectionsSet* set;
		std::function<UInt32(Collection&)> createIterator;
	protected:
		IteratorsSet<ELEMENT_TYPE, DS::Iterator<ELEMENT_TYPE>> iterators;
	public:
		Iterable(CollectionsSet* set, UInt32 collectionTypeId)
			:set(set)
			,createIterator([this](auto collection){return iterators.CreateIterator<Collection>(collection);})
			,iterators(collectionTypeId)
		{
			
		}
		virtual ~Iterable() = default;

		Iterator CreateIterator(BGSKeyword* identifier)
		{
			Iterator retVal;
			UInt32 iteratorId = DS::Iterator<ELEMENT_TYPE>::InvalidID;
			if (!set->ExecuteOn<UInt32>(identifier, createIterator, iteratorId))
				D("Failed to create iterator");

			retVal.Set("IteratorId", iteratorId);
			retVal.Set("CollectionId", identifier);
			return retVal;
		}

		bool DeleteIterator(Iterator iterator)
		{
			if (iterator.IsNone())
				return false;
			UInt32 iteratorId = DS::Iterator<ELEMENT_TYPE>::InvalidID;
			BGSKeyword* collectionId = nullptr;
			iterator.Get("CollectionId", &collectionId);
			iterator.Get("IteratorId", &iteratorId);
			if (!collectionId)
				return false;
			return iterators.RemoveIterator(collectionId->formID, iteratorId);
		}

		Pointer Next(Iterator iterator)
		{
			UInt32 iteratorId = DS::Iterator<ELEMENT_TYPE>::InvalidID;
			BGSKeyword* collectionId = nullptr;
			bool argValid = false;
			if (!iterator.IsNone())
			{
				iterator.Get("CollectionId", &collectionId);
				iterator.Get("IteratorId", &iteratorId);
				argValid = collectionId != nullptr;
			}
			else
			{
				D("Next: invalid iterator");
			}
			ELEMENT_TYPE val;
			bool isFinished = false;
			bool isValid = false;
			if (argValid)
			{
				bool needValidation = false;
				isValid = iterators.Next(collectionId->formID, iteratorId, val, isFinished, needValidation);
				if (!isValid)
				{
					val = DefaultValue<ELEMENT_TYPE>::Get();
					D("Next() returned false on iterator %d of collection %08X", iteratorId, collectionId->formID);
				}
				if (needValidation)
					set->Validate(collectionId);
			}
			else
			{
				val = DefaultValue<ELEMENT_TYPE>::Get();
			}
			Pointer retVal;
			retVal.Set<UInt32>("IteratorId", iteratorId);
			retVal.Set<BGSKeyword*>("CollectionId", collectionId);
			retVal.Set<bool>("Valid", isValid);
			retVal.Set<ELEMENT_TYPE>("Value", val);
			retVal.Set<bool>("Finished", isFinished);

			if (isFinished && collectionId)
				iterators.RemoveIterator(collectionId->formID, iteratorId);

			return retVal;
		}
	};
};