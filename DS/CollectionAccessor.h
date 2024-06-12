#pragma once
#include "TypeDefs.h"
#include "f4se/PapyrusValue.h"
#include "f4se/PluginAPI.h"
#include "transferable_vector.hpp"
#include <optional>
#include "CollectionTypeId.h"


namespace DSAPI
{
#pragma pack(push)
#pragma pack(4)
	struct Collection
	{
		UInt32 Version; // currently version is 1 for all types of collections
		UInt32 Identifier; // see CollectionTypeId.h
	};
#pragma pack(pop)

	template<typename ELEMENT_TYPE>
	struct BaseArray1 : Collection
	{
		enum {version = 1};
		bool (__cdecl *Delete)(BGSKeyword* identifier);
		UInt32 (__cdecl *Size)(BGSKeyword* identifier);
		bool (__cdecl *Add)(BGSKeyword*, ELEMENT_TYPE value);
		bool (__cdecl *AddRange)(BGSKeyword* identifier, transferable_vector<ELEMENT_TYPE>&& toAdd);
		std::optional<ELEMENT_TYPE> (__cdecl *Get)(BGSKeyword* identifier, SInt32 index);
		bool (__cdecl *Set)(BGSKeyword* identifier, SInt32 index, ELEMENT_TYPE value);
		bool (__cdecl *Insert)(BGSKeyword* identifier, ELEMENT_TYPE value, SInt32 index);
		bool (__cdecl *AppendArray)(BGSKeyword* identifier, BGSKeyword* toAdd);
		bool (__cdecl *Remove)(BGSKeyword* identifier, SInt32 index);
		UInt32 (__cdecl *Clear)(BGSKeyword* identifier);
		void (__cdecl *DumpAll)();
		void (__cdecl *Dump)(BGSKeyword* identifier);
	};
	

	template<typename ELEMENT_TYPE>
	struct PrimitiveArray1 : BaseArray1<ELEMENT_TYPE>
	{
		bool (__cdecl *Create)(BGSKeyword* identifier);
		SInt32 (__cdecl *IndexOf)(BGSKeyword* identifier, ELEMENT_TYPE value, SInt32 startIndex);
		bool (__cdecl *Sort)(BGSKeyword* identifier, UInt32 mode);
		std::optional<transferable_vector<ELEMENT_TYPE>> (__cdecl *ToArray)(BGSKeyword* identifier);
	};
	

	struct IntArray1 final : PrimitiveArray1<SInt32>{enum{type_id=IntArrayTypeId};};
	struct FloatArray1 final : PrimitiveArray1<float>{enum{type_id=FloatArrayTypeId};};
	struct StringArray1 final : PrimitiveArray1<BSFixedString>{enum{type_id=StringArrayTypeId};};
	struct FormArray1 final : PrimitiveArray1<FORM_TYPE>{enum{type_id=FormArrayTypeId};};

	struct StructArray1 final : BaseArray1<VMValue>
	{
		enum{type_id=StructArrayTypeId};
		bool (__cdecl *Create)(BGSKeyword* identifier, BSFixedString structName);
		SInt32 (__cdecl *IndexOf)(BGSKeyword* identifier, BSFixedString fieldName, VMValue value, SInt32 startIndex);
		bool (__cdecl *Sort)(BGSKeyword* identifier, BSFixedString fieldName, UInt32 mode);
	};


