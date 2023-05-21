#pragma once

#include "f4se/Serialization.h"
#include "f4se/GameForms.h"
#include "f4se/PluginAPI.h"
#include <unordered_map>
#include "MyTrace.h"

template <typename T>
class FormsSet
{
	std::unordered_map<UInt32, T*> forms;

public:

	bool Save(const F4SESerializationInterface* serializer) const
	{
		UInt32 size = forms.size();
		if (!serializer->WriteRecordData(&size, sizeof(size)))
			return FE("Failed to write size of forms");

		for (auto& form : forms)
		{
			UInt32 id = form.first;
			if (!serializer->WriteRecordData(&id, sizeof(id)))
				return FE("Failed to write id=%08X of some form");;
		}

		return true;
	}

	bool Load(const F4SESerializationInterface* serializer, UInt32& lost)
	{
		UInt32 size = 0;
		if (!Serialization::ReadData(serializer, &size))
		{
			return FE("Error reading Forms list size");
		}

		for (UInt32 i = 0; i < size; i++)
		{
			UInt32 savedId = 0;
			UInt32 actualId = 0;
			if (!serializer->ReadRecordData(&savedId, sizeof(savedId)))
			{
				return FE("Error reading FormId");
			}

			if (!serializer->ResolveFormId(savedId, &actualId))
			{
				W("Form %08X not resolved ", savedId);
				lost++;
				continue; // skip if Form is missing
			}
			TESForm* f = LookupFormByID(actualId);
			if (!f)
			{
				W("Form %08X not found ", actualId);
				lost++;
				continue; // ignore missing form
			}
			T* form = dynamic_cast<T*>(f);
			if (form)
			{
				forms.insert(std::make_pair(actualId, form)); // insert directly
			}
			else 
			{ // ignore invalid Form... not sure how it could happens
				lost++;
				W("Invalid Form with saved id=%08X and resolved id=%08X", savedId, actualId);
			}
		}

		return true;
	}

	bool Add(T* form)
	{
		if (form)
		{
			return forms.insert(std::make_pair(form->formID, form)).second;
		}
		return FD("Attempt to add null form");
	}
	bool Remove(T* form)
	{
		if (form)
		{
			return forms.erase(form->formID) > 0;
		}
		return FD("Attempt to remove null form");
	}
	bool Contains(UInt32 id) const
	{
		return forms.find(id) != forms.end();
	}

	bool Contains(T* form) const
	{
		return form && forms.find(form->formID) != forms.end();
	}

	UInt32 Length()
	{
		return forms.size();
	}

	bool IsEmpty() const
	{
		return forms.size() == 0;
	}

	void Clear()
	{
		forms.clear();
	}

	void ForEach(std::function<void(const T*)> functor)
	{
		for (auto form : forms)
		{
			functor(form.second);
		}
	}

	bool ForEach(std::function<bool(const T*)> functor)
	{
		for (auto form : forms)
		{
			if (!functor(form.second))
				return false;
		}
		return true;
	}

	static void PrintForm(const T* form)
	{
		Dmp("    %08X", form ? form->formID : 0);
	}
};

