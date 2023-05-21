Scriptname DS:Debug Native Hidden Const

; toggles log trace messages to game console in addition to log file %userprofile%\Documents\My Games\Fallout4\F4SE\DS.log
; you could call this function from game console:
; cgf "DS:Debug.LogToConsole" true
function LogToConsole(bool enable) native global

; set log level. default level is warning (2)
; 0 - No logs
; 1 - only errors
; 2 - warnings and errors
; 3 - information messages, warnings and errors
; 4 - debug messages, information messages, warnings and errors
; you could call this function from game console:
; cgf "DS:Debug.SetLogLevel" 4
function SetLogLevel(int level) native global

; Dumps internal state to log file and to console if LogToConsole was called previously
; you could call this function from game console:
; cgf "DS:Debug.Dump"
function Dump() native global

; adds message to log
; and to console if log to console is enabled
function Print(string message) native global
