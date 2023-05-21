#include "TestRunner.h"
#include "f4se/GameForms.h"
#include <thread>
#include "MyTrace.h"
#include "../DS/CollectionAccessor.h"
#include "ArrayTester.hpp"
#include "SetTester.hpp"
#include "DictTester.hpp"
#include "f4se/GameRTTI.h"

#include "TestDataProvider.hpp"

namespace Test
{
	constexpr UInt32 keyword = 0x0006D95E; // WRackTrigger

	F4SEMessagingInterface* messaging;
	PluginHandle myPlugin;

	DSAPI::IntArray1 intArray;
	DSAPI::FloatArray1 floatArray;
	DSAPI::StringArray1 stringArray;
	DSAPI::FormArray1 formArray;
	DSAPI::StructArray1 structArray;

	DSAPI::IntSet1 intSet;
	DSAPI::FloatSet1 floatSet;
	DSAPI::StringSet1 stringSet;
	DSAPI::FormSet1 formSet;

	DSAPI::IntDictInt1 intDictInt;
	DSAPI::IntDictFloat1 intDictFloat;
	DSAPI::IntDictString1 intDictString;
	DSAPI::IntDictForm1 intDictForm;
	DSAPI::IntDictStruct1 intDictStruct;

	DSAPI::StringDictInt1 stringDictInt;
	DSAPI::StringDictFloat1 stringDictFloat;
	DSAPI::StringDictString1 stringDictString;
	DSAPI::StringDictForm1 stringDictForm;
	DSAPI::StringDictStruct1 stringDictStruct;

	DSAPI::FormDictInt1 formDictInt;
	DSAPI::FormDictFloat1 formDictFloat;
	DSAPI::FormDictString1 formDictString;
	DSAPI::FormDictForm1 formDictForm;
	DSAPI::FormDictStruct1 formDictStruct;


	DSAPI::IntDictIntArray1 intDictIntArray;
	DSAPI::IntDictFloatArray1 intDictFloatArray;
	DSAPI::IntDictStringArray1 intDictStringArray;
	DSAPI::IntDictFormArray1 intDictFormArray;
	DSAPI::IntDictStructArray1 intDictStructArray;

	DSAPI::StringDictIntArray1 stringDictIntArray;
	DSAPI::StringDictFloatArray1 stringDictFloatArray;
	DSAPI::StringDictStringArray1 stringDictStringArray;
	DSAPI::StringDictFormArray1 stringDictFormArray;
	DSAPI::StringDictStructArray1 stringDictStructArray;

	DSAPI::FormDictIntArray1 formDictIntArray;
	DSAPI::FormDictFloatArray1 formDictFloatArray;
	DSAPI::FormDictStringArray1 formDictStringArray;
	DSAPI::FormDictFormArray1 formDictFormArray;
	DSAPI::FormDictStructArray1 formDictStructArray;

	bool TestStarted = false;
	bool volatile NeedCancel = false;
	void CancelTest()
	{
		NeedCancel = true;
	}


