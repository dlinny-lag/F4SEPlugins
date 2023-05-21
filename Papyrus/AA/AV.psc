Scriptname AA:AV Native Hidden Const

; Registers ActorValue that need to be managed by Additional Attributes
; Should be called by script in plugin that has this ActorValue definition
; NOTE: actually this method could be called for ActorValues defined in any mod, even in Fallout4.esm
; but be ready that another mod already registered this ActorValue
; 
; attribute - ActorValue to register. 
; defaultValue - default value for ActorValue when actor is initilized first time. Must be >= minValue and <= defaultMax
; minValue - minimum allowed value for attribute. Must be < defaultMax
; defaultMax - default base value when actor is initilized first time. it is interpreted as MAX for attribute.
; randomMin - low border of attribute value when value is randomly generated using uniform distribution. must be <= randomMax and >=minValue and <= defaultMax
; randomMax - high border of attribute value when value is randomly generated using uniform distribution. must be >= randomMin and >=minValue and <= defaultMax
; randomMean - mean value when value is randomly generated using normal distribution. it is a good idea to set it to be in range [minValue, defaultMax]
; randomDeviation - deviation when value is randomly generated using normal distribution. must be non-negative. zero deviation will cause to use mean value always. 
; returns true if registration was successful
; returns false in following cases:
;  - arguments are invalid 
;  - ActorValue is marked as Hardcoded
;  - ActorValue is marked as DamageIsPositive
;  - ActorValue is already registered 
; See also ActorManager script for more details
bool function RegisterAttribute(ActorValue attribute, float defaultValue, float minValue, float defaultMax, float randomMin, float randomMax, float randomMean, float randomDeviation) native global

; Updates attribute registration parameters. Attribute values in actors will not be touched
; see details about arguments above
; returns true on success
; returns false in following cases:
;  - arguments are invalid
;  - ActorValue is marked as Hardcoded
;  - ActorValue is marked as DamageIsPositive
;  - ActorValue is not registered 
bool function UpdateAttributeRegistration(ActorValue attribute, float defaultValue, float randomMin, float randomMax, float randomMean, float randomDeviation) native global

; Unregisters previoulsy regestered ActorValue
; Could be usefull after your mod upgrading or for replacing attribute registration made by another mod
; Be carefull replacing attributes registered by another mods
; returns true if ActorValue was sucessfully unregistered
; returns false if attribute is None or ActorValue was not previously registered
bool function UnregisterAttribute(ActorValue attribute) native global

; Checks that ActorValue was registered
; returns true if ActorValue is registered
; returns false if attribute is None or ActorValue is not registered
bool function IsAttributeRegistered(ActorValue attribute) native global

; Increases attribute's value by delta
; sender - any Form that will appear in change notification. can be None
; actor - actor what attribute is changing
; attribute - attribute to change. can be any, except 'Damage is Positive' flagged
; delta - amount of change. must be greater than 0
; mode - initilization mode if attribute was never defined for actor. ignored for not registered attributes
;   0 - default mode. initializes value with defaultValue previously passed to RegisterAttribute
;       it is possible that value was assigned before first call of this method, by ObjectReference.SetValue(), for example
;       or was defined in ESP/ESL/ESM.
;       in this case assigned value will be used to set initial value of attribute instead of defaultValue
;   1 - uniform distribution random mode. assigns some random value between randomMin and randomMax previously passed to RegisterAttribute
;   2 - normal distribution random mode. generated value capped by [minValue, defaultMax] interval
; returns true if value changed, false - otherwise
; false will be returned if
;  - arguments are invalid
;  - delta is 0 or less
;  - attribute has flag 'Damage is Positive'
bool function Increase(Form sender, Actor actor, ActorValue attribute, float delta, int mode) native global

