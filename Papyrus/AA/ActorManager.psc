Scriptname AA:ActorManager extends ActiveMagicEffect

ActorValue Property AA_InitializationType Auto Const
ActorValue Property AA_Managed Auto Const
ActorValue Property AA_LastTimeHandled Auto Const

AA:API Property API Auto Const
GlobalVariable Property AA_UpdateTimeout Auto Const
import AA:Internal

; same to API.psc. avoid cross-script locks
int alreadyInitialized = 100 const
int Unmanaged = 0 const
int Managed = 1 const
int Pending = 2 const
int Failed = 3 const

int timer = 1 const
Actor myActor

Event OnEffectStart(Actor akTarget, Actor akCaster)
    myActor = akTarget
    if !AA_InitializationType 
        ; fatal: script disposed or not initialized properly
        Debug.Trace("ActorManager: no AA_InitializationType. Actor=" + myActor)
        if  myActor
            ActorValue managedAV = Game.GetFormFromFile(0x00000800, "AA.esm") as ActorValue
            myActor.SetValue(managedAV, Failed) ; unlock
        endif
        return
    endif

    if myActor.IsDead()
        myActor.SetValue(AA_Managed, Failed) ; unlock
        Debug.Trace("ActorManager: " + myActor + " is dead already") 
        return
    endif

    int initType = myActor.GetValue(AA_InitializationType) as int
    UnregisterForRemoteEvent(myActor, "OnUnload")
    bool registered = RegisterForRemoteEvent(myActor, "OnUnload")
    if !registered ; indicator that script is unbound
        ActorValue managedAV = Game.GetFormFromFile(0x00000800, "AA.esm") as ActorValue
        myActor.SetValue(managedAV, Failed) ; unlock
        Debug.Trace("ActorManager: Failed to register OnUnload for actor " + myActor) 
        return
    endif
    myActor.SetValue(AA_Managed, Managed) ; unlock

    if initType >= alreadyInitialized ; spell was re-casted
        ; do not process actor immediately
        ; some external script might want to be subscribed for changes
        ; when actor becomes managed
        API.__FireActorManaged(myActor)
        RestartTimer()
    else ; first time start
        myActor.SetValue(AA_InitializationType, alreadyInitialized+initType) ; prevent any re-initializations when spell will be re-casted
        myActor.SetValue(AA_LastTimeHandled, Utility.GetCurrentGameTime())
        API.__FireActorInitilized(myActor)
        RestartTimer()
    endif
    Debug.Trace("ActorManager for " + myActor + ": started" )
Endevent

Event ObjectReference.OnUnload(ObjectReference sender)
    Debug.Trace("ActorManager for " + myActor + ": dispel due to unload" )
    Dispel()
Endevent

function RestartTimer()
    if !AA_UpdateTimeout
        ; fatal: AA.esm lost or script is unbound
        Dispel()
        return
    endif
    float timeout = AA_UpdateTimeout.GetValue()
    StartTimerGameTime(timeout, timer)
endfunction


Event OnTimerGameTime(int aiTimerID)
    if timer == aiTimerID
        HandleElapsedTime()
    endif
EndEvent


Event OnEffectFinish(Actor akTarget, Actor akCaster)
    myActor.SetValue(AA_Managed, Unmanaged)
    Debug.Trace("ActorManager for " + myActor + ": stopped" )
    API.__FireActorUnmanaged(myActor) ; TODO: ensure that API is not None here
Endevent


function HandleElapsedTime()
    if !AA_LastTimeHandled
        ; fatal: AA.esm lost or script is unbound
        return
    endif
    float lastUpdateTime = myActor.GetValue(AA_LastTimeHandled)
    float currentTime = Utility.GetCurrentGameTime()
    ScriptObject[] calcs = __GetCalculators()
    float diffInHours = (currentTime - lastUpdateTime) * 24.0
    Debug.Trace("Elapsed " + diffInHours + " hours on " + myActor + ". calcs = " + calcs)
    int session = __StartCalculationSession(myActor, (myActor.GetValue(AA_InitializationType) as int) % alreadyInitialized)
    int i = 0
    Form[] senders = new Form[0]
    while i < calcs.Length
        AA:ICalculator calc = calcs[i] as AA:ICalculator
        float[] calcValues = __GetCalculatorValues(session, calc)
        ; it is possible that calculator/attributes disappeared when some plugin disabled between save/loads
        ; so just to be sure - check values again inside loop
        if calc && calcValues 
            Keyword sender = calc.HandleElapsedTime(myActor, diffInHours, calcValues)
            if sender
                senders.Add(sender)
            endif
            __UpdateCalculatorValues(session, calc, calcValues)
        else
            Debug.Trace("ActorManager:" + myActor + ": Calc = " + calc + ", Values " + calcValues)
        endif
        i += 1
    endwhile
    myActor.SetValue(AA_LastTimeHandled, currentTime)
    __CloseCalculationSession(senders, session)

    API.__FireCalculationFinished(myActor)
    RestartTimer()
endfunction

