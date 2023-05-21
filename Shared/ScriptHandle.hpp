#pragma once
#include "MyTrace.h"
#include "f4se/PapyrusArgs.h"

inline UInt64 SafeGetHandle(VMObject* script)
{
	SInt32 oldLock = -1; // seems good enough for invalid value as locks count is positive
	__try
	{
		IComplexType* type = script->ValueRef().GetComplexType();
		if (!type)
			return 0;
		if (type->GetType() != VMValue::kType_Identifier)
			return 0;

		oldLock = script->ValueRef().data.id->Lock();
		D("OldLock=%d", oldLock);
		const UInt64 handle = script->ValueRef().data.id->m_handle;
		script->ValueRef().data.id->Unlock(oldLock);
		if (handle > 0) // valid handle is positive
			return handle;
		return 0;
	}
	__except(true)
	{
		if (oldLock != -1)
			script->ValueRef().data.id->Unlock(oldLock);
		return 0;
	}
}

struct ScriptHandle
{
	ScriptHandle() = default;
	ScriptHandle(VMObject* script)
	{
		if (script != nullptr)
		{
			Handle = SafeGetHandle(script);
			if (Handle != 0)
				Name = script->GetObjectType();
		}
	}
	ScriptHandle(UInt64 handle, BSFixedString scriptName)
	{
		Handle = handle;
		Name = scriptName;
	}
	bool IsInvalid() const
	{
		return Handle == 0 || Name.c_str() == nullptr || Name.c_str()[0] == 0;
	}

	VMObject AsObject(VirtualMachine * vm) const
	{
		VMValue val;
		val.SetNone();
		GetIdentifier(&val, Handle, &Name, vm);
		VMObject retVal;
		retVal.UnpackObject(&val);
		return retVal;
	}
	UInt64 Handle = 0;
	BSFixedString Name;
};

namespace std
{
	template <>
	struct equal_to<ScriptHandle>
	{
		bool operator()(const ScriptHandle& a, const ScriptHandle& b) const
		{
			return a.Handle == b.Handle;
		}
	};
	template <>
	struct hash<ScriptHandle>
	{
		std::hash<UInt64> Hash;
		size_t operator()(const ScriptHandle& s) const noexcept
		{
			return Hash(s.Handle);
		}
	};
}


