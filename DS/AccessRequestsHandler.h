#pragma once
#include "CollectionAccessor.h"
#include "f4se/PluginAPI.h"

namespace DSAPI
{
	struct AccessorRequestProcessor
	{
		AccessorRequestProcessor(PluginHandle plugin, F4SEMessagingInterface* messaging)
			:plugin(plugin)
			,messaging(messaging)
		{
			
		}
		bool ProcessRequest(const AccessorRequest* request, const char* sender) const;
	private:
		PluginHandle plugin;
		F4SEMessagingInterface* messaging;
		template<typename Accessor> bool Dispatch(Accessor toSend, const char* receiver) const;
	};
}