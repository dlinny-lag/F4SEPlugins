Scriptname AA:Internal Native Hidden Const


; methods below designed for internal usage
; don't call them directly in your script
; calls at inappropriate moment might cause unexpected behavior, CTD and save files damage
; functions and their signature can be changed without notice

bool function __RegisterCalculator(ScriptObject calculator, ActorValue[] attributes) native global
bool function __UnregisterCalculator(ScriptObject calculator) native global
ScriptObject[] function __GetCalculators() native global
bool function __IsCalculatorRegistered(ScriptObject calculator) native global
int function __StartCalculationSession(Actor actor, int mode) native global
float[] function __GetCalculatorValues(int sessionId, ScriptObject calculator) native global
function __UpdateCalculatorValues(int sessionId, ScriptObject calculator, float[] calculatedValues) native global
function __CloseCalculationSession(Form[] senders, int sessionId) native global