Scriptname AA:AVChanges Native Hidden Const

; Subscribe for ActorValues changes notifications
;
; handler - script that will receive notifications
;   this script should have following event handlers to receive notifications:
;     Function OnIncreased(Form[] senders, Actor a, ActorValue attribute, float newValue, float previousValue, float exceed)
;      ...
;     EndFunction
;     Function OnDecreased(Form[] senders, Actor a, ActorValue attribute, float newValue, float previousValue, float exceed)
;      ...
;     EndFunction
;
; actors - list of actors to listen for changes. 
;   can be empty. in this case changes made on any actor will be notified to this script
;   if it is empty then attributes list must not be empty
;   duplications are ignored
;
; attributes - list of ActorValues to listen for changes
;   can be empty. in this case change of any ActorValue caused by call of Increase/Decrease will be notified
;   if is not empty then calls of OnIncreased/OnDecreased will be called for these attributes only
;   duplications are ignored
;   it is possible that some ActorValue(s) are disappeared between saves when esp/esl/esm was uninstalled
;   so ActorValue shall not be changed and as result - event handler will not be invoked
; 
; Be aware about array length limitation - 128
;
; One of actors or attributes must be non empty. Subscription for all Actors and for all attributes is not allowed
; 
; returns true if registration was successful
; returns false if registration was not successfull
; reason of failure:
; 1. actors or/and attributes is None
; 2. both actors and attributes are empty 
; 3. script already has subscription
; 4. actors or/and attributes has None values in the list
bool function RegisterForAttributesChanges(ScriptObject handler, Actor[] actors, ActorValue[] attributes) native global

; Unsubscribe from attributes changes
; Script will not receive any notifications about attributes changes
; returns true if unsubscription was successful
; returns false if handler is not registered
bool function UnregisterForAttributesChanges(ScriptObject handler) native global

; Checks that handler is registered
; returns true if registered
; returns false if handler is None or is not registered
bool function IsHandlerRegistered(ScriptObject handler) native global


struct ScriptDescriptor
    int FormId
    int HighWord
    string Name
endstruct

; Returns information about subscribed script
; returned value can be used as argument UnregisterForAttributesChangesByDescriptor
; returns None if handler is invalid or not registered
ScriptDescriptor function GetSubscribedDescriptor(ScriptObject handler) native global

; Unsubscribe from attributes changes
; Specific API to have an ability to unsubscribe from destroyed ActiveMagicEffect's script
; returns true if unsubscription was successful
; returns false if handler is not registered
bool function UnregisterForAttributesChangesByDescriptor(ScriptDescriptor handler) native global