	void TestWorker()
	{
		using namespace Testers;
		const IntDataProvider intDataProvider;
		const FloatDataProvider floatDataProvider;
		const StringDataProvider stringDataProvider;
		const FormDataProvider formDataProvider;
		const StatAchievementProvider structDataProvider;

		BGSKeyword* id = DYNAMIC_CAST(LookupFormByID(keyword), TESForm, BGSKeyword);
		// arrays
		PrimitiveArrayTester("Int", id, intArray, intDataProvider).Perform(&NeedCancel);
		PrimitiveArrayTester("Float", id, floatArray, floatDataProvider).Perform(&NeedCancel);
		PrimitiveArrayTester("String", id, stringArray, stringDataProvider).Perform(&NeedCancel);
		PrimitiveArrayTester("Form", id, formArray, formDataProvider).Perform(&NeedCancel);
		StructArrayTester(id, structArray, structDataProvider).Perform(&NeedCancel);

		// sets
		SetTester("Int", id, intSet, intDataProvider).Perform(&NeedCancel);
		SetTester("Float", id, floatSet, floatDataProvider).Perform(&NeedCancel);
		SetTester("String", id, stringSet, stringDataProvider).Perform(&NeedCancel);
		SetTester("Form", id, formSet, formDataProvider).Perform(&NeedCancel);


		// value dicts
		PrimitiveValueDictTester("Int", "Int", id, intDictInt, intDataProvider, intDataProvider).Perform(&NeedCancel);
		PrimitiveValueDictTester("Int", "Float", id, intDictFloat, intDataProvider, floatDataProvider).Perform(&NeedCancel);
		PrimitiveValueDictTester("Int", "String", id, intDictString, intDataProvider, stringDataProvider).Perform(&NeedCancel);
		PrimitiveValueDictTester("Int", "Form", id, intDictForm, intDataProvider, formDataProvider).Perform(&NeedCancel);
		StructValueDictTester("Int", id, intDictStruct, intDataProvider, structDataProvider).Perform(&NeedCancel);

		PrimitiveValueDictTester("String", "Int", id, stringDictInt, stringDataProvider, intDataProvider).Perform(&NeedCancel);
		PrimitiveValueDictTester("String", "Float", id, stringDictFloat, stringDataProvider, floatDataProvider).Perform(&NeedCancel);
		PrimitiveValueDictTester("String", "String", id, stringDictString, stringDataProvider, stringDataProvider).Perform(&NeedCancel);
		PrimitiveValueDictTester("String", "Form", id, stringDictForm, stringDataProvider, formDataProvider).Perform(&NeedCancel);
		StructValueDictTester("String", id, stringDictStruct, stringDataProvider, structDataProvider).Perform(&NeedCancel);

		PrimitiveValueDictTester("Form", "Int", id, formDictInt, formDataProvider, intDataProvider).Perform(&NeedCancel);
		PrimitiveValueDictTester("Form", "Float", id, formDictFloat, formDataProvider, floatDataProvider).Perform(&NeedCancel);
		PrimitiveValueDictTester("Form", "String", id, formDictString, formDataProvider, stringDataProvider).Perform(&NeedCancel);
		PrimitiveValueDictTester("Form", "Form", id, formDictForm, formDataProvider, formDataProvider).Perform(&NeedCancel);
		StructValueDictTester("Form", id, formDictStruct, formDataProvider, structDataProvider).Perform(&NeedCancel);


		// array dicts
		PrimitiveArrayDictTester("Int", "Int", id, intDictIntArray, intDataProvider, intDataProvider).Perform(&NeedCancel);
		PrimitiveArrayDictTester("Int", "Float", id, intDictFloatArray, intDataProvider, floatDataProvider).Perform(&NeedCancel);
		PrimitiveArrayDictTester("Int", "String", id, intDictStringArray, intDataProvider, stringDataProvider).Perform(&NeedCancel);
		PrimitiveArrayDictTester("Int", "Form", id, intDictFormArray, intDataProvider, formDataProvider).Perform(&NeedCancel);
		StructArrayDictTester("Int", id, intDictStructArray, intDataProvider, structDataProvider).Perform(&NeedCancel);

		PrimitiveArrayDictTester("String", "Int", id, stringDictIntArray, stringDataProvider, intDataProvider).Perform(&NeedCancel);
		PrimitiveArrayDictTester("String", "Float", id, stringDictFloatArray, stringDataProvider, floatDataProvider).Perform(&NeedCancel);
		PrimitiveArrayDictTester("String", "String", id, stringDictStringArray, stringDataProvider, stringDataProvider).Perform(&NeedCancel);
		PrimitiveArrayDictTester("String", "Form", id, stringDictFormArray, stringDataProvider, formDataProvider).Perform(&NeedCancel);
		StructArrayDictTester("String", id, stringDictStructArray, stringDataProvider, structDataProvider).Perform(&NeedCancel);

		PrimitiveArrayDictTester("Form", "Int", id, formDictIntArray, formDataProvider, intDataProvider).Perform(&NeedCancel);
		PrimitiveArrayDictTester("Form", "Float", id, formDictFloatArray, formDataProvider, floatDataProvider).Perform(&NeedCancel);
		PrimitiveArrayDictTester("Form", "String", id, formDictStringArray, formDataProvider, stringDataProvider).Perform(&NeedCancel);
		PrimitiveArrayDictTester("Form", "Form", id, formDictFormArray, formDataProvider, formDataProvider).Perform(&NeedCancel);
		StructArrayDictTester("Form", id, formDictStructArray, formDataProvider, structDataProvider).Perform(&NeedCancel);

		TestStarted = false;
	}
	void Init(F4SEMessagingInterface* messaging, PluginHandle myPlugin)
	{
		Test::messaging = messaging;
		Test::myPlugin = myPlugin;
	}

	void InitiateTestsRun()
	{
		if (TestStarted)
		{
			W("Test started already");
			return;
		}
		TestStarted = true;
		NeedCancel = false;
		I("Starting DS tests");
		std::thread thread(TestWorker);
		thread.detach();
	}

	template <class Collection>
	void InitAccessorVariable(Collection& variable, const char* name, F4SEMessagingInterface::Message* msg)
	{
		const auto received = DSAPI::AsAccessor<Collection>(msg);
		if (received.has_value())
		{
			I("Received accessor for %s", name);
			variable = received.value();
		}
		else
		{
			DSAPI::Collection as_collection = DSAPI::GetAccessorInfo(msg);
			E("Failed to get %s accessor. version = %d, id=%.4s", name, as_collection.Version, &as_collection.Identifier);
		}
	}

	
	// contains collection type id of currently initializing accessor 
	// ProcessMessage should receive corresponding object
	DSAPI::AccessorType currentCollection = DSAPI::None;