	template <typename ELEMENT_TYPE>
	struct Set1 : Collection
	{
		enum {version = 1};
		bool (__cdecl *Create)(BGSKeyword* identifier);
		bool (__cdecl *Delete)(BGSKeyword* identifier);
		SInt32 (__cdecl *Size)(BGSKeyword* identifier);
		bool (__cdecl *Add)(BGSKeyword* identifier, ELEMENT_TYPE value);
		SInt32 (__cdecl *AddRange)(BGSKeyword* identifier, transferable_vector<ELEMENT_TYPE> toAdd);
		SInt32 (__cdecl *RemoveRange)(BGSKeyword* identifier, transferable_vector<ELEMENT_TYPE> toRemove);
		bool (__cdecl *Intersect)(BGSKeyword* identifier, transferable_vector<ELEMENT_TYPE> other);
		bool (__cdecl *Contains)(BGSKeyword* identifier, ELEMENT_TYPE value);
		bool (__cdecl *Remove)(BGSKeyword* identifier, ELEMENT_TYPE value);
		bool (__cdecl *CopyToArray)(BGSKeyword* identifier, BGSKeyword* arrayId);
		SInt32 (__cdecl *AddArray)(BGSKeyword* identifier, BGSKeyword* arrayId);
		SInt32 (__cdecl *AddSet)(BGSKeyword* identifier, BGSKeyword* setId);
		bool (__cdecl *IntersectArray)(BGSKeyword* identifier, BGSKeyword* arrayId);
		bool (__cdecl *IntersectSet)(BGSKeyword* identifier, BGSKeyword* setId);
		SInt32 (__cdecl *RemoveArray)(BGSKeyword* identifier, BGSKeyword* arrayId);
		SInt32 (__cdecl *RemoveSet)(BGSKeyword* identifier, BGSKeyword* setId);
		UInt32 (__cdecl *Clear)(BGSKeyword* identifier);
		std::optional<transferable_vector<ELEMENT_TYPE>> (__cdecl *ToArray)(BGSKeyword* identifier);
		void (__cdecl *DumpAll)();
		void (__cdecl *Dump)(BGSKeyword* identifier);
	};
	
	struct IntSet1 final : Set1<SInt32>{enum{type_id=IntSetTypeId};};
	struct FloatSet1 final : Set1<float>{enum{type_id=FloatSetTypeId};};
	struct StringSet1 final : Set1<BSFixedString>{enum{type_id=StringSetTypeId};};
	struct FormSet1 final : Set1<FORM_TYPE>{enum{type_id=FormSetTypeId};};

	template <typename KEY_TYPE>
	struct BaseDict1 : Collection
	{
		enum {version = 1};
		bool (__cdecl *Delete)(BGSKeyword* identifier);
		SInt32 (__cdecl *Size)(BGSKeyword* identifier);
		bool (__cdecl *Contains)(BGSKeyword* identifier, KEY_TYPE key);
		bool (__cdecl *Remove)(BGSKeyword* identifier, KEY_TYPE key);
		void (__cdecl *Dump)(BGSKeyword* identifier);
		void (__cdecl *DumpAll)();
		bool (__cdecl *CopyKeysToArray)(BGSKeyword* identifier, BGSKeyword* arrayId);
		bool (__cdecl *CopyKeysToSet)(BGSKeyword* identifier, BGSKeyword* setId);
		SInt32 (__cdecl *RemoveKeysByArray)(BGSKeyword* identifier, BGSKeyword* arrayId);
		SInt32 (__cdecl *RemoveKeysBySet)(BGSKeyword* identifier, BGSKeyword* setId);
		std::optional<transferable_vector<KEY_TYPE>> (__cdecl *KeysToArray)(BGSKeyword* identifier);
	};

	template<typename KEY_TYPE, typename VALUE_TYPE>
	struct BaseValueDict1 : BaseDict1<KEY_TYPE>
	{
		bool (__cdecl *Add)(BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE value);
		bool (__cdecl *Set)(BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE value);
		std::optional<VALUE_TYPE> (__cdecl *Get)(BGSKeyword* identifier, KEY_TYPE key);
		bool (__cdecl *Replace)(BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE newValue);
	};

	template <typename KEY_TYPE, typename VALUE_TYPE>
	struct PrimitiveValueDict1 : BaseValueDict1<KEY_TYPE, VALUE_TYPE>
	{
		bool (__cdecl *Create)(BGSKeyword* identifier);
	};

	template <typename KEY_TYPE>
	struct StructValueDict1 : BaseValueDict1<KEY_TYPE, VMValue>
	{
		bool (__cdecl *Create)(BGSKeyword* identifier, BSFixedString structName);
	};

	template<typename KEY_TYPE, typename VALUE_TYPE>
	struct BaseArrayDict1 : BaseDict1<KEY_TYPE>
	{
		std::optional<VALUE_TYPE> (__cdecl *GetElement)(BGSKeyword* identifier, KEY_TYPE key, UInt32 index);
		bool (__cdecl *AddElement)(BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE value);
		bool (__cdecl *AddRange)(BGSKeyword* identifier, KEY_TYPE key, transferable_vector<VALUE_TYPE> toAdd);
		bool (__cdecl *SetElement)(BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE value, SInt32 index);
		bool (__cdecl *InsertElement)(BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE value, SInt32 index);
		bool (__cdecl *RemoveAtIndex)(BGSKeyword* identifier, KEY_TYPE key, SInt32 index);
		SInt32 (__cdecl *ArrayLength)(BGSKeyword* identifier, KEY_TYPE key);
		bool (__cdecl *AppendArray)(BGSKeyword* identifier, KEY_TYPE key, BGSKeyword* arrayId);
		bool (__cdecl *CopyToArray)(BGSKeyword* identifier, KEY_TYPE key, BGSKeyword* targetArrayId);
	};

