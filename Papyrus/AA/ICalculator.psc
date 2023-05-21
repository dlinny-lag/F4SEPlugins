Scriptname AA:ICalculator extends Quest

; a - actor for what we calculate changes
; elapsed - time elapsed since last calculation, in hours
; values - ordered list of value of corresponding attributes. change these values in your handler
;          order is same to order of attributes passed to API.RegisterCalculator
; should returns Keyword associated with this calculator if something was changed
;   returning keyword could be None if calculator won't change values, or just don't want to return anything
;   returning Keyword will appear as argument in OnIncreased/OnDecreased calls
Keyword function HandleElapsedTime(Actor a, float elapsed, float[] values)
    Debug.Trace("AA:ICalculator:HandleElapsedTime: Abstract method call")
    return None
endfunction