	void RequestAccessor(DSAPI::AccessorType type, const char* name)
	{
		currentCollection = type;
		if (!DSAPI::RequestAccessor(messaging, myPlugin, currentCollection))
			E("Failed to request %s accessor", name);
	}

	void ProcessMessage(F4SEMessagingInterface::Message* msg)
	{
		I("Received DS message");
		switch (currentCollection)
		{
			case DSAPI::AccessorType::IntArray: InitAccessorVariable(intArray, "IntArray", msg); break;
			case DSAPI::AccessorType::FloatArray: InitAccessorVariable(floatArray, "FloatArray", msg); break;
			case DSAPI::AccessorType::StringArray: InitAccessorVariable(stringArray, "StringArray", msg); break;
			case DSAPI::AccessorType::FormArray: InitAccessorVariable(formArray, "FormArray", msg); break;
			case DSAPI::AccessorType::StructArray: InitAccessorVariable(structArray, "StructArray", msg); break;
			case DSAPI::AccessorType::IntSet: InitAccessorVariable(intSet, "IntSet", msg); break;
			case DSAPI::AccessorType::FloatSet: InitAccessorVariable(floatSet, "FloatSet", msg); break;
			case DSAPI::AccessorType::StringSet: InitAccessorVariable(stringSet, "StringSet", msg); break;
			case DSAPI::AccessorType::FormSet: InitAccessorVariable(formSet, "FormSet", msg); break;

			case DSAPI::AccessorType::IntDictInt: InitAccessorVariable(intDictInt, "IntDictInt", msg); break;
			case DSAPI::AccessorType::IntDictFloat: InitAccessorVariable(intDictFloat, "IntDictFloat", msg); break;
			case DSAPI::AccessorType::IntDictString: InitAccessorVariable(intDictString, "IntDictString", msg); break;
			case DSAPI::AccessorType::IntDictForm: InitAccessorVariable(intDictForm, "IntDictForm", msg); break;
			case DSAPI::AccessorType::IntDictStruct: InitAccessorVariable(intDictStruct, "IntDictStruct", msg); break;

			case DSAPI::AccessorType::StringDictInt: InitAccessorVariable(stringDictInt, "StringDictInt", msg); break;
			case DSAPI::AccessorType::StringDictFloat: InitAccessorVariable(stringDictFloat, "StringDictFloat", msg); break;
			case DSAPI::AccessorType::StringDictString: InitAccessorVariable(stringDictString, "StringDictString", msg); break;
			case DSAPI::AccessorType::StringDictForm: InitAccessorVariable(stringDictForm, "StringDictForm", msg); break;
			case DSAPI::AccessorType::StringDictStruct: InitAccessorVariable(stringDictStruct, "StringDictStruct", msg); break;

			case DSAPI::AccessorType::FormDictInt: InitAccessorVariable(formDictInt, "FormDictInt", msg); break;
			case DSAPI::AccessorType::FormDictFloat: InitAccessorVariable(formDictFloat, "FormDictFloat", msg); break;
			case DSAPI::AccessorType::FormDictString: InitAccessorVariable(formDictString, "FormDictString", msg); break;
			case DSAPI::AccessorType::FormDictForm: InitAccessorVariable(formDictForm, "FormDictForm", msg); break;
			case DSAPI::AccessorType::FormDictStruct: InitAccessorVariable(formDictStruct, "FormDictStruct", msg); break;


			case DSAPI::AccessorType::IntDictIntArray: InitAccessorVariable(intDictIntArray, "IntDictIntArray", msg); break;
			case DSAPI::AccessorType::IntDictFloatArray: InitAccessorVariable(intDictFloatArray, "IntDictFloatArray", msg); break;
			case DSAPI::AccessorType::IntDictStringArray: InitAccessorVariable(intDictStringArray, "IntDictStringArray", msg); break;
			case DSAPI::AccessorType::IntDictFormArray: InitAccessorVariable(intDictFormArray, "IntDictFormArray", msg); break;
			case DSAPI::AccessorType::IntDictStructArray: InitAccessorVariable(intDictStructArray, "IntDictStructArray", msg); break;

			case DSAPI::AccessorType::StringDictIntArray: InitAccessorVariable(stringDictIntArray, "StringDictIntArray", msg); break;
			case DSAPI::AccessorType::StringDictFloatArray: InitAccessorVariable(stringDictFloatArray, "StringDictFloatArray", msg); break;
			case DSAPI::AccessorType::StringDictStringArray: InitAccessorVariable(stringDictStringArray, "StringDictStringArray", msg); break;
			case DSAPI::AccessorType::StringDictFormArray: InitAccessorVariable(stringDictFormArray, "StringDictFormArray", msg); break;
			case DSAPI::AccessorType::StringDictStructArray: InitAccessorVariable(stringDictStructArray, "StringDictStructArray", msg); break;

			case DSAPI::AccessorType::FormDictIntArray: InitAccessorVariable(formDictIntArray, "FormDictIntArray", msg); break;
			case DSAPI::AccessorType::FormDictFloatArray: InitAccessorVariable(formDictFloatArray, "FormDictFloatArray", msg); break;
			case DSAPI::AccessorType::FormDictStringArray: InitAccessorVariable(formDictStringArray, "FormDictStringArray", msg); break;
			case DSAPI::AccessorType::FormDictFormArray: InitAccessorVariable(formDictFormArray, "FormDictFormArray", msg); break;
			case DSAPI::AccessorType::FormDictStructArray: InitAccessorVariable(formDictStructArray, "FormDictStructArray", msg); break;
			default:
				E("Unexpected message received");
			break;
		}

	}


