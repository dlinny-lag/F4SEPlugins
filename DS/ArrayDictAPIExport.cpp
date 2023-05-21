#include "ArrayDictAPIExport.h"

#include "LoadManager.h"

namespace ArrayDictAPIExport
{
	IntDictIntArrayExport g_IntDictIntArrayExport("Int", "Int",&ArrayAPIExport::g_IntArrayExport, &SetAPIExport::g_IntSetExport, &ArrayAPIExport::g_IntArrayExport,&g_IntDictIntArrayExport);
	IntDictFloatArrayExport g_IntDictFloatArrayExport("Int", "Float",&ArrayAPIExport::g_IntArrayExport, &SetAPIExport::g_IntSetExport, &ArrayAPIExport::g_FloatArrayExport,&g_IntDictFloatArrayExport);
	IntDictStringArrayExport g_IntDictStringArrayExport("Int", "String",&ArrayAPIExport::g_IntArrayExport, &SetAPIExport::g_IntSetExport, &ArrayAPIExport::g_StringArrayExport,&g_IntDictStringArrayExport);
	IntDictFormArrayExport g_IntDictFormArrayExport("Int", "Form",&ArrayAPIExport::g_IntArrayExport, &SetAPIExport::g_IntSetExport, &ArrayAPIExport::g_FormArrayExport,&g_IntDictFormArrayExport);
	IntDictStructArrayExport g_IntDictStructArrayExport("Int", &ArrayAPIExport::g_IntArrayExport, &SetAPIExport::g_IntSetExport, &ArrayAPIExport::g_StructArrayExport,&g_IntDictStructArrayExport);

	StringDictIntArrayExport g_StringDictIntArrayExport("String", "Int",&ArrayAPIExport::g_StringArrayExport, &SetAPIExport::g_StringSetExport, &ArrayAPIExport::g_IntArrayExport,&g_StringDictIntArrayExport);
	StringDictFloatArrayExport g_StringDictFloatArrayExport("String", "Float",&ArrayAPIExport::g_StringArrayExport, &SetAPIExport::g_StringSetExport, &ArrayAPIExport::g_FloatArrayExport,&g_StringDictFloatArrayExport);
	StringDictStringArrayExport g_StringDictStringArrayExport("String", "String",&ArrayAPIExport::g_StringArrayExport, &SetAPIExport::g_StringSetExport, &ArrayAPIExport::g_StringArrayExport,&g_StringDictStringArrayExport);
	StringDictFormArrayExport g_StringDictFormArrayExport("String", "Form",&ArrayAPIExport::g_StringArrayExport, &SetAPIExport::g_StringSetExport, &ArrayAPIExport::g_FormArrayExport,&g_StringDictFormArrayExport);
	StringDictStructArrayExport g_StringDictStructArrayExport("String", &ArrayAPIExport::g_StringArrayExport, &SetAPIExport::g_StringSetExport, &ArrayAPIExport::g_StructArrayExport,&g_StringDictStructArrayExport);

	FormDictIntArrayExport g_FormDictIntArrayExport("Form", "Int",&ArrayAPIExport::g_FormArrayExport, &SetAPIExport::g_FormSetExport, &ArrayAPIExport::g_IntArrayExport,&g_FormDictIntArrayExport);
	FormDictFloatArrayExport g_FormDictFloatArrayExport("Form", "Float",&ArrayAPIExport::g_FormArrayExport, &SetAPIExport::g_FormSetExport, &ArrayAPIExport::g_FloatArrayExport,&g_FormDictFloatArrayExport);
	FormDictStringArrayExport g_FormDictStringArrayExport("Form", "String",&ArrayAPIExport::g_FormArrayExport, &SetAPIExport::g_FormSetExport, &ArrayAPIExport::g_StringArrayExport,&g_FormDictStringArrayExport);
	FormDictFormArrayExport g_FormDictFormArrayExport("Form", "Form",&ArrayAPIExport::g_FormArrayExport, &SetAPIExport::g_FormSetExport, &ArrayAPIExport::g_FormArrayExport,&g_FormDictFormArrayExport);
	FormDictStructArrayExport g_FormDictStructArrayExport("Form", &ArrayAPIExport::g_FormArrayExport, &SetAPIExport::g_FormSetExport, &ArrayAPIExport::g_StructArrayExport,&g_FormDictStructArrayExport);

