﻿#include "SetAPIExport.h"

#include "LoadManager.h"


namespace SetAPIExport
{
	IntSetExport g_IntSetExport("Int", &g_IntSetExport, &ArrayAPIExport::g_IntArrayExport);
	FloatSetExport g_FloatSetExport("Float", &g_FloatSetExport, &ArrayAPIExport::g_FloatArrayExport);
	StringSetExport g_StringSetExport("String", &g_StringSetExport, &ArrayAPIExport::g_StringArrayExport);
	FormSetExport g_FormSetExport("Form", &g_FormSetExport, &ArrayAPIExport::g_FormArrayExport);

	void DumpAll()
	{
		g_IntSetExport.DumpAll();
		g_FloatSetExport.DumpAll();
		g_StringSetExport.DumpAll();
		g_FormSetExport.DumpAll();
	}

	void Clear()
	{
		g_IntSetExport.Clear();
		g_FloatSetExport.Clear();
		g_StringSetExport.Clear();
		g_FormSetExport.Clear();
	}

	bool Save(const F4SESerializationInterface* serializer)
	{
		g_IntSetExport.Save(serializer);
		g_FloatSetExport.Save(serializer);
		g_StringSetExport.Save(serializer);
		g_FormSetExport.Save(serializer);

		return true;
	}

	void RegisterLoaders()
	{
		LoadManager::RegisterRecordLoader(&g_IntSetExport);
		LoadManager::RegisterRecordLoader(&g_FloatSetExport);
		LoadManager::RegisterRecordLoader(&g_StringSetExport);
		LoadManager::RegisterRecordLoader(&g_FormSetExport);
	}
	
	bool Register(VirtualMachine* vm)
	{
		_MESSAGE("SetAPIExport: Register");

		g_IntSetExport.Register(vm);
		g_FloatSetExport.Register(vm);
		g_StringSetExport.Register(vm);
		g_FormSetExport.Register(vm);

		return true;
	}
}
