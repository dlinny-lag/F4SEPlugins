#include "Dispatcher.h"
#include "f4se/GameThreads.h"
#include "f4se/PluginManager.h"
F4SETaskInterface* taskManager = nullptr;
F4SEMessagingInterface* messagingManager;
PluginHandle sender;

// AAI2
constexpr UInt32 AttributeIncrementMessageType = Notifications::AttributeIncrementMessageType2;
// AAD2
constexpr UInt32 AttributeDecrementMessageType = Notifications::AttributeDecrementMessageType2;

template<UInt32 ChangeType>
struct NotificationTask : public ITaskDelegate
{
	Notifications::Change Change;
	NotificationTask(Notifications::Change&& change)
		:Change(std::move(change))
	{
		
	}
	void Run() override
	{
		messagingManager->Dispatch(sender, ChangeType, &Change, sizeof(Change), nullptr);
	}
};

void Notifications::Dispatcher::Init(PluginHandle handle, F4SEMessagingInterface* messaging, F4SETaskInterface* tasks)
{
	sender = handle;
	taskManager = tasks;
	messagingManager = messaging;
}

void Notifications::Dispatcher::DispatchIncrease(Change&& change)
{
	taskManager->AddTask(new NotificationTask<AttributeIncrementMessageType>(std::move(change)));
}
void Notifications::Dispatcher::DispatchDecrease(Change&& change)
{
	taskManager->AddTask(new NotificationTask<AttributeDecrementMessageType>(std::move(change)));
}