	template<typename KEY_TYPE, typename VALUE_TYPE>
	struct PrimitiveArrayDict1 : BaseArrayDict1<KEY_TYPE, VALUE_TYPE>
	{
		bool (__cdecl *Create)(BGSKeyword* identifier);
		SInt32 (__cdecl *IndexOf)(BGSKeyword* identifier, KEY_TYPE key, VALUE_TYPE value, SInt32 startIndex);
		bool (__cdecl *SortArray)(BGSKeyword* identifier, KEY_TYPE key, UInt32 mode);
	};

	template<typename KEY_TYPE>
	struct StructArrayDict1 : BaseArrayDict1<KEY_TYPE, VMValue>
	{
		bool (__cdecl *Create)(BGSKeyword* identifier, BSFixedString structName);
		SInt32 (__cdecl *IndexOf)(BGSKeyword* identifier, KEY_TYPE key, BSFixedString fieldName, VMValue value, SInt32 startIndex);
		bool (__cdecl *SortArray)(BGSKeyword* identifier, KEY_TYPE key, BSFixedString fieldName, UInt32 mode);
	};

	struct IntDictInt1 final : PrimitiveValueDict1<SInt32, SInt32> {enum{type_id=IntDictIntTypeId};};
	struct IntDictFloat1 final : PrimitiveValueDict1<SInt32, float> {enum{type_id=IntDictFloatTypeId};};
	struct IntDictString1 final : PrimitiveValueDict1<SInt32, BSFixedString> {enum{type_id=IntDictStringTypeId};};
	struct IntDictForm1 final : PrimitiveValueDict1<SInt32, FORM_TYPE> {enum{type_id=IntDictFormTypeId};};
	struct IntDictStruct1 final : StructValueDict1<SInt32> {enum{type_id=IntDictStructTypeId};};

	struct StringDictInt1 final : PrimitiveValueDict1<BSFixedString, SInt32> {enum{type_id=StringDictIntTypeId};};
	struct StringDictFloat1 final : PrimitiveValueDict1<BSFixedString, float> {enum{type_id=StringDictFloatTypeId};};
	struct StringDictString1 final : PrimitiveValueDict1<BSFixedString, BSFixedString> {enum{type_id=StringDictStringTypeId};};
	struct StringDictForm1 final : PrimitiveValueDict1<BSFixedString, FORM_TYPE> {enum{type_id=StringDictFormTypeId};};
	struct StringDictStruct1 final : StructValueDict1<BSFixedString> {enum{type_id=StringDictStructTypeId};};

	struct FormDictInt1 final : PrimitiveValueDict1<FORM_TYPE, SInt32> {enum{type_id=FormDictIntTypeId};};
	struct FormDictFloat1 final : PrimitiveValueDict1<FORM_TYPE, float> {enum{type_id=FormDictFloatTypeId};};
	struct FormDictString1 final : PrimitiveValueDict1<FORM_TYPE, BSFixedString> {enum{type_id=FormDictStringTypeId};};
	struct FormDictForm1 final : PrimitiveValueDict1<FORM_TYPE, FORM_TYPE> {enum{type_id=FormDictFormTypeId};};
	struct FormDictStruct1 final : StructValueDict1<FORM_TYPE> {enum{type_id=FormDictStructTypeId};};

	struct IntDictIntArray1 final : PrimitiveArrayDict1<SInt32, SInt32> {enum{type_id=IntDictIntArrayTypeId};};
	struct IntDictFloatArray1 final : PrimitiveArrayDict1<SInt32, float> {enum{type_id=IntDictFloatArrayTypeId};};
	struct IntDictStringArray1 final : PrimitiveArrayDict1<SInt32, BSFixedString> {enum{type_id=IntDictStringArrayTypeId};};
	struct IntDictFormArray1 final : PrimitiveArrayDict1<SInt32, FORM_TYPE> {enum{type_id=IntDictFormArrayTypeId};};
	struct IntDictStructArray1 final : StructArrayDict1<SInt32> {enum{type_id=IntDictStructArrayTypeId};};