; Increases attribute's value by delta, but not exceeding maximum. exceed in change notification will be 0
; sender - any Form that will appear in change notification. can be None
; actor - actor what attribute is changing
; attribute - attribute to change. can be any, except 'Damage is Positive' flagged
; delta - amount of change. must be greater than 0
; mode - initilization mode if attribute was never defined for actor. ignored for not registered attributes
;   0 - default mode. initializes value with defaultValue previously passed to RegisterAttribute
;       it is possible that value was assigned before first call of this method, by ObjectReference.SetValue(), for example
;       or was defined in ESP/ESL/ESM.
;       in this case assigned value will be used to set initial value of attribute instead of defaultValue
;   1 - uniform distribution random mode. assigns some random value between randomMin and randomMax previously passed to RegisterAttribute
;   2 - normal distribution random mode. generated value capped by [minValue, defaultMax] interval
; returns true if value changed, false - otherwise
; false will be returned if
;  - arguments are invalid
;  - delta is 0 or less
;  - attribute has flag 'Damage is Positive'
bool function IncreaseCapped(Form sender, Actor actor, ActorValue attribute, float delta, int mode) native global

; Decreases attribute's value by delta
; sender - any Form that will appear in change notification. can be None
; actor - actor what attribute is changing
; attribute - attribute to change. can be any, except 'Damage is Positive' flagged
; delta - amount of change. must be greater than 0
; mode - initilization mode if attribute was never defined for actor. ignored for not registered attributes
;   0 - default mode. initializes value with defaultValue previously passed to RegisterAttribute
;       it is possible that value was assigned before first call of this method, by ObjectReference.SetValue(), for example
;       or was defined in ESP/ESL/ESM.
;       in this case assigned value will be used to set initial value of attribute instead of defaultValue
;   1 - uniform distribution random mode. assigns some random value between randomMin and randomMax previously passed to RegisterAttribute
;   2 - normal distribution random mode. generated value capped by [minValue, defaultMax] interval
; returns true if value changed, false - otherwise
; false will be returned if
;  - arguments are invalid
;  - delta is 0 or less
;  - attribute has flag 'Damage is Positive'
bool function Decrease(Form sender, Actor actor, ActorValue attribute, float delta, int mode) native global

; Decreases attribute's value by delta, but not exceeding minimum. exceed in change notification will be 0
; sender - any Form that will appear in change notification. can be None
; actor - actor what attribute is changing
; attribute - attribute to change. can be any, except 'Damage is Positive' flagged
; delta - amount of change. must be greater than 0
; mode - initilization mode if attribute was never defined for actor. ignored for not registered attributes
;   0 - default mode. initializes value with defaultValue previously passed to RegisterAttribute
;       it is possible that value was assigned before first call of this method, by ObjectReference.SetValue(), for example
;       or was defined in ESP/ESL/ESM.
;       in this case assigned value will be used to set initial value of attribute instead of defaultValue
;   1 - uniform distribution random mode. assigns some random value between randomMin and randomMax previously passed to RegisterAttribute
;   2 - normal distribution random mode. generated value capped by [minValue, defaultMax] interval
; returns true if value changed, false - otherwise
; false will be returned if
;  - arguments are invalid
;  - delta is 0 or less
;  - attribute has flag 'Damage is Positive'
bool function DecreaseCapped(Form sender, Actor actor, ActorValue attribute, float delta, int mode) native global

; Modifies attribute's value by delta
; sender - any Form that will appear in change notification. can be None
; actor - actor what attribute is changing
; attribute - attribute to change. can be any, except 'Damage is Positive' flagged
; delta - amount of change. Positive values will cause Increasing, Negative - decreasing. With appropriate notification
; mode - initilization mode if attribute was never defined for actor. ignored for not registered attributes
;   0 - default mode. initializes value with defaultValue previously passed to RegisterAttribute
;       it is possible that value was assigned before first call of this method, by ObjectReference.SetValue(), for example
;       or was defined in ESP/ESL/ESM.
;       in this case assigned value will be used to set initial value of attribute instead of defaultValue
;   1 - uniform distribution random mode. assigns some random value between randomMin and randomMax previously passed to RegisterAttribute
;   2 - normal distribution random mode. generated value capped by [minValue, defaultMax] interval
; returns true if value changed, false - otherwise
; false will be returned if
;  - arguments are invalid
;  - delta is 0
;  - attribute has flag 'Damage is Positive'
bool function Modify(Form sender, Actor actor, ActorValue attribute, float delta, int mode) native global

