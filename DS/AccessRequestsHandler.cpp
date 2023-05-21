#include "AccessRequestsHandler.defs.h"
#include "AccessRequestsHandler.h"
#include "MyTrace.h"

namespace DSAPI
{
	// arrays

	IntArray1 GetIntArrayAccessor()
	{
		IntArray1 retVal{IntArray1::version};
		DECLARE_PRIMITIVE_ARRAY_ACCESSOR_BODY(retVal, Int)
		return retVal;
	}
	FloatArray1 GetFloatArrayAccessor()
	{
		FloatArray1 retVal{FloatArray1::version};
		DECLARE_PRIMITIVE_ARRAY_ACCESSOR_BODY(retVal, Float)
		return retVal;
	}
	StringArray1 GetStringArrayAccessor()
	{
		StringArray1 retVal{StringArray1::version};
		DECLARE_PRIMITIVE_ARRAY_ACCESSOR_BODY(retVal, String)
		return retVal;
	}
	FormArray1 GetFormArrayAccessor()
	{
		FormArray1 retVal{FormArray1::version};
		DECLARE_PRIMITIVE_ARRAY_ACCESSOR_BODY(retVal, Form)
		return retVal;
	}
	StructArray1 GetStructArrayAccessor()
	{
		StructArray1 retVal{StructArray1::version};
		DECLARE_STRUCT_ARRAY_ACCESSOR_BODY(retVal);
		return retVal;
	}

	// sets

	IntSet1 GetIntSetAccessor()
	{
		IntSet1 retVal{IntSet1::version};
		DECLARE_SET_ACCESSOR_BODY(retVal, Int);
		return retVal;
	}
	FloatSet1 GetFloatSetAccessor()
	{
		FloatSet1 retVal{FloatSet1::version};
		DECLARE_SET_ACCESSOR_BODY(retVal, Float);
		return retVal;
	}
	StringSet1 GetStringSetAccessor()
	{
		StringSet1 retVal{StringSet1::version};
		DECLARE_SET_ACCESSOR_BODY(retVal, String);
		return retVal;
	}
	FormSet1 GetFormSetAccessor()
	{
		FormSet1 retVal{FormSet1::version};
		DECLARE_SET_ACCESSOR_BODY(retVal, Form);
		return retVal;
	}

	// value dicts

	IntDictInt1 GetIntDictIntAccessor()
	{
		IntDictInt1 retVal{IntDictInt1::version};
		DECLARE_PRIMITIVE_VALUE_DICT_BODY(retVal, Int, Int);
		return retVal;
	}
	IntDictFloat1 GetIntDictFloatAccessor()
	{
		IntDictFloat1 retVal{IntDictFloat1::version};
		DECLARE_PRIMITIVE_VALUE_DICT_BODY(retVal, Int, Float);
		return retVal;
	}
	IntDictString1 GetIntDictStringAccessor()
	{
		IntDictString1 retVal{IntDictString1::version};
		DECLARE_PRIMITIVE_VALUE_DICT_BODY(retVal, Int, String);
		return retVal;
	}
	IntDictForm1 GetIntDictFormAccessor()
	{
		IntDictForm1 retVal{IntDictForm1::version};
		DECLARE_PRIMITIVE_VALUE_DICT_BODY(retVal, Int, Form);
		return retVal;
	}
	IntDictStruct1 GetIntDictStructAccessor()
	{
		IntDictStruct1 retVal{IntDictStruct1::version};
		DECLARE_STRUCT_VALUE_DICT_BODY(retVal, Int);
		return retVal;
	}
	StringDictInt1 GetStringDictIntAccessor()
	{
		StringDictInt1 retVal{StringDictInt1::version};
		DECLARE_PRIMITIVE_VALUE_DICT_BODY(retVal, String, Int);
		return retVal;
	}
	StringDictFloat1 GetStringDictFloatAccessor()
	{
		StringDictFloat1 retVal{StringDictFloat1::version};
		DECLARE_PRIMITIVE_VALUE_DICT_BODY(retVal, String, Float);
		return retVal;
	}
	StringDictString1 GetStringDictStringAccessor()
	{
		StringDictString1 retVal{StringDictString1::version};
		DECLARE_PRIMITIVE_VALUE_DICT_BODY(retVal, String, String);
		return retVal;
	}
	StringDictForm1 GetStringDictFormAccessor()
	{
		StringDictForm1 retVal{StringDictForm1::version};
		DECLARE_PRIMITIVE_VALUE_DICT_BODY(retVal, String, Form);
		return retVal;
	}
	StringDictStruct1 GetStringDictStructAccessor()
	{
		StringDictStruct1 retVal{StringDictStruct1::version};
		DECLARE_STRUCT_VALUE_DICT_BODY(retVal, String);
		return retVal;
	}
	FormDictInt1 GetFormDictIntAccessor()
	{
		FormDictInt1 retVal{FormDictInt1::version};
		DECLARE_PRIMITIVE_VALUE_DICT_BODY(retVal, Form, Int);
		return retVal;
	}
	FormDictFloat1 GetFormDictFloatAccessor()
	{
		FormDictFloat1 retVal{FormDictFloat1::version};
		DECLARE_PRIMITIVE_VALUE_DICT_BODY(retVal, Form, Float);
		return retVal;
	}
	FormDictString1 GetFormDictStringAccessor()
	{
		FormDictString1 retVal{FormDictString1::version};
		DECLARE_PRIMITIVE_VALUE_DICT_BODY(retVal, Form, String);
		return retVal;
	}
	FormDictForm1 GetFormDictFormAccessor()
	{
		FormDictForm1 retVal{FormDictForm1::version};
		DECLARE_PRIMITIVE_VALUE_DICT_BODY(retVal, Form, Form);
		return retVal;
	}
	FormDictStruct1 GetFormDictStructAccessor()
	{
		FormDictStruct1 retVal{FormDictStruct1::version};
		DECLARE_STRUCT_VALUE_DICT_BODY(retVal, Form);
		return retVal;
	}

