Scriptname StringAttribute Native Hidden Const

; get version number of the plugin
int function GetVersion() native global

; get string representation of version of the plugin
string function GetVersionString() native global


; registers default value for string attribute
;   attribute - keyword to identify attribute
;   defaultValue - it will be assigned when actor is initialized first time
; returns true if successfuly registered
; returns false if arguments are invalid or attribute is registered already 
bool function RegisterStringAttribute(Keyword attribute, string defaultValue) native global

; unregisters string attribute
;   attribute - keyword to identify attribute
; return true on success
; returns false if argument is invalid or attribute was not registered
bool function UnregisterStringAttribute(Keyword attribute) native global

; return true if attribute is registered. false - otherwise.
;   attribute - keyword to identify attribute
bool function IsStringAttributeRegistered(Keyword attribute) native global

; returns previously assigned value
; returns empty string if arguments are invalid or value was never assigned
string function SetString(Form sender, Actor a, Keyword attribute, string value) native global

; appends string to attribute
; returns result of concatenation
; if attribute was not assigned passed value will be returned
string function Append(Form sender, Actor a, Keyword attribute, string value) native global

; returns current value of attribute
; returns empty string if arguments are invalid or value was never assigned
string function GetString(Actor a, Keyword attribute) native global

; registers for string attribute changes
; handler - script that will receive notifications
;  this script should have following event handler to receive notifications:
;  function OnStringChanged(Form sender, Actor a, Keyword attribute, string newValue, string prevValue)
;   ; sender - identifier of sender who changed value, can be None
;   ; a - actor what attribute was changed
;   ; attribute - changed attribute
;  endfunction
;   notification handler is matched by its name, so handler script must have single instance only
;   so reasonable option - use script for Quest object
;   potentionally script names can be same for two mods, so it is recommended to use unique namespace or prefix for your script
;   renaming of script after registration will cause inablity to onvoke event handler on this script
;
; actors - liat of actors to listen for change
;   can be empty. in this case changes made on any actor will be notified to this script
;   if it is empty then attributes list must not be empty
;   duplications are ignored
; attributes - list of Keywords that represents attributes identifiers to listen for changes
;   can be empty. in this case change of any attribute caused by call of SetString will be notified
;   if is not empty then calls of SetString will be called for these attributes only
;   duplications are ignored
;   it is possible that some attributes are disappeared between saves when esp/esl/esm was uninstalled
;   so attributes shall not be changed and as result - event handler will not be invoked
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
; NOTE: passing handler=None will cause crash
bool function RegisterForAttributesChange(ScriptObject handler, Actor[] actors, Keyword[] attributes) native global

; Unsubscribe from string attributes changes
; Script will not receive any notifications about attributes changes
; returns true if unsubscription was successful
; returns false if handler is not registered
; NOTE: passing None will cause crash
bool function UnregisterForAttributesChange(ScriptObject handler) native global

; Checks that handler of string attributes changes is registered
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