	struct StringDictIntArray1 final : PrimitiveArrayDict1<BSFixedString, SInt32> {enum{type_id=StringDictIntArrayTypeId};};
	struct StringDictFloatArray1 final : PrimitiveArrayDict1<BSFixedString, float> {enum{type_id=StringDictFloatArrayTypeId};};
	struct StringDictStringArray1 final : PrimitiveArrayDict1<BSFixedString, BSFixedString> {enum{type_id=StringDictStringArrayTypeId};};
	struct StringDictFormArray1 final : PrimitiveArrayDict1<BSFixedString, FORM_TYPE> {enum{type_id=StringDictFormArrayTypeId};};
	struct StringDictStructArray1 final : StructArrayDict1<BSFixedString> {enum{type_id=StringDictStructArrayTypeId};};

	struct FormDictIntArray1 final : PrimitiveArrayDict1<FORM_TYPE, SInt32> {enum{type_id=FormDictIntArrayTypeId};};
	struct FormDictFloatArray1 final : PrimitiveArrayDict1<FORM_TYPE, float> {enum{type_id=FormDictFloatArrayTypeId};};
	struct FormDictStringArray1 final : PrimitiveArrayDict1<FORM_TYPE, BSFixedString> {enum{type_id=FormDictStringArrayTypeId};};
	struct FormDictFormArray1 final : PrimitiveArrayDict1<FORM_TYPE, FORM_TYPE> {enum{type_id=FormDictFormArrayTypeId};};
	struct FormDictStructArray1 final : StructArrayDict1<FORM_TYPE> {enum{type_id=FormDictStructArrayTypeId};};

	enum AccessorType
	{
		None = 0, // invalid

		IntArray = IntArrayTypeId,
		FloatArray = FloatArrayTypeId,
		StringArray = StringArrayTypeId,
		FormArray = FormArrayTypeId,
		StructArray = StructArrayTypeId,

		IntSet = IntSetTypeId,
		FloatSet = FloatSetTypeId,
		StringSet = StringSetTypeId,
		FormSet = FormSetTypeId,

		IntDictInt = IntDictIntTypeId,
		IntDictFloat = IntDictFloatTypeId,
		IntDictString = IntDictStringTypeId,
		IntDictForm = IntDictFormTypeId,
		IntDictStruct = IntDictStructTypeId,

		StringDictInt = StringDictIntTypeId,
		StringDictFloat = StringDictFloatTypeId,
		StringDictString = StringDictStringTypeId,
		StringDictForm = StringDictFormTypeId,
		StringDictStruct = StringDictStructTypeId,

		FormDictInt = FormDictIntTypeId,
		FormDictFloat = FormDictFloatTypeId,
		FormDictString = FormDictStringTypeId,
		FormDictForm = FormDictFormTypeId,
		FormDictStruct = FormDictStructTypeId,

		IntDictIntArray = IntDictIntArrayTypeId,
		IntDictFloatArray = IntDictFloatArrayTypeId,
		IntDictStringArray = IntDictStringArrayTypeId,
		IntDictFormArray = IntDictFormArrayTypeId,
		IntDictStructArray = IntDictStructArrayTypeId,

		StringDictIntArray = StringDictIntArrayTypeId,
		StringDictFloatArray = StringDictFloatArrayTypeId,
		StringDictStringArray = StringDictStringArrayTypeId,
		StringDictFormArray = StringDictFormArrayTypeId,
		StringDictStructArray = StringDictStructArrayTypeId,

		FormDictIntArray = FormDictIntArrayTypeId,
		FormDictFloatArray = FormDictFloatArrayTypeId,
		FormDictStringArray = FormDictStringArrayTypeId,
		FormDictFormArray = FormDictFormArrayTypeId,
		FormDictStructArray = FormDictStructArrayTypeId,
	};

#pragma pack (push)
#pragma pack(4)
	struct AccessorRequest
	{
		static inline constexpr int MessageType = 'DSAR';
		AccessorType Type;
		int Version; // version of accessor object. currently = 1
	};
#pragma pack(pop)

