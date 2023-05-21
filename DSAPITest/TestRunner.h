#pragma once

#include "f4se/PluginAPI.h"
namespace Test
{
	void Init(F4SEMessagingInterface* messaging, PluginHandle myPlugin);
	void ProcessMessage(F4SEMessagingInterface::Message* msg);
	void InitiateAccessorsInitialization();
	void InitiateTestsRun();
	void CancelTest();
};