	void InitiateAccessorsInitialization()
	{
		RequestAccessor(DSAPI::AccessorType::IntArray, "IntArray");
		RequestAccessor(DSAPI::AccessorType::FloatArray, "FloatArray");
		RequestAccessor(DSAPI::AccessorType::StringArray, "StringArray");
		RequestAccessor(DSAPI::AccessorType::FormArray, "FormArray");
		RequestAccessor(DSAPI::AccessorType::StructArray, "StructArray");

		RequestAccessor(DSAPI::AccessorType::IntSet, "IntSet");
		RequestAccessor(DSAPI::AccessorType::FloatSet, "FloatSet");
		RequestAccessor(DSAPI::AccessorType::StringSet, "StringSet");
		RequestAccessor(DSAPI::AccessorType::FormSet, "FormSet");
		
		RequestAccessor(DSAPI::AccessorType::IntDictInt, "IntDictInt");
		RequestAccessor(DSAPI::AccessorType::IntDictFloat, "IntDictFloat");
		RequestAccessor(DSAPI::AccessorType::IntDictString, "IntDictString");
		RequestAccessor(DSAPI::AccessorType::IntDictForm, "IntDictForm");
		RequestAccessor(DSAPI::AccessorType::IntDictStruct, "IntDictStruct");

		RequestAccessor(DSAPI::AccessorType::StringDictInt, "StringDictInt");
		RequestAccessor(DSAPI::AccessorType::StringDictFloat, "StringDictFloat");
		RequestAccessor(DSAPI::AccessorType::StringDictString, "StringDictString");
		RequestAccessor(DSAPI::AccessorType::StringDictForm, "StringDictForm");
		RequestAccessor(DSAPI::AccessorType::StringDictStruct, "StringDictStruct");

		RequestAccessor(DSAPI::AccessorType::FormDictInt, "FormDictInt");
		RequestAccessor(DSAPI::AccessorType::FormDictFloat, "FormDictFloat");
		RequestAccessor(DSAPI::AccessorType::FormDictString, "FormDictString");
		RequestAccessor(DSAPI::AccessorType::FormDictForm, "FormDictForm");
		RequestAccessor(DSAPI::AccessorType::FormDictStruct, "FormDictStruct");

		
		RequestAccessor(DSAPI::AccessorType::IntDictIntArray, "IntDictIntArray");
		RequestAccessor(DSAPI::AccessorType::IntDictFloatArray, "IntDictFloatArray");
		RequestAccessor(DSAPI::AccessorType::IntDictStringArray, "IntDictStringArray");
		RequestAccessor(DSAPI::AccessorType::IntDictFormArray, "IntDictFormArray");
		RequestAccessor(DSAPI::AccessorType::IntDictStructArray, "IntDictStructArray");

		RequestAccessor(DSAPI::AccessorType::StringDictIntArray, "StringDictIntArray");
		RequestAccessor(DSAPI::AccessorType::StringDictFloatArray, "StringDictFloatArray");
		RequestAccessor(DSAPI::AccessorType::StringDictStringArray, "StringDictStringArray");
		RequestAccessor(DSAPI::AccessorType::StringDictFormArray, "StringDictFormArray");
		RequestAccessor(DSAPI::AccessorType::StringDictStructArray, "StringDictStructArray");

		RequestAccessor(DSAPI::AccessorType::FormDictIntArray, "FormDictIntArray");
		RequestAccessor(DSAPI::AccessorType::FormDictFloatArray, "FormDictFloatArray");
		RequestAccessor(DSAPI::AccessorType::FormDictStringArray, "FormDictStringArray");
		RequestAccessor(DSAPI::AccessorType::FormDictFormArray, "FormDictFormArray");
		RequestAccessor(DSAPI::AccessorType::FormDictStructArray, "FormDictStructArray");

		currentCollection = DSAPI::AccessorType::None;
	}
}
