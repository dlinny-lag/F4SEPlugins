#include "BSReferenceTypeUtils.h"
#include "Shared/MyTrace.h"

VMObjectTypeInfo* formType = nullptr;
bool BSReferenceTypeUtils::IsForm(const VMObjectTypeInfo* type)
{
	if (!formType)
	{
		BSFixedString formName("Form");
		if (!(*g_gameVM)->m_virtualMachine->GetObjectTypeInfoByName(&formName, &formType))
		{
			E("Fatal! Can't get Form type info");
		}
		formName.Release();
	}
	return Is(type, formType);
}

bool BSReferenceTypeUtils::Is(const VMObjectTypeInfo* type, const VMObjectTypeInfo* basetype)
{
	if (!type)
		return false;
	if (!basetype)
		return false;

	if (type == basetype)
		return true;
	while (type != nullptr)
	{
		if (type->m_parent == basetype)
			return true;
		type = (VMObjectTypeInfo*)type->m_parent;
	}
	return false;
}