; Modifies attribute's value by delta, but not exceeding maximum or minimum. exceed in change notification will be 0
; sender - any Form that will appear in change notification. can be None
; actor - actor what attribute is changing
; attribute - attribute to change. can be any, except 'Damage is Positive' flagged
; delta - amount of change. Positive values will cause Increasing, Negative - decreasing. With appropriate notification
; mode - initilization mode if attribute was never defined for actor. ignored for not registered attributes
;   0 - default mode. initializes value with defaultValue previously passed to RegisterAttribute
;       it is possible that value was assigned before first call of this method, by ObjectReference.SetValue(), for example
;       or was defined in ESP/ESL/ESM.
;       in this case assigned value will be used to set initial value of attribute instead of defaultValue
;   1 - uniform distribution random mode. assigns some random value between randomMin and randomMax previously passed to RegisterAttribute
;   2 - normal distribution random mode. generated value capped by [minValue, defaultMax] interval
; returns true if value changed, false - otherwise
; false will be returned if
;  - arguments are invalid
;  - delta is 0
;  - attribute has flag 'Damage is Positive'
bool function ModifyCapped(Form sender, Actor actor, ActorValue attribute, float delta, int mode) native global

; Modifies attributes values by corresponding deltas
; sender - any Form that will appear in change notification. can be None
; actor - actor what attribute is changing 
; attributes - array of attributes to change. can be any, except 'Damage is Positive' flagged
; deltas - array of changes. Positive values will cause Increasing, Negative - decreasing. With appropriate notification
; mode - initilization mode if attribute was never defined for actor. ignored for not registered attributes
;   0 - default mode. initializes value with defaultValue previously passed to RegisterAttribute
;       it is possible that value was assigned before first call of this method, by ObjectReference.SetValue(), for example
;       or was defined in ESP/ESL/ESM.
;       in this case assigned value will be used to set initial value of attribute instead of defaultValue
;   1 - uniform distribution random mode. assigns some random value between randomMin and randomMax previously passed to RegisterAttribute
;   2 - normal distribution random mode. generated value capped by [minValue, defaultMax] interval
; returns false if arguments are invalid, true - otherwise
;
; None or invalid attributes are silently ignored
; if length of deltas in less than length of attributes then rest attributes will remain unchanged
bool function ModifyValues(Form sender, Actor actor, ActorValue[] attributes, float[] deltas, int mode) native global

; Modifies attributes values by corresponding deltas, but not exceeding maximum or minimum. exceed in change notification will be 0
; sender - any Form that will appear in change notification. can be None
; actor - actor what attribute is changing 
; attributes - array of attributes to change. can be any, except 'Damage is Positive' flagged
; deltas - array of changes. Positive values will cause Increasing, Negative - decreasing. With appropriate notification
; mode - initilization mode if attribute was never defined for actor. ignored for not registered attributes
;   0 - default mode. initializes value with defaultValue previously passed to RegisterAttribute
;       it is possible that value was assigned before first call of this method, by ObjectReference.SetValue(), for example
;       or was defined in ESP/ESL/ESM.
;       in this case assigned value will be used to set initial value of attribute instead of defaultValue
;   1 - uniform distribution random mode. assigns some random value between randomMin and randomMax previously passed to RegisterAttribute
;   2 - normal distribution random mode. generated value capped by [minValue, defaultMax] interval
; returns false if arguments are invalid, true - otherwise
;
; None or invalid attributes are silently ignored
; if length of deltas in less than length of attributes then rest attributes will remain unchanged
bool function ModifyValuesCapped(Form sender, Actor actor, ActorValue[] attributes, float[] deltas, int mode) native global

; Returns attribute's value
; actor - the actor to get the attribute value for
; attribute - what atttribute value to get. can be any except 'Damage is Positive' flagged
; mode - initilization mode if attribute was never defined for actor. ignored for not registered attributes
;   0 - default mode. initializes value with defaultValue previously passed to RegisterAttribute
;       it is possible that value was assigned before first call of this method, by ObjectReference.SetValue(), for example
;       or was defined in ESP/ESL/ESM.
;       in this case assigned value will be used to set initial value of attribute instead of defaultValue
;   1 - uniform distribution random mode. assigns some random value between randomMin and randomMax previously passed to RegisterAttribute
;   2 - normal distribution random mode. generated value capped by [minValue, defaultMax] interval
; if arguments are invalid then 0 will be returned
float function Get(Actor actor, ActorValue attribute, int mode) native global

