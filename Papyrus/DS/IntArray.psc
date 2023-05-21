Scriptname DS:IntArray Native Hidden Const

; Creates array for specified identifier
; identifier is the object id that must be used in subsequent calls of functions from this script
; returns true on success
; returns false if identifier is invalid or array with this identifier was created already
bool function Create(Keyword identifier) global native

; Deletes previously created array
; returns true on success
; return false if no array for this identifier was found
bool function Delete(Keyword identifier) global native

; Returns size of array
; returns -1 if identifier is invalid or no array found by specified identifier
int function Size(Keyword identifier) global native

; appends new element to array
; returns true on success
; returns false if identifier is invalid or no array found by specified identifier
bool function Add(Keyword identifier, int value) global native

; appends array of elements to array
; returns true on success
; returns false if identifier is invalid or no array found by specified identifier
bool function AddRange(Keyword identifier, int[] toAdd) global native

; appends array of elements to array specified by identifier
; toAdd - identifier of existing array to append. must be same type
; returns true on success
; returns false if identifier is invalid or no array found by specified identifier
bool function AppendArray(Keyword identifier, Keyword toAdd) global native

; returns element by index
; return default value if 
; - identifier is invalid 
; - no array found by specified identifier
; - index is out of range
int function Get(Keyword identifier, int index) global native

; assigns element by index
; returns true on success
; returns false if
; - identifier is invalid 
; - no array found by specified identifier
; - index is out of range
bool function Set(Keyword identifier, int index, int value) global native

; inserts new element to specified index
; returns true on success
; returns false if
; - identifier is invalid 
; - no array found by specified identifier
; - index is out of range
bool function Insert(Keyword identifier, int value, int index) global native

; searches for first occurrence of element starting from startIndex
; returns index of element if element was found
; returns -1 if
; - identifier is invalid 
; - no array found by specified identifier
; - startIndex is out of range
; - no element found
int function IndexOf(Keyword identifier, int value, int startIndex) global native

; removes element by index
; returns true if element was removed successfuly
; returns false if
; - identifier is invalid 
; - no array found by specified identifier
; - index is out of range
bool function Remove(Keyword identifier, int index) global native

; sorts elements
; mode to sort:
;  0 - ascending
;  1 - descending
; returns false if
; - identifier is invalid 
; - no array found by specified identifier
bool function Sort(Keyword identifier, int mode) global native

; removes all elements from array specified by identifier
; returns amount of removed elements on success
; returns -1 if
; - identifier is invalid
; - no array found by specified identifier 
int function Clear(Keyword identifier) global native


; returns natural array with elements of array identified by identifier
; order of elemets is undefined
; returns None if
; - identifier is None
; - no array found by specified identifier 
int[] function ToArray(Keyword identifier) global native


; dumps content of collection to log. and to console if DS:Debug.LogToConsole was enabled
function Dump(Keyword identifier) global native
; dumps content of all collections of this type to log. and to console if DS:Debug.LogToConsole was enabled
function DumpAll() global native