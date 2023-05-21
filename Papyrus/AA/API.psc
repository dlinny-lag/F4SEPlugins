Scriptname AA:API extends Quest

import AA:Internal

; service properties
ActorValue Property AA_InitializationType Auto Const
ActorValue Property AA_Managed Auto Const
SPELL Property AA_ManageActorSpell Auto Const
GlobalVariable Property AA_UpdateTimeout Auto Const
ActorValue Property AA_LastTimeHandled Auto Const

; same to ActorManager.psc. avoid cross-script locks
int alreadyInitialized = 100 const
int Unmanaged = 0 const
int Managed = 1 const
int Pending = 2 const
int Failed = 3 const


event OnInit()
    RegisterForRemoteEvent(Game.GetPlayer(), "OnPlayerLoadGame")
EndEvent
event Actor.OnPlayerLoadGame(Actor a)

endevent


; Increase attribute's value by delta for certain actor
;  sender - any Form that will appear in OnIncrease notification. can be None
;  actor - actor what attribute is changing
;  attribute - attribute to change. can be any, except 'Damage is Positive' flagged
;  delta - amount of change. must be greater than 0
; Note: calls RestoreValue under the hood.
; You can pass any attribute here but for unregistered attributes 
; 'exceed' argument in notification will always be 0
; even if new value could exceed base value
; returns true if value changed, false - otherwise
; false will be returned if
;  - arguments are invalid
;  - delta is 0 or less
;  - attribute has flag 'Damage is Positive'
bool function Increase(Form sender, Actor a, ActorValue attribute, float delta)
    EnableTimeChanges(a)
    return AA:AV.Increase(sender, a, attribute, delta, (a.GetValue(AA_InitializationType) as int) % alreadyInitialized)
endfunction

; Decrease attribute's value by delta for certain actor
;  sender - any Form that will appear in OnDecrease notification. can be None
;  actor - actor what attribute is changing
;  attribute - attribute to change. can be any, except 'Damage is Positive' flagged
;  delta - amount of change. must be greater than 0
; Note: calls DamageValue under the hood.
; You can pass any attribute here but for unregistered attributes 
; 'exceed' argument in notification will always be 0
; No bounds checks is performed for unregistered attributes
; returns true if value changed, false - otherwise
; false will be returned if
;  - arguments are invalid
;  - delta is 0 or less
;  - attribute has flag 'Damage is Positive'
bool function Decrease(Form sender, Actor a, ActorValue attribute, float delta)
    EnableTimeChanges(a)
    return AA:AV.Decrease(sender, a, attribute, delta, (a.GetValue(AA_InitializationType) as int) % alreadyInitialized)
endfunction

; Assigns certain value to attribute
;  sender - any Form that will appear in change notification. can be None
;  actor - actor what attribute is changing
;  attribute - attribute to change. can be any, except 'Damage is Positive' flagged
;  value - desired value. appropriate notification will be sent depending on change
; You can pass any attribute here but for unregistered attributes 
; 'exceed' argument in notification will always be 0
; No bounds checks is performed for unregistered attributes
; returns true if value changed, false - otherwise
; false will be returned if
;  - arguments are invalid
;  - value is exactly equal to existing attribute value
;  - attribute has flag 'Damage is Positive'
bool function Set(Form sender, Actor a, ActorValue attribute, float value)
    EnableTimeChanges(a)
    return AA:AV.Set(sender, a, attribute, value, (a.GetValue(AA_InitializationType) as int) % alreadyInitialized)
endfunction

; Returns attribute's value for certain actor
; attribute - what atttribute value to get. can be any except 'Damage is Positive' flagged
; if arguments are invalid 0 will be returned
float function Get(Actor a, ActorValue attribute)
    EnableTimeChanges(a)
    return AA:AV.Get(a, attribute, (a.GetValue(AA_InitializationType) as int) % alreadyInitialized)
endfunction

