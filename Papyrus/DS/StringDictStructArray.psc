Scriptname DS:StringDictStructArray Native Hidden Const

; creates <String, StructArray> dictionary
; identifier is the object id that must be used in subsequent calls of functions from this script
; structName - full name of structure elements of what will be stored in array. 
;    Must be in format: 
;    namespace:scriptname#structname
;    or
;    scriptname#structname
;    where scriptname is the name of script where structname structure is declared
; returns true on success
; returns false if identifier is invalid or dictionary with this identifier was created already
bool function Create(Keyword identifier, string structName) global native

; Deletes previously created <String, StructArray> dictionary
; returns true on success
; return false if no dictionary for this identifier was found
bool function Delete(Keyword identifier) global native

; Returns size of dictionary
; returns -1 if identifier is invalid or no dictionary found by specified identifier
int function Size(Keyword identifier) global native

; checks if key contained in the dictionary defined by identifier
; returns true if key was found
; returns false if
; - identifier is invalid 
; - no dictionary found by specified identifier
; - no array found by specified key
bool function Contains(Keyword identifier, string key) global native

; removes key/array pair from dictionary defined by identifier
; returns true if element was successfuly removed
; returns false if
; - identifier is invalid 
; - no dictionary found by specified identifier
; - no array found by specified key 
bool function Remove(Keyword identifier, string key) global native


; returns length of array specified by key
; returns -1 if
; - identifier is invalid
; - no dictionary found by identifier
; - no array found by specified key. one could use Contains() to detect if array present in dictionary
int function ArrayLength(Keyword identifier, string key) global native

; returns struct by index from array specified by key
; returns bool false value if 
; - identifier is invalid
; - no dictionary found by identifier
; - no array found by specified key. one could use Contains() to detect if array present in dictionary
; - index is out of range
var function GetElement(Keyword identifier, string key, int index) global native

; appends new element to array specified by key
; value - struct object to append. None is allowed
; if array does not exist for this key then new array is created
; returns true if element was added successfully
; returns false if
; - identifier is invalid 
bool function AddElement(Keyword identifier, string key, var value) global native

; appends array of elements to array specified by key
; None elements are allowed
; all elements that does not match expected structure type will be added as None
; if array is not exist for this key then new array is created
; returns true if at least one element was added successfully
; returns false if
; - identifier is invalid 
; - no dictionary found by identifier
bool function AddRange(Keyword identifier, string key, var[] toAdd) global native


; assigns struct by index to array specified by key
; returns false if
; - identifier is invalid 
; - no dictionary found by identifier
; - no array found by specified key. one could use Contains() to detect if array present in dictionary
bool function SetElement(Keyword identifier, string key, var value, int index) global native

; removes element by index from array specified by key
; returns false if
; - identifier is invalid 
; - no dictionary found by identifier
; - no array found by specified key. one could use Contains() to detect if array present in dictionary
bool function RemoveAtIndex(Keyword identifier, string key, int index) global native

; inserts new struct to at specified index to array specified by key
; returns true on success
; returns false if
; - identifier is invalid 
; - no dictionary found by identifier
; - no array found by specified key. one could use Contains() to detect if array present in dictionary
; - index is out of range
bool function InsertElement(Keyword identifier, string key, var value, int index) global native


; returns index of first element equal to value in the array specified by key
; fieldName is name of field what need to be equal to fieldValue
; returns index of element if element was found
; returns -1 if
; - identifier is invalid 
; - no array found by specified key. one could use Contains() to detect if array present in dictionary
; - startIndex is out of range
; - no element found
; - fieldName is not a name of expected structure's field
; - fieldValue does not match type expected for value
int function IndexOf(Keyword identifier, string key, string fieldName, var fieldValue, int startIndex) global native

; sorts elements of array specified by key
; mode to sort:
;  0 - ascending
;  1 - descending
; returns false if
; - identifier is invalid 
; - no dictionary found by identifier
; - no array found by specified key. one could use Contains() to detect if array present in dictionary
bool function SortArray(Keyword identifier, string key, string fieldName, int mode) global native

; Copies keys of dictionary specified by identifier to DS:StringArray specified by arrayId
; if no array exist with arrayId then new one will be created
; returns true on success
; returns false if
; - identifier is invalid 
; - arrayId is invalid
; - no dictionary found by specified identifier 
; - non empty array with arrayId exist already
bool function CopyKeysToArray(Keyword identifier, Keyword arrayId) global native

; Copies keys of dictionary specified by identifier to DS:StringSet specified by setId
; if no set exist with setId then new one will be created
; returns true on success
; returns false if
; - identifier is invalid 
; - setId is invalid
; - no dictionary found by specified identifier 
; - non empty set with setId exist already
bool function CopyKeysToSet(Keyword identifier, Keyword setId) global native

; removes key/value pairs from dictionary specified by identifier
; elements in DS:StringArray specified by arrayId interpreted as keys to remove
; returns amount of removed elements
; returns -1 if
; - identifier is invalid 
; - no dictionary found by specified identifier 
; - arrayId is invalid
; - not array with arrayId was found
int function RemoveKeysByArray(Keyword identifier, Keyword arrayId) global native

; removes key/value pairs from dictionary specified by identifier
; elements in DS:StringSet specified by setId interpreted as keys to remove
; returns amount of removed elements
; returns -1 if
; - identifier is invalid 
; - no dictionary found by specified identifier 
; - setId is invalid
; - not set with setId was found
int function RemoveKeysBySet(Keyword identifier, Keyword setId) global native

; appends elements of DS:StringArray specified by arrayId to array specified by key
; if array is not exist for this key then new array is created
; returns true if element was added successfully
; returns false if
; - identifier is invalid 
; - no dictionary found by identifier
; - no array found by specified key
; - arrayId is invalid
; - no array with arrayId was found
; - array was created for another struct
bool function AppendArray(Keyword identifier, string key, Keyword arrayId) global native

; copies elements of array specified by key to DS:StringArray specified by destinationArrayId
; returns true on success
; returns false if
; - identifier is invalid 
; - no dictionary found by specified identifier 
; - targetArrayId is invalid
; - non empty array with targetArrayId exist already
; - target array is empty, but created for another struct
bool function CopyToArray(Keyword identifier, string key, Keyword targetArrayId) global native

; dumps content of collection to log. and to console if DS:Debug.LogToConsole was enabled
function Dump(Keyword identifier) global native
; dumps content of all collections of this type to log. and to console if DS:Debug.LogToConsole was enabled
function DumpAll() global native

; Iterator descriptor object
Struct KeyIterator
    ; unique iterator id. greater than 0, if valid
    int IteratorId
    ;iterating collection identifier
    Keyword CollectionId
EndStruct

; pointer to Key of key/value pair.
Struct KeyPointer
    ; unique iterator id. greater than 0, if valid
    int IteratorId
    ; iterating collection identifier
    Keyword CollectionId
    ; is pointer contains valid value
    bool Valid
    ; Key of current iteration pointer to key/value pair
    string Value
    ; is last element in collection reached
    ; will be true when Key contains last element of collection
    bool Finished
EndStruct

; creates iterator descriptor object to iterate over collection
; when collection is fully iterated then descriptor becomes invalid and can not be used
KeyIterator function CreateKeyIterator(Keyword identifier) global native

; removes existing iterator descriptor. should be called for iterrupting iteration process
; return true if iterator descriptor was exist and deleted
bool function DeleteKeyIterator(KeyIterator iterator) global native

; moves pointer to the next value in collection
; iterator - iterator descriptor to advance
KeyPointer function NextKey(KeyIterator iterator) global native