	// array dicts

	IntDictIntArray1 GetIntDictIntArrayAccessor()
	{
		IntDictIntArray1 retVal{IntDictIntArray1::version};
		DECLARE_PRIMITIVE_ARRAY_DICT_BODY(retVal, Int, Int);
		return retVal;
	}
	IntDictFloatArray1 GetIntDictFloatArrayAccessor()
	{
		IntDictFloatArray1 retVal{IntDictFloatArray1::version};
		DECLARE_PRIMITIVE_ARRAY_DICT_BODY(retVal, Int, Float);
		return retVal;
	}
	IntDictStringArray1 GetIntDictStringArrayAccessor()
	{
		IntDictStringArray1 retVal{IntDictStringArray1::version};
		DECLARE_PRIMITIVE_ARRAY_DICT_BODY(retVal, Int, String);
		return retVal;
	}
	IntDictFormArray1 GetIntDictFormArrayAccessor()
	{
		IntDictFormArray1 retVal{IntDictFormArray1::version};
		DECLARE_PRIMITIVE_ARRAY_DICT_BODY(retVal, Int, Form);
		return retVal;
	}
	IntDictStructArray1 GetIntDictStructArrayAccessor()
	{
		IntDictStructArray1 retVal{IntDictStructArray1::version};
		DECLARE_STRUCT_ARRAY_DICT_BODY(retVal, Int);
		return retVal;
	}
	StringDictIntArray1 GetStringDictIntArrayAccessor()
	{
		StringDictIntArray1 retVal{StringDictIntArray1::version};
		DECLARE_PRIMITIVE_ARRAY_DICT_BODY(retVal, String, Int);
		return retVal;
	}
	StringDictFloatArray1 GetStringDictFloatArrayAccessor()
	{
		StringDictFloatArray1 retVal{StringDictFloatArray1::version};
		DECLARE_PRIMITIVE_ARRAY_DICT_BODY(retVal, String, Float);
		return retVal;
	}
	StringDictStringArray1 GetStringDictStringArrayAccessor()
	{
		StringDictStringArray1 retVal{StringDictStringArray1::version};
		DECLARE_PRIMITIVE_ARRAY_DICT_BODY(retVal, String, String);
		return retVal;
	}
	StringDictFormArray1 GetStringDictFormArrayAccessor()
	{
		StringDictFormArray1 retVal{StringDictFormArray1::version};
		DECLARE_PRIMITIVE_ARRAY_DICT_BODY(retVal, String, Form);
		return retVal;
	}
	StringDictStructArray1 GetStringDictStructArrayAccessor()
	{
		StringDictStructArray1 retVal{StringDictStructArray1::version};
		DECLARE_STRUCT_ARRAY_DICT_BODY(retVal, String);
		return retVal;
	}
	FormDictIntArray1 GetFormDictIntArrayAccessor()
	{
		FormDictIntArray1 retVal{FormDictIntArray1::version};
		DECLARE_PRIMITIVE_ARRAY_DICT_BODY(retVal, Form, Int);
		return retVal;
	}
	FormDictFloatArray1 GetFormDictFloatArrayAccessor()
	{
		FormDictFloatArray1 retVal{FormDictFloatArray1::version};
		DECLARE_PRIMITIVE_ARRAY_DICT_BODY(retVal, Form, Float);
		return retVal;
	}
	FormDictStringArray1 GetFormDictStringArrayAccessor()
	{
		FormDictStringArray1 retVal{FormDictStringArray1::version};
		DECLARE_PRIMITIVE_ARRAY_DICT_BODY(retVal, Form, String);
		return retVal;
	}
	FormDictFormArray1 GetFormDictFormArrayAccessor()
	{
		FormDictFormArray1 retVal{FormDictFormArray1::version};
		DECLARE_PRIMITIVE_ARRAY_DICT_BODY(retVal, Form, Form);
		return retVal;
	}
	FormDictStructArray1 GetFormDictStructArrayAccessor()
	{
		FormDictStructArray1 retVal{FormDictStructArray1::version};
		DECLARE_STRUCT_ARRAY_DICT_BODY(retVal, Form);
		return retVal;
	}