int maxAttempts = 1000 const
int retestAfter = 50 const
int function __WaitForManagedStateEstablishing(Actor a)
    int managedState = a.GetValue(AA_Managed) as int
    int retest = 1
    while managedState == Pending
        if retest > maxAttempts
            Debug.Trace("Managed state establishing for actor " + a + " is timed out")
            return Failed ; something went wrong
        endif
        if retest % retestAfter == 0
            ; we spinned 50 cycles... maybe actor became invalid already?
            if a.IsDead() || a.IsDeleted() || a.IsDestroyed() || a.IsDisabled()
                Debug.Trace("" + a + " became invalid while waiting for managed state establishing. Spinned " + retest + " times")
                return Failed
            endif
        else
            ; wait for initialization complete
            Utility.WaitMenuMode(0.05)
            managedState = a.GetValue(AA_Managed) as int
        endif
        retest += 1
    endwhile
    return managedState
endfunction

; returns true if time-based changes was enabled on actor
; return false if time-based changes can not be applied to actor
; NOTE: MUST NOT be called from OnInit. Will cause deadlock
bool function EnableTimeChanges(Actor a)
    if !a || a.IsDead() || a.IsDeleted() || a.IsDestroyed() || a.IsDisabled(); || !a.Is3DLoaded()
        Debug.Trace("AA:API:EnsureActorManaged: wrong actor state. [" + a + "]")
        return false
    endif

    int managedState = a.GetValue(AA_Managed) as int
    if managedState == Managed
        return true
    endif

    if !IsActorValid(a)
        Debug.Trace("AA:API:EnsureActorManaged: Actor is not allowed [" + a + "]")
        return false
    endif

    managedState = a.GetValue(AA_Managed) as int
    if managedState == Unmanaged
        a.SetValue(AA_Managed, Pending)
        managedState = Pending
    else ; Pending? 
        managedState = __WaitForManagedStateEstablishing(a)
        ; probably state change was initiated by another process
        ; initialization is finished here
        if managedState == Failed
            a.RemoveSpell(AA_ManageActorSpell)
            a.SetValue(AA_Managed, Unmanaged)
            return false
        elseif managedState == Managed
            return true
        endif
    endif

    ; state should be Pending here
    if managedState != Pending
        if managedState != Unmanaged
            Debug.Trace("Unexpected state! " + managedState + " for actor " + a, 2) ; should never happen
        endif
        ; probably state is Unmanaged, so it is OK to return false here
        return false
    endif

    bool spellAdded = a.AddSpell(AA_ManageActorSpell, false)
    if !spellAdded ; spell was added previously but magic effect script is not running
        bool removed = a.RemoveSpell(AA_ManageActorSpell)
        if !removed
            Debug.Trace("Failed to remove spell! " + a, 2)
        endif
        ; fail this attempt
        ; even if we will try to add spell right now
        ; MagicEffect script will be run as unbound. don't know why
        ; probably next time attempt to manage this actor will be successful
        a.SetValue(AA_Managed, Unmanaged)
        Debug.Trace("Failed to add spell for actor " + a)
        return false
    endif

    managedState = __WaitForManagedStateEstablishing(a)

    if managedState == Failed
        a.RemoveSpell(AA_ManageActorSpell)
        a.SetValue(AA_Managed, Unmanaged)
        return false
    elseif managedState == Managed
        return true
    endif

     ; TODO: unhandled race condition?
    Debug.Trace("EnsureActorManaged: Failed to initialize [" + a + "]")
    a.RemoveSpell(AA_ManageActorSpell) ; no trust
    a.SetValue(AA_Managed, Unmanaged) ; recover state to initial
    return false
Endfunction



;  -------------
;  Calculators
;  -------------


