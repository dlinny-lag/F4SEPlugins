#include "ArrayAPIExport.h"
#include "LoadManager.h"

namespace ArrayAPIExport
{
	IntArrayExport g_IntArrayExport("Int", &g_IntArrayExport);
	FloatArrayExport g_FloatArrayExport("Float", &g_FloatArrayExport);
	StringArrayExport g_StringArrayExport("String", &g_StringArrayExport);
	FormArrayExport g_FormArrayExport("Form", &g_FormArrayExport);

	StructArrayExport g_StructArrayExport("Struct", &g_StructArrayExport);

	void DumpAll()
	{
		g_IntArrayExport.DumpAll();
		g_FloatArrayExport.DumpAll();
		g_StringArrayExport.DumpAll();
		g_FormArrayExport.DumpAll();
		g_StructArrayExport.DumpAll();
	}
	void Clear()
	{
		g_IntArrayExport.Clear();
		g_FloatArrayExport.Clear();
		g_StringArrayExport.Clear();
		g_FormArrayExport.Clear();
		g_StructArrayExport.Clear();
	}

	bool Save(const F4SESerializationInterface* serializer)
	{
		D("Saving int arrays... %s", S(g_IntArrayExport.Save(serializer)));
		D("Saving float arrays... %s", S(g_FloatArrayExport.Save(serializer)));
		D("Saving string arrays... %s", S(g_StringArrayExport.Save(serializer)));
		D("Saving Form arrays... %s", S(g_FormArrayExport.Save(serializer)));
		D("Saving struct arrays... %s", S(g_StructArrayExport.Save(serializer)));
		return true;
	}

	void RegisterLoaders()
	{
		LoadManager::RegisterRecordLoader(g_IntArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_IntArrayExport.Load(serializer, version, length);});
		LoadManager::RegisterRecordLoader(g_FloatArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_FloatArrayExport.Load(serializer, version, length);});
		LoadManager::RegisterRecordLoader(g_StringArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_StringArrayExport.Load(serializer, version, length);});
		LoadManager::RegisterRecordLoader(g_FormArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_FormArrayExport.Load(serializer, version, length);});
		LoadManager::RegisterRecordLoader(g_StructArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_StructArrayExport.Load(serializer, version, length);});
	}

	bool Register(VirtualMachine* vm)
	{
		_MESSAGE("ArrayAPIExport: Register");

		g_IntArrayExport.Register(vm);
		g_FloatArrayExport.Register(vm);
		g_StringArrayExport.Register(vm);
		g_FormArrayExport.Register(vm);
		g_StructArrayExport.Register(vm);
		return true;
	}
};