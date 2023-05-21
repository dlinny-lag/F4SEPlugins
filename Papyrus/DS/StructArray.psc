Scriptname DS:StructArray Native Hidden Const

; Creates array for specified identifier
; identifier is the object id that must be used in subsequent calls of functions from this script
; structName - full name of structure elements of what will be stored in array. 
;    Must be in format: 
;    namespace:scriptname#structname
;    or
;    scriptname#structname
;    where scriptname is the name of script where structname structure is declared
; subsequent calls to created array will validate input values to match their types to structure type
; returns true on success
; returns false if
; - identifier is invalid 
; - array with this identifier was created already
; - no structure with that name was found 
bool function Create(Keyword identifier, string structName) global native

; Deletes previously created array
; returns true on success
; return false if no array for this identifier was found
bool function Delete(Keyword identifier) global native

; Returns size of array
; returns -1 if identifier is invalid or no array found by specified identifier
int function Size(Keyword identifier) global native

; appends new element to array
; None value is allowed
; returns true on success
; returns false if identifier is invalid or no array found by specified identifier
bool function Add(Keyword identifier, var value) global native

; appends array of elements to array
; None elements are allowed
; all elements that does not match expected structure type will be added as None
; returns true on success
; returns false if identifier is invalid or no array found by specified identifier
bool function AddRange(Keyword identifier, var[] toAdd) global native

; appends array of elements to array specified by identifier
; toAdd - identifier of existing array to append. must be same type
; returns true on success
; returns false if identifier is invalid or no array found by specified identifier
bool function AppendArray(Keyword identifier, Keyword toAdd) global native

; returns element by index
; None value is returned if 
; - identifier is invalid 
; - no array found by specified identifier
; - index is out of range
var function Get(Keyword identifier, int index) global native

; assigns element by index
; returns true on success
; returns false if
; - identifier is invalid 
; - no array found by specified identifier
; - index is out of range
bool function Set(Keyword identifier, int index, var value) global native

; inserts new element to specified index
; returns true on success
; returns false if
; - identifier is invalid 
; - no array found by specified identifier
; - index is out of range
; - value's type does not match expected structure type
bool function Insert(Keyword identifier, var value, int index) global native

; searches for first occurrence of element starting from startIndex
; fieldName is name of field what need to be equal to fieldValue
; returns index of element if element was found
; returns -1 if
; - identifier is invalid 
; - no array found by specified identifier
; - startIndex is out of range
; - no element found
; - fieldName is not a name of expected structure's field
; - fieldValue does not match type expected for value
int function IndexOf(Keyword identifier, string fieldName, var fieldValue, int startIndex) global native

; removes element by index
; returns true if element was removed successfuly
; returns false if
; - identifier is invalid 
; - no array found by specified identifier
; - index is out of range
bool function Remove(Keyword identifier, int index) global native

; sorts struct objects by specified fieldName
; mode to sort:
;  0 - ascending
;  1 - descending
; returns false if
; - identifier is invalid 
; - no array found by specified identifier
; - internal error
bool function Sort(Keyword identifier, string fieldName, int mode) global native

; removes all elements from array specified by identifier
; returns amount of removed elements on success
; returns -1 if
; - identifier is invalid
; - no array found by specified identifier 
int function Clear(Keyword identifier) global native

; dumps content of collection to log. and to console if DS:Debug.LogToConsole was enabled
function Dump(Keyword identifier) global native
; dumps content of all collections of this type to log. and to console if DS:Debug.LogToConsole was enabled
function DumpAll() global native