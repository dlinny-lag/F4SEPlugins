#pragma once
#include <f4se/PluginAPI.h>

#include "Change.hpp"

namespace Notifications
{
	typedef Change2 Change;
	struct Dispatcher
	{
		static void Init(PluginHandle handle, F4SEMessagingInterface* messaging, F4SETaskInterface* tasks);
		static void DispatchIncrease(Change&& change);
		static void DispatchDecrease(Change&& change);
	};

}
