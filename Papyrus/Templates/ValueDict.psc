Scriptname DS:{keyTypeName}Dict{valueTypeName} Native Hidden Const

; creates an <{keyTypeName}, {valueTypeName}> dictionary
; identifier is the object id that must be used in subsequent calls of functions from this script
; returns true on success
; returns false if identifier is invalid or dictionary with this identifier was created already
bool function Create(Keyword identifier) global native

; Deletes previously created <{keyTypeName}, {valueTypeName}> dictionary
; returns true on success
; return false if no dictionary for this identifier was found
bool function Delete(Keyword identifier) global native

; Returns size of dictionary
; returns -1 if identifier is invalid or no dictionary found by specified identifier
int function Size(Keyword identifier) global native


Struct Result
    bool Found
    {valueType} Value
EndStruct
; returns value by specified key
; returns default value if 
; - identifier is invalid
; - no dictionary found by identifier
; - no key found in dictionary
Result function Get(Keyword identifier, {keyType} key) global native

; adds new pair to dictionary
; returns true if element was added successfully
; returns false if
; - identifier is invalid 
; - no dictionary found by specified identifier
; - element this key is in dictionary already
bool function Add(Keyword identifier, {keyType} key, {valueType} value) global native

; adds new or replaces existing key/value pair in dictionary defined by identifier
; if identifier or key is invalid then function will silently fail
; returns false if
; - identifier is invalid 
; - no dictionary found by specified identifier
bool function Set(Keyword identifier, {keyType} key, {valueType} value) global native

; replaces value for existing key in dictionary defined by identifier
; if key does not exist then value will not changed
; returns false if
; - identifier is invalid 
; - no dictionary found by specified identifier
; - key does not exist
bool function Replace(Keyword identifier, {keyType} key, {valueType} value) global native

; checks if key contained in the dictionary defined by identifier
; returns true if key was found
; returns false if
; - identifier is invalid 
; - no dictionary found by specified identifier
; - key was not found in the dictionary
bool function Contains(Keyword identifier, {keyType} key) global native

; removes key/value pair from dictionary defined by identifier
; returns true if element was successfuly removed
; returns false if
; - identifier is invalid 
; - no dictionary found by specified identifier
; - key was not found in the dictionary 
bool function Remove(Keyword identifier, {keyType} key) global native

; Copies keys of dictionary specified by identifier to DS:{keyTypeName}Array specified by arrayId
; if no array exist with arrayId then new one will be created
; returns true on success
; returns false if
; - identifier is invalid 
; - arrayId is invalid
; - no dictionary found by specified identifier 
; - non empty array with arrayId exist already
bool function CopyKeysToArray(Keyword identifier, Keyword arrayId) global native

; Copies keys of dictionary specified by identifier to DS:{keyTypeName}Set specified by setId
; if no set exist with setId then new one will be created
; returns true on success
; returns false if
; - identifier is invalid 
; - setId is invalid
; - no dictionary found by specified identifier 
; - non empty set with setId exist already
bool function CopyKeysToSet(Keyword identifier, Keyword setId) global native

; removes key/value pairs from dictionary specified by identifier
; elements in DS:{keyTypeName}Array specified by arrayId interpreted as keys to remove
; returns amount of removed elements
; returns -1 if
; - identifier is invalid 
; - no dictionary found by specified identifier 
; - arrayId is invalid
; - no array with arrayId was found
int function RemoveKeysByArray(Keyword identifier, Keyword arrayId) global native

; removes key/value pairs from dictionary specified by identifier
; elements in DS:{keyTypeName}Set specified by setId interpreted as keys to remove
; returns amount of removed elements
; returns -1 if
; - identifier is invalid 
; - no dictionary found by specified identifier 
; - setId is invalid
; - no set with setId was found
int function RemoveKeysBySet(Keyword identifier, Keyword setId) global native

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
    {keyType} Value
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