; Returns attribute's max limit. 
; actor - the actor to get the attribute value for
; attribute - what attribute maximum to get. can be any except 'Damage is Positive' flagged
; if arguments are invalid then 0 will be returned
; if attribute's value is not set then
; - returns defaultMax for registered attributes
; - returns default value for unregistered attributes
; for registered attributes return defaultMax + PeakValueModifier
; for unregistered attributes engine's function is used to calculate max
float function GetMax(Actor actor, ActorValue attribute) native global

; Returns values of passed array of attributes
; actor - the actor to get the values for
; attributes - what atttributes values to get. may contain any except 'Damage is Positive' flagged
; mode - initilization mode if attribute was never defined for actor. ignored for not registered attributes
;   0 - default mode. initializes value with defaultValue previously passed to RegisterAttribute
;       it is possible that value was assigned before first call of this method, by ObjectReference.SetValue(), for example
;       or was defined in ESP/ESL/ESM.
;       in this case assigned value will be used to set initial value of attribute instead of defaultValue
;   1 - uniform distribution random mode. assigns some random value between randomMin and randomMax previously passed to RegisterAttribute
;   2 - normal distribution random mode. generated value capped by [minValue, defaultMax] interval
; if some attribute is invalid then 0 will be assigned
float[] function GetValues(Actor actor, ActorValue[] attributes, int mode) native global

; Returns max limits for passed array of attributes. 
; actor - the actor to get the attribute value for
; attributes - what attributes maximums to get. can be any except 'Damage is Positive' flagged
; if arguments are invalid then 0 will be returned
; if attribute's value is not set then
; - returns defaultMax for registered attributes
; - returns default value for unregistered attributes
; for registered attributes return defaultMax + PeakValueModifier
; for unregistered attributes engine's function is used to calculate max
float[] function GetMaximums(Actor actor, ActorValue[] attributes) native global

; Assigns certain value to attribute
; sender - any Form that will appear in change notification. can be None
; actor - actor what attribute is changing
; attribute - attribute to change. can be any, except 'Damage is Positive' flagged
; value - desired value. appropriate notification will be sent depending on change
; mode - initilization mode if attribute was never defined for actor. ignored for not registered attributes
;   0 - default mode. initializes value with defaultValue previously passed to RegisterAttribute
;       it is possible that value was assigned before first call of this method, by ObjectReference.SetValue(), for example
;       or was defined in ESP/ESL/ESM.
;       in this case assigned value will be used to set initial value of attribute instead of defaultValue
;   1 - uniform distribution random mode. assigns some random value between randomMin and randomMax previously passed to RegisterAttribute
;   2 - normal distribution random mode. generated value capped by [minValue, defaultMax] interval
; returns true if value changed, false - otherwise
; false will be returned if
;  - arguments are invalid
;  - value is exactly equal to existing attribute value
;  - attribute has flag 'Damage is Positive'
bool function Set(Form sender, Actor actor, ActorValue attribute, float value, int mode) native global

; Assigns attributes to corresponding values
; sender - any Form that will appear in change notification. can be None
; actor - actor what attributes are changing 
; attributes - array of attributes to change. can be any, except 'Damage is Positive' flagged
; values - array of values to set. appropriate notifications will be sent depending on change
; mode - initilization mode if attribute was never defined for actor. ignored for not registered attributes
;   0 - default mode. initializes value with defaultValue previously passed to RegisterAttribute
;       it is possible that value was assigned before first call of this method, by ObjectReference.SetValue(), for example
;       or was defined in ESP/ESL/ESM.
;       in this case assigned value will be used to set initial value of attribute instead of defaultValue
;   1 - uniform distribution random mode. assigns some random value between randomMin and randomMax previously passed to RegisterAttribute
;   2 - normal distribution random mode. generated value capped by [minValue, defaultMax] interval
; returns false if arguments are invalid, true - otherwise
;
; None or invalid attributes are silently ignored
; if length of values is less than length of attributes then rest attributes will remain unchanged
bool function SetValues(Form sender, Actor actor, ActorValue[] attributes, float[] values, int mode) native global

