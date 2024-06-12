#pragma once
#include "f4se/GameForms.h"
#include "f4se/PluginAPI.h"
#include <vector>

#define AA_PluginName "AA"

#ifndef _DEBUG
// sizeof std::vector in debug build is not equal to sizeof in release build
// just don't use it in debug build
namespace Notifications // version 1
{ 
	// AAI1
	constexpr UInt32 AttributeIncrementMessageType1 = 'AAI1';
	// AAD1
	constexpr UInt32 AttributeDecrementMessageType1 = 'AAD1';
 	struct Change1 // deprecated and no longer used since 0.9.0
	{
		std::vector<TESForm*> senders; // NOTE: might contain nullptrs
		Actor* Actor;
		ActorValueInfo* Attribute;
		float PreviousValue;
		float NewValue;
		float Exceed;
	};
	template <int> struct SizeTest;
	template<> struct SizeTest<0x38>{};
	template struct SizeTest<sizeof(Change1)>;

#ifndef __AA
	static_assert(sizeof(Change1) == 0x38);
#endif
};
#endif

namespace Notifications // version 2
{ 
	// AAI2
	constexpr UInt32 AttributeIncrementMessageType2 = 'AAI2';
	// AAD2
	constexpr UInt32 AttributeDecrementMessageType2 = 'AAD2';
 	struct Change2
	{
		Actor* Actor;
		ActorValueInfo* Attribute;
		float PreviousValue;
		float NewValue;
		float Exceed;
		UInt32 SendersCount;
		std::unique_ptr<TESForm*> Senders; // NOTE: might contain nullptrs

		Change2(class Actor* actor, class ActorValueInfo* attr, float prevVal, float newVal, float exceed, const std::vector<TESForm*>& senders)
			: Actor(actor)
			, Attribute(attr)
			, PreviousValue(prevVal)
			, NewValue(newVal)
			, Exceed(exceed)
 			, SendersCount(static_cast<UInt32>(senders.size()))
			, Senders(new TESForm*[SendersCount])
		{
			TESForm** ptr = Senders.get();
			std::copy_n(senders.begin(), SendersCount, ptr);
		}

		Change2(Change2&& other) noexcept
		{
			Actor = other.Actor; 
			Attribute = other.Attribute; 
			PreviousValue = other.PreviousValue;
			NewValue = other.NewValue;
			Exceed = other.Exceed;
			SendersCount = other.SendersCount; 
			Senders = std::move(other.Senders);

			other.Actor = nullptr;
			other.Attribute = nullptr;
			other.SendersCount = 0;
		}

		Change2() = delete;
		Change2(Change2&) = delete;
		Change2(const Change2&) = delete;
		Change2(const Change2&&) = delete;
		~Change2() = default;
		Change2& operator=(const Change2&) = delete;
		Change2& operator=(Change2&&) = delete;
	};
	template <int> struct SizeTest2;
	template<> struct SizeTest2<40>{};
	template struct SizeTest2<sizeof(Change2)>;
	static_assert(sizeof(Change2) == 40);
}

namespace Notifications
{
	/// <summary>
	/// Call this method to receive changes notifications. Method must be called after AA plugin loading. See 'Messaging API docs' in f4se/PluginAPI.h
	/// </summary>
	/// <param name="messaging">F4SEMessagingInterface for your plugin. Use QueryInterface in F4SEPlugin_Query to obtain pointer</param>
	/// <param name="plugin">Your plugin handle. use F4SEInterface::GetPluginHandle to obtain it</param>
	/// <param name="handler">Changes notifications handler</param>
	bool inline SubscribeForChanges(const F4SEMessagingInterface* messaging, PluginHandle plugin, F4SEMessagingInterface::EventCallback handler)
	{
		return messaging->RegisterListener(plugin, AA_PluginName, handler);
	}

	template <class T, int version>
	const T* ChangeMessageAs(const F4SEMessagingInterface::Message* msg)
	{
		if (sizeof(T) != msg->dataLen)
		{
			_MESSAGE("Invalid message (version %d) size. Expected %d, but got %d", version, sizeof(T), msg->dataLen);
			return nullptr;
		}
		const T* change = static_cast<T*>(msg->data);
		return change;
	}

#ifndef _DEBUG
	inline const Change1* GetAsVersion1(const F4SEMessagingInterface::Message* msg)
	{
		return ChangeMessageAs<Change1, 1>(msg);
	}
#endif

	inline const Change2* GetAsVersion2(const F4SEMessagingInterface::Message* msg)
	{
		return ChangeMessageAs<Change2, 2>(msg);
	}
};

/*
void ExampleMessageCallback(F4SEMessagingInterface::Message* msg)
{
	if (msg->type == Notifications::AttributeIncrementMessageType2)
	{
		// some value increased
		const Notifications::Change2* change = Notifications::GetAsVersion2(msg)
		// change pointer will be deleted almost immediately after this function return
		// so you have to get a copy if you want to proceed it later
	}
	throw "Don't use example in production";
}
*/