	constexpr inline int AccessorMessage = 'DSAM';
	// Call this method to receive accessor requests response. Method must be called after DS plugin loading. See 'Messaging API docs' in f4se/PluginAPI.h
	// handler will be called with AccessorMessage message type
	bool inline RegisterForAccessorsReceiving(const F4SEMessagingInterface* messaging, PluginHandle plugin, F4SEMessagingInterface::EventCallback handler)
	{
		return messaging->RegisterListener(plugin, DS_PluginName, handler);
	}
	// appropriate object will be sent to the listener registered with RegisterForAccessorsReceiving
	bool inline RequestAccessor(const F4SEMessagingInterface* messaging, PluginHandle sender, AccessorType type, int version = 1)
	{
		AccessorRequest request{type, version};
		return messaging->Dispatch(sender, AccessorRequest::MessageType, &request, sizeof(request), DS_PluginName);
	}

	template <typename Accessor>
	bool IsAccessorValid(const Accessor& accessor)
	{
		static_assert(std::is_base_of_v<Collection, Accessor>);
		static_assert(std::is_final_v<Accessor>);
		return Accessor::type_id == accessor.Identifier && Accessor::version == accessor.Version;
	}

	template<typename Accessor>
	std::optional<Accessor> AsAccessor(F4SEMessagingInterface::Message* msg)
	{
		if (msg->type != AccessorMessage)
			return std::nullopt; // wrong message
		if (strncmp(msg->sender, DS_PluginName, 2) != 0)
			return std::nullopt; // wrong sender
		if (msg->dataLen != sizeof(Accessor))
			return std::nullopt; // data type mismatch
		Accessor value = *static_cast<Accessor*>(msg->data);
		if (!IsAccessorValid(value))
			return std::nullopt; // wrong content
		return value;
	}

	inline Collection GetAccessorInfo(F4SEMessagingInterface::Message* msg)
	{
		if (msg->type != AccessorMessage)
			return {0,AccessorType::None}; // wrong message
		if (strncmp(msg->sender, DS_PluginName, 2) != 0)
			return {0,AccessorType::None}; // wrong sender
		if (msg->dataLen < sizeof(Collection))
			return  {0,AccessorType::None};
		return *static_cast<Collection*>(msg->data);
	}

	namespace verification
	{
		// ensure binary compatibility of passing arguments
		static_assert(sizeof(std::optional<transferable_vector<SInt32>>) == sizeof(std::optional<transferable_vector<float>>));
		static_assert(sizeof(std::optional<transferable_vector<SInt32>>) == sizeof(std::optional<transferable_vector<BSFixedString>>));
		static_assert(sizeof(std::optional<transferable_vector<SInt32>>) == sizeof(std::optional<transferable_vector<FORM_TYPE>>));
		static_assert(sizeof(std::optional<transferable_vector<SInt32>>) == sizeof(std::optional<transferable_vector<VMValue>>));
		static_assert(sizeof(std::optional<transferable_vector<SInt32>>) == sizeof(transferable_vector<SInt32>) + 8);

		static_assert(sizeof(Collection) == sizeof(void*));
		
		inline constexpr int CollectionSize = 1;
		inline constexpr int BaseArraySize1 = 12;
		inline constexpr int PrimitiveArraySize1 = 4;
		inline constexpr int StructArraySize1 = 3;

		inline constexpr int SetSize1 = 20;

		inline constexpr int BaseDictSize1 = 11;

		inline constexpr int BaseValueDictSize1 = 4;
		inline constexpr int PrimitiveValueDictSize1 = 1;
		inline constexpr int StructValueDictSize1 = 1;

		inline constexpr int BaseArrayDictSize1 = 9;
		inline constexpr int PrimitiveArrayDictSize1 = 3;
		inline constexpr int StructArrayDictSize1 = 3;
		

		static_assert(sizeof(IntArray1) == (CollectionSize + BaseArraySize1 + PrimitiveArraySize1)*sizeof(void*));
		static_assert(sizeof(FloatArray1) == (CollectionSize + BaseArraySize1 + PrimitiveArraySize1)*sizeof(void*));
		static_assert(sizeof(StringArray1) == (CollectionSize + BaseArraySize1 + PrimitiveArraySize1)*sizeof(void*));
		static_assert(sizeof(FormArray1) == (CollectionSize + BaseArraySize1 + PrimitiveArraySize1)*sizeof(void*));
		static_assert(sizeof(StructArray1) == (CollectionSize + BaseArraySize1 + StructArraySize1)*sizeof(void*));