	constexpr int currentVersion = 1;

	template<typename Accessor>
	bool AccessorRequestProcessor::Dispatch(Accessor toSend, const char* receiver) const
	{
#ifdef _DEBUG
		// try to detect unassigned functions pointers
		int64_t* ptr = static_cast<int64_t*>(static_cast<void*>(&toSend)); 
		for(int i = 0; i < sizeof(Accessor)/sizeof(int64_t); ++i)
		{
			if (0LL == *ptr)
				E("Accessor is not properly initialized. 0 at %d offset", i);
			++ptr;
		}
#endif
		return messaging->Dispatch(plugin, AccessorMessage, &toSend, sizeof(toSend), receiver);
	}

	bool AccessorRequestProcessor::ProcessRequest(const AccessorRequest* request, const char* requester) const
	{
		if (request->Version != currentVersion)
			return false;
		switch(request->Type)
		{
			case AccessorType::IntArray: return Dispatch(GetIntArrayAccessor(), requester);
			case AccessorType::FloatArray: return Dispatch(GetFloatArrayAccessor(), requester);
			case AccessorType::StringArray: return Dispatch(GetStringArrayAccessor(), requester);
			case AccessorType::FormArray: return Dispatch(GetFormArrayAccessor(), requester);
			case AccessorType::StructArray: return Dispatch(GetStructArrayAccessor(), requester);
			case AccessorType::IntSet: return Dispatch(GetIntSetAccessor(), requester);
			case AccessorType::FloatSet: return Dispatch(GetFloatSetAccessor(), requester);
			case AccessorType::StringSet: return Dispatch(GetStringSetAccessor(), requester);
			case AccessorType::FormSet: return Dispatch(GetFormSetAccessor(), requester);

			case AccessorType::IntDictInt: return Dispatch(GetIntDictIntAccessor(), requester);
			case AccessorType::IntDictFloat: return Dispatch(GetIntDictFloatAccessor(), requester);
			case AccessorType::IntDictString: return Dispatch(GetIntDictStringAccessor(), requester);
			case AccessorType::IntDictForm: return Dispatch(GetIntDictFormAccessor(), requester);
			case AccessorType::IntDictStruct: return Dispatch(GetIntDictStructAccessor(), requester);

			case AccessorType::StringDictInt: return Dispatch(GetStringDictIntAccessor(), requester);
			case AccessorType::StringDictFloat: return Dispatch(GetStringDictFloatAccessor(), requester);
			case AccessorType::StringDictString: return Dispatch(GetStringDictStringAccessor(), requester);
			case AccessorType::StringDictForm: return Dispatch(GetStringDictFormAccessor(), requester);
			case AccessorType::StringDictStruct: return Dispatch(GetStringDictStructAccessor(), requester);

			case AccessorType::FormDictInt: return Dispatch(GetFormDictIntAccessor(), requester);
			case AccessorType::FormDictFloat: return Dispatch(GetFormDictFloatAccessor(), requester);
			case AccessorType::FormDictString: return Dispatch(GetFormDictStringAccessor(), requester);
			case AccessorType::FormDictForm: return Dispatch(GetFormDictFormAccessor(), requester);
			case AccessorType::FormDictStruct: return Dispatch(GetFormDictStructAccessor(), requester);

			case AccessorType::IntDictIntArray: return Dispatch(GetIntDictIntArrayAccessor(), requester);
			case AccessorType::IntDictFloatArray: return Dispatch(GetIntDictFloatArrayAccessor(), requester);
			case AccessorType::IntDictStringArray: return Dispatch(GetIntDictStringArrayAccessor(), requester);
			case AccessorType::IntDictFormArray: return Dispatch(GetIntDictFormArrayAccessor(), requester);
			case AccessorType::IntDictStructArray: return Dispatch(GetIntDictStructArrayAccessor(), requester);

			case AccessorType::StringDictIntArray: return Dispatch(GetStringDictIntArrayAccessor(), requester);
			case AccessorType::StringDictFloatArray: return Dispatch(GetStringDictFloatArrayAccessor(), requester);
			case AccessorType::StringDictStringArray: return Dispatch(GetStringDictStringArrayAccessor(), requester);
			case AccessorType::StringDictFormArray: return Dispatch(GetStringDictFormArrayAccessor(), requester);
			case AccessorType::StringDictStructArray: return Dispatch(GetStringDictStructArrayAccessor(), requester);

			case AccessorType::FormDictIntArray: return Dispatch(GetFormDictIntArrayAccessor(), requester);
			case AccessorType::FormDictFloatArray: return Dispatch(GetFormDictFloatArrayAccessor(), requester);
			case AccessorType::FormDictStringArray: return Dispatch(GetFormDictStringArrayAccessor(), requester);
			case AccessorType::FormDictFormArray: return Dispatch(GetFormDictFormArrayAccessor(), requester);
			case AccessorType::FormDictStructArray: return Dispatch(GetFormDictStructArrayAccessor(), requester);

		}
		return false;
	}
}