; returns unique positive integer number for actor
; this number ramain same until new game is not started
; return 0 if argument is invalid
; all returning values are greater than 10000
int function GetUniqueId (Actor actor) native global

Struct Parameters
    ; Random value generation mode: 
    ; 0 - no generation, default value will be used
    ; 1 - uniform distribution
    ; 2 - normal distibution
    int Mode 
    float Min ; min value for uniform distribution. ignored if Mode != 1
    float Max ; max value for uniform distribution. ignored if Mode != 1
    float Mean ; mean value for normal distribution. ignored if Mode != 2
    float Deviation; deviation value for normal distribution. negative value interpreted as failure or as mode = 0. ignored if Mode != 2
EndStruct

; Assigns attribute to random value. appropriate notification will be sent depending on change
; sender - any Form that will appear in change notification. can be None
; actor - actor what attribute is changing
; attribute - attribute to change. attribute must be registered. 
; param - random value generation parameters. Must not be None
; if registered attribute is not initialized yet, then it will be initialized with default mode first (see mode = 0 above)
;
; returns false if some arguments are invalid, true - otherwise
;
; it is possbible that new random value is exacly same to already assigned value. in this case notification will not be sent
bool function SetRandom(Form sender, Actor actor, ActorValue attribute, Parameters param) native global

; Assigns attributes to corresponding random values. appropriate notifications will be sent depending on change
; sender - any Form that will appear in change notification. can be None
; actor - actor what attribute is changing
; attributes - array of attributes to change. attributes must be registered. invalid or unregistered attributes are silently ignored
; params - array of random value generation parameters. None values are silently ignored
;
; returns false if some arguments are invalid, true - otherwise
;
; if length of params is less than length of attributes then rest attributes will remain unchanged
; for normal distribution generated value can exceed attribute limits. in this case exceed parameter in notification will be non zero
bool function SetRandomValues(Form sender, Actor actor, ActorValue[] attributes, Parameters[] params) native global

; Generates array of random values corresponding to passed attributes and params
; attributes - attributes to generate random values for. 
; params - array of random value generation parameters. 
; returning array have same size as attributes array
;
; if some attribute is None then 0 will appear in returning array
; if some attribute is not registered then ActorValue's default value will appear in returning array
; if params length is less than attributes length then default value will appear in returning array
; if some param is None then default value will appear in returning array
; 
; returns None if attributes or params is None
; if length of params is less than length of attributes then default value will appear for rest attributes
float[] function GenerateRandomValues(ActorValue[] attributes, Parameters[] params) native global


; Ensures that each attribute in passed array is initalized with initial value. exact value depends on initilization mode and registered attribute parameters
;  actor - actor what attributes need to be initilized
;  attributes - array of attributes that need to be initialized
;  modes - array of initialization modes. should have same size as attributes array. accepted element values: [-1, 2]
;    0 - default mode. initializes value with defaultValue previously passed to RegisterAttribute
;       it is possible that value was assigned before first call of this method, by ObjectReference.SetValue(), for example
;       or was defined in ESP/ESL/ESM.
;       in this case assigned value will be used to set initial value of attribute instead of defaultValue
;    1 - uniform distribution random mode. assigns some random value between randomMin and randomMax previously passed to RegisterAttribute
;    2 - normal distribution random mode. generated value capped by [minValue, defaultMax] interval
;   -1 - initializes attribute with corresponding value passed in values array.
;  exactValues - array of exact values for initialization. can be None if modes array doesn't contain -1 element(s)
;    element from this array will be used for corresponding attribute if corresponding mode is -1. rest elements from this array are ignored.
;
; invalid arguments are silently ignored
; if corresponding attribute is not registered or initialized already, then call has no effect for this attribute
; if modes array size is less than attributes array then rest attributes are ignored
; if some attribute has -1 mode and exactValues array hasn't element for it (is not large enough) then attribute will be initialized with 0 (default) mode
; if exact value does not fall to registered attribute min/max range then it will be capped by min/max
; if mode for corresponding attribute is -1 and this attribute have value defined in ESM/ESL/ESP then defined value is ignored and passed exact value is used instead
function EnsureInitialized(Actor actor, ActorValue[] attributes, int[] modes, float[] exactValues) native global