; Registers calculator script
; Calculator script must be derived from AA:ICalculator 
; Call this function from your OnInit method
; an example:
; Scriptname MyMegaCalculator extends AA:ICalculator
; event OnInit()
;   API.RegisterCalculator(self, MyAttributes)
; endevent
;
; calculator - reference to calculator script
; processingAttributes - list of registered attributes that will be calculated by this calculator
;   calculator will receive attribute values in same order as processingAttributes
;
; returns true if script was registered
; returns false if script is invalid or already registerd
; false will be returned if 128 array size limit is reached
; so max amount of calculators is 128
bool function RegisterCalculator(AA:ICalculator calculator, ActorValue[] processingAttributes)
    return __RegisterCalculator(calculator, processingAttributes)
endfunction

bool function IsCalculatorRegistered(AA:ICalculator calculator)
    return __IsCalculatorRegistered(calculator)
endfunction

; Could be usefull if you want explicitly uninstall your plugin
; Unregistering is not necessary
bool function UnregisterCalculator(AA:ICalculator calculator)
    return __UnregisterCalculator(calculator)
endfunction

;  -------------
;  Actor filters
;  -------------

AA:IActorFilter[] filters
; Registers actor filter that can define what actors can be managed by AA
; additional actor filter can add some actor(s) or actor type(s) to managed actors list
; so if any actor filter allow some actor then this actor is allowed to be managed
bool function RegisterActorFilter(AA:IActorFilter filter)
    if filter == None
        return false
    endif
    if filters == None
        filters = new AA:IActorFilter[1]
        filters[0] = filter
        return true
    endif
    __ValidateActorFilters()
    int i = 0
    while i < filters.Length
        if filters[i] == filter
            return false
        endif
        i += 1
    endwhile

    filters.Add(filter)
    return true
endfunction

bool function IsActorFilterRegistered(AA:IActorFilter filter)
    if !filters
        return false
    endif
    int i = 0
    while i < filters.Length
        if filters[i] == filter
            return true
        endif
        i += 1
    endwhile
    return false
endfunction

function __ValidateActorFilters()
    if filters == None
        filters = new AA:IActorFilter[0]
        return
    endif

    int i = 0
    while i < filters.Length
        if filters[i] == None
            filters.Remove(i)
        else
            i += 1    
        endif
    endwhile
endfunction

bool function IsActorValid(Actor a)
    __ValidateActorFilters()
    int i = 0
    while i < filters.Length
        if filters[i].IsActorValid(a)
            return true
        endif
        i += 1
    endwhile
    return false
endfunction

;
; Actor a = args[0] as Actor
CustomEvent OnActorInitilized
function __FireActorInitilized(Actor a)
    var[] args = new var[1]
    args[0] = a
    SendCustomEvent("OnActorInitilized", args)
endfunction

;
; Actor a = args[0] as Actor
CustomEvent OnActorManaged
function __FireActorManaged(Actor a)
    var[] args = new var[1]
    args[0] = a
    SendCustomEvent("OnActorManaged", args)
endfunction

;
; Actor a = args[0] as Actor
CustomEvent OnActorUnmanaged
function __FireActorUnmanaged(Actor a)
    var[] args = new var[1]
    args[0] = a
    SendCustomEvent("OnActorUnmanaged", args)
endfunction

;
; Actor a = args[0] as Actor
CustomEvent OnCalculationsFinished
function __FireCalculationFinished(Actor a)
    var[] args = new var[1]
    args[0] = a
    SendCustomEvent("OnCalculationsFinished", args)
endfunction

; returns parameters object initialized for random generation with uniform distribution
; see SetRandom/SetRandomValues/GenerateRandomValues in AA:AV script
AA:AV:Parameters function Uniform(float min, float max) global
    AA:AV:Parameters p = new AA:AV:Parameters
    p.Mode = 1
    p.Min = min
    p.Max = max
    return p
endfunction
; returns parameters object initialized for random generation with normal distribution
; see SetRandom/SetRandomValues/GenerateRandomValues in AA:AV script
AA:AV:Parameters function Normal(float mean, float deviatin) global
    AA:AV:Parameters p = new AA:AV:Parameters
    p.Mode = 2
    p.Mean = mean
    p.Deviation = deviatin
    return p
endfunction