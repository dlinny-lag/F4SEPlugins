#include "ValueDictAPIExport.h"

#include "LoadManager.h"


namespace ValueDictAPIExport
{
	IntDictIntExport g_IntDictIntExport("Int", "Int", &ArrayAPIExport::g_IntArrayExport, &SetAPIExport::g_IntSetExport, &g_IntDictIntExport);
	IntDictFloatExport g_IntDictFloatExport("Int", "Float", &ArrayAPIExport::g_IntArrayExport, &SetAPIExport::g_IntSetExport, &g_IntDictFloatExport);
	IntDictStringExport g_IntDictStringExport("Int", "String", &ArrayAPIExport::g_IntArrayExport, &SetAPIExport::g_IntSetExport, &g_IntDictStringExport);
	IntDictFormExport g_IntDictFormExport("Int", "Form", &ArrayAPIExport::g_IntArrayExport, &SetAPIExport::g_IntSetExport, &g_IntDictFormExport);
	IntDictStructExport g_IntDictStructExport("Int", &ArrayAPIExport::g_IntArrayExport, &SetAPIExport::g_IntSetExport, &g_IntDictStructExport);

	StringDictIntExport g_StringDictIntExport("String","Int", &ArrayAPIExport::g_StringArrayExport, &SetAPIExport::g_StringSetExport, &g_StringDictIntExport);
	StringDictFloatExport g_StringDictFloatExport("String", "Float",&ArrayAPIExport::g_StringArrayExport, &SetAPIExport::g_StringSetExport, &g_StringDictFloatExport);
	StringDictStringExport g_StringDictStringExport("String", "String", &ArrayAPIExport::g_StringArrayExport, &SetAPIExport::g_StringSetExport, &g_StringDictStringExport);
	StringDictFormExport g_StringDictFormExport("String", "Form", &ArrayAPIExport::g_StringArrayExport, &SetAPIExport::g_StringSetExport, &g_StringDictFormExport);
	StringDictStructExport g_StringDictStructExport("String", &ArrayAPIExport::g_StringArrayExport, &SetAPIExport::g_StringSetExport, &g_StringDictStructExport);

	FormDictIntExport g_FormDictIntExport("Form","Int", &ArrayAPIExport::g_FormArrayExport, &SetAPIExport::g_FormSetExport, &g_FormDictIntExport);
	FormDictFloatExport g_FormDictFloatExport("Form", "Float",&ArrayAPIExport::g_FormArrayExport, &SetAPIExport::g_FormSetExport, &g_FormDictFloatExport);
	FormDictStringExport g_FormDictStringExport("Form", "String", &ArrayAPIExport::g_FormArrayExport, &SetAPIExport::g_FormSetExport, &g_FormDictStringExport);
	FormDictFormExport g_FormDictFormExport("Form", "Form", &ArrayAPIExport::g_FormArrayExport, &SetAPIExport::g_FormSetExport, &g_FormDictFormExport);
	FormDictStructExport g_FormDictStructExport("Form", &ArrayAPIExport::g_FormArrayExport, &SetAPIExport::g_FormSetExport, &g_FormDictStructExport);

	void DumpAll()
	{
		g_IntDictIntExport.DumpAll();
		g_IntDictFloatExport.DumpAll();
		g_IntDictStringExport.DumpAll();
		g_IntDictFormExport.DumpAll();
		g_IntDictStructExport.DumpAll();

		g_StringDictIntExport.DumpAll();
		g_StringDictFloatExport.DumpAll();
		g_StringDictStringExport.DumpAll();
		g_StringDictFormExport.DumpAll();
		g_StringDictStructExport.DumpAll();

		g_FormDictIntExport.DumpAll();
		g_FormDictFloatExport.DumpAll();
		g_FormDictStringExport.DumpAll();
		g_FormDictFormExport.DumpAll();
		g_FormDictStructExport.DumpAll();
	}

	void Clear()
	{
		g_IntDictIntExport.Clear();
		g_IntDictFloatExport.Clear();
		g_IntDictStringExport.Clear();
		g_IntDictFormExport.Clear();
		g_IntDictStructExport.Clear();

		g_StringDictIntExport.Clear();
		g_StringDictFloatExport.Clear();
		g_StringDictStringExport.Clear();
		g_StringDictFormExport.Clear();
		g_StringDictStructExport.Clear();

		g_FormDictIntExport.Clear();
		g_FormDictFloatExport.Clear();
		g_FormDictStringExport.Clear();
		g_FormDictFormExport.Clear();
		g_FormDictStructExport.Clear();
	}

	bool Save(const F4SESerializationInterface* serializer)
	{
		D("Saving int key dicts");
		g_IntDictIntExport.Save(serializer);
		g_IntDictFloatExport.Save(serializer);
		g_IntDictStringExport.Save(serializer);
		g_IntDictFormExport.Save(serializer);
		g_IntDictStructExport.Save(serializer);

		D("Saving string key dicts");
		g_StringDictIntExport.Save(serializer);
		g_StringDictFloatExport.Save(serializer);
		g_StringDictStringExport.Save(serializer);
		g_StringDictFormExport.Save(serializer);
		g_StringDictStructExport.Save(serializer);

		D("Saving Form key dicts");
		g_FormDictIntExport.Save(serializer);
		g_FormDictFloatExport.Save(serializer);
		g_FormDictStringExport.Save(serializer);
		g_FormDictFormExport.Save(serializer);
		g_FormDictStructExport.Save(serializer);

		return true;
	}

	void RegisterLoaders()
	{
		DS::LoadManager::RegisterRecordLoader(g_IntDictIntExport.DataId(), [](auto serializer, auto version, auto length){return g_IntDictIntExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_IntDictFloatExport.DataId(), [](auto serializer, auto version, auto length){return g_IntDictFloatExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_IntDictStringExport.DataId(), [](auto serializer, auto version, auto length){return g_IntDictStringExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_IntDictFormExport.DataId(), [](auto serializer, auto version, auto length){return g_IntDictFormExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_IntDictStructExport.DataId(), [](auto serializer, auto version, auto length){return g_IntDictStructExport.Load(serializer, version, length);});

		DS::LoadManager::RegisterRecordLoader(g_StringDictIntExport.DataId(), [](auto serializer, auto version, auto length){return g_StringDictIntExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_StringDictFloatExport.DataId(), [](auto serializer, auto version, auto length){return g_StringDictFloatExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_StringDictStringExport.DataId(), [](auto serializer, auto version, auto length){return g_StringDictStringExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_StringDictFormExport.DataId(), [](auto serializer, auto version, auto length){return g_StringDictFormExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_StringDictStructExport.DataId(), [](auto serializer, auto version, auto length){return g_StringDictStructExport.Load(serializer, version, length);});

		DS::LoadManager::RegisterRecordLoader(g_FormDictIntExport.DataId(), [](auto serializer, auto version, auto length){return g_FormDictIntExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_FormDictFloatExport.DataId(), [](auto serializer, auto version, auto length){return g_FormDictFloatExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_FormDictStringExport.DataId(), [](auto serializer, auto version, auto length){return g_FormDictStringExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_FormDictFormExport.DataId(), [](auto serializer, auto version, auto length){return g_FormDictFormExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_FormDictStructExport.DataId(), [](auto serializer, auto version, auto length){return g_FormDictStructExport.Load(serializer, version, length);});
	}
	
	bool Register(VirtualMachine* vm)
	{
		_MESSAGE("ValueDictAPIExport: Register");
		g_IntDictIntExport.Register(vm);
		g_IntDictFloatExport.Register(vm);
		g_IntDictStringExport.Register(vm);
		g_IntDictFormExport.Register(vm);
		g_IntDictStructExport.Register(vm);

		g_StringDictIntExport.Register(vm);
		g_StringDictFloatExport.Register(vm);
		g_StringDictStringExport.Register(vm);
		g_StringDictFormExport.Register(vm);
		g_StringDictStructExport.Register(vm);

		g_FormDictIntExport.Register(vm);
		g_FormDictFloatExport.Register(vm);
		g_FormDictStringExport.Register(vm);
		g_FormDictFormExport.Register(vm);
		g_FormDictStructExport.Register(vm);

		return true;
	}

}