	void DumpAll()
	{
		g_IntDictIntArrayExport.DumpAll();
		g_IntDictFloatArrayExport.DumpAll();
		g_IntDictStringArrayExport.DumpAll();
		g_IntDictFormArrayExport.DumpAll();
		g_IntDictStructArrayExport.DumpAll();

		g_StringDictIntArrayExport.DumpAll();
		g_StringDictFloatArrayExport.DumpAll();
		g_StringDictStringArrayExport.DumpAll();
		g_StringDictFormArrayExport.DumpAll();
		g_StringDictStructArrayExport.DumpAll();

		g_FormDictIntArrayExport.DumpAll();
		g_FormDictFloatArrayExport.DumpAll();
		g_FormDictStringArrayExport.DumpAll();
		g_FormDictFormArrayExport.DumpAll();
		g_FormDictStructArrayExport.DumpAll();
	}

	void Clear()
	{
		g_IntDictIntArrayExport.Clear();
		g_IntDictFloatArrayExport.Clear();
		g_IntDictStringArrayExport.Clear();
		g_IntDictFormArrayExport.Clear();
		g_IntDictStructArrayExport.Clear();

		g_StringDictIntArrayExport.Clear();
		g_StringDictFloatArrayExport.Clear();
		g_StringDictStringArrayExport.Clear();
		g_StringDictFormArrayExport.Clear();
		g_StringDictStructArrayExport.Clear();

		g_FormDictIntArrayExport.Clear();
		g_FormDictFloatArrayExport.Clear();
		g_FormDictStringArrayExport.Clear();
		g_FormDictFormArrayExport.Clear();
		g_FormDictStructArrayExport.Clear();
	}

	bool Save(const F4SESerializationInterface* serializer)
	{
		g_IntDictIntArrayExport.Save(serializer);
		g_IntDictFloatArrayExport.Save(serializer);
		g_IntDictStringArrayExport.Save(serializer);
		g_IntDictFormArrayExport.Save(serializer);
		g_IntDictStructArrayExport.Save(serializer);

		g_StringDictIntArrayExport.Save(serializer);
		g_StringDictFloatArrayExport.Save(serializer);
		g_StringDictStringArrayExport.Save(serializer);
		g_StringDictFormArrayExport.Save(serializer);
		g_StringDictStructArrayExport.Save(serializer);

		g_FormDictIntArrayExport.Save(serializer);
		g_FormDictFloatArrayExport.Save(serializer);
		g_FormDictStringArrayExport.Save(serializer);
		g_FormDictFormArrayExport.Save(serializer);
		g_FormDictStructArrayExport.Save(serializer);

		return true;
	}

	void RegisterLoaders()
	{
		DS::LoadManager::RegisterRecordLoader(g_IntDictIntArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_IntDictIntArrayExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_IntDictFloatArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_IntDictFloatArrayExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_IntDictStringArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_IntDictStringArrayExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_IntDictFormArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_IntDictFormArrayExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_IntDictStructArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_IntDictStructArrayExport.Load(serializer, version, length);});

		DS::LoadManager::RegisterRecordLoader(g_StringDictIntArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_StringDictIntArrayExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_StringDictFloatArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_StringDictFloatArrayExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_StringDictFormArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_StringDictFormArrayExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_IntDictFormArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_IntDictFormArrayExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_StringDictStructArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_StringDictStructArrayExport.Load(serializer, version, length);});

		DS::LoadManager::RegisterRecordLoader(g_FormDictIntArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_FormDictIntArrayExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_FormDictFloatArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_FormDictFloatArrayExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_FormDictStringArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_FormDictStringArrayExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_FormDictFormArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_FormDictFormArrayExport.Load(serializer, version, length);});
		DS::LoadManager::RegisterRecordLoader(g_FormDictStructArrayExport.DataId(), [](auto serializer, auto version, auto length){return g_FormDictStructArrayExport.Load(serializer, version, length);});
	}

	bool Register(VirtualMachine* vm)
	{
		_MESSAGE("ArrayDictAPIExport: Register");

		g_IntDictIntArrayExport.Register(vm);
		g_IntDictFloatArrayExport.Register(vm);
		g_IntDictStringArrayExport.Register(vm);
		g_IntDictFormArrayExport.Register(vm);
		g_IntDictStructArrayExport.Register(vm);

		g_StringDictIntArrayExport.Register(vm);
		g_StringDictFloatArrayExport.Register(vm);
		g_StringDictStringArrayExport.Register(vm);
		g_StringDictFormArrayExport.Register(vm);
		g_StringDictStructArrayExport.Register(vm);

		g_FormDictIntArrayExport.Register(vm);
		g_FormDictFloatArrayExport.Register(vm);
		g_FormDictStringArrayExport.Register(vm);
		g_FormDictFormArrayExport.Register(vm);
		g_FormDictStructArrayExport.Register(vm);

		return true;
	}
;}