		static_assert(offsetof(IntArray1,Version) == 0);
		static_assert(offsetof(IntArray1,Identifier) == 4);

		static_assert(sizeof(IntSet1) == (CollectionSize + SetSize1)*sizeof(void*));
		static_assert(sizeof(FloatSet1) == (CollectionSize + SetSize1)*sizeof(void*));
		static_assert(sizeof(StringSet1) == (CollectionSize + SetSize1)*sizeof(void*));
		static_assert(sizeof(FormSet1) == (CollectionSize + SetSize1)*sizeof(void*));

		static_assert(sizeof(IntDictInt1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + PrimitiveValueDictSize1)*(sizeof(void*)));
		static_assert(sizeof(IntDictFloat1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + PrimitiveValueDictSize1)*(sizeof(void*)));
		static_assert(sizeof(IntDictString1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + PrimitiveValueDictSize1)*(sizeof(void*)));
		static_assert(sizeof(IntDictForm1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + PrimitiveValueDictSize1)*(sizeof(void*)));
		static_assert(sizeof(IntDictStruct1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + StructValueDictSize1)*(sizeof(void*)));

		static_assert(sizeof(StringDictInt1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + PrimitiveValueDictSize1)*(sizeof(void*)));
		static_assert(sizeof(StringDictFloat1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + PrimitiveValueDictSize1)*(sizeof(void*)));
		static_assert(sizeof(StringDictString1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + PrimitiveValueDictSize1)*(sizeof(void*)));
		static_assert(sizeof(StringDictForm1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + PrimitiveValueDictSize1)*(sizeof(void*)));
		static_assert(sizeof(StringDictStruct1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + StructValueDictSize1)*(sizeof(void*)));

		static_assert(sizeof(FormDictInt1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + PrimitiveValueDictSize1)*(sizeof(void*)));
		static_assert(sizeof(FormDictFloat1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + PrimitiveValueDictSize1)*(sizeof(void*)));
		static_assert(sizeof(FormDictString1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + PrimitiveValueDictSize1)*(sizeof(void*)));
		static_assert(sizeof(FormDictForm1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + PrimitiveValueDictSize1)*(sizeof(void*)));
		static_assert(sizeof(FormDictStruct1) == (CollectionSize + BaseDictSize1 + BaseValueDictSize1 + StructValueDictSize1)*(sizeof(void*)));

		static_assert(sizeof(IntDictIntArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + PrimitiveArrayDictSize1)*(sizeof(void*)));
		static_assert(sizeof(IntDictFloatArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + PrimitiveArrayDictSize1)*(sizeof(void*)));
		static_assert(sizeof(IntDictStringArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + PrimitiveArrayDictSize1)*(sizeof(void*)));
		static_assert(sizeof(IntDictFormArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + PrimitiveArrayDictSize1)*(sizeof(void*)));
		static_assert(sizeof(IntDictStructArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + StructArrayDictSize1)*(sizeof(void*)));

		static_assert(sizeof(StringDictIntArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + PrimitiveArrayDictSize1)*(sizeof(void*)));
		static_assert(sizeof(StringDictFloatArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + PrimitiveArrayDictSize1)*(sizeof(void*)));
		static_assert(sizeof(StringDictStringArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + PrimitiveArrayDictSize1)*(sizeof(void*)));
		static_assert(sizeof(StringDictFormArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + PrimitiveArrayDictSize1)*(sizeof(void*)));
		static_assert(sizeof(StringDictStructArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + StructArrayDictSize1)*(sizeof(void*)));

		static_assert(sizeof(FormDictIntArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + PrimitiveArrayDictSize1)*(sizeof(void*)));
		static_assert(sizeof(FormDictFloatArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + PrimitiveArrayDictSize1)*(sizeof(void*)));
		static_assert(sizeof(FormDictStringArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + PrimitiveArrayDictSize1)*(sizeof(void*)));
		static_assert(sizeof(FormDictFormArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + PrimitiveArrayDictSize1)*(sizeof(void*)));
		static_assert(sizeof(FormDictStructArray1) == (CollectionSize + BaseDictSize1 + BaseArrayDictSize1 + StructArrayDictSize1)*(sizeof(void*)));

		static_assert(sizeof(AccessorRequest) == 8);
	};
};