Scriptname DS:IntSet Native Hidden Const

; Creates set for specified identifier
; identifier is the object id that must be used in subsequent calls of functions from this script
; returns true on success
; returns false if identifier is invalid or set with this identifier was created already
bool function Create(Keyword identifier) global native

; Deletes previously created set
; returns true on success
; return false if no set for this identifier was found
bool function Delete(Keyword identifier) global native

; Returns size of set
; returns -1 if identifier is invalid or no set found by specified identifier
int  function Size(Keyword identifier) global native

; adds new element to set
; returns true if element was added successfully
; returns false if
; - identifier is invalid 
; - no set found by specified identifier
; - element was in set already
bool function Add(Keyword identifier, int value) global native

; adds collection of elements to set specified by identifier
; returns amount of inserted elements
; returns -1 if
; - identifier is invalid
; - no set found by specified identifier 
int function AddRange(Keyword identifier, int[] toAdd) global native

; removes single element from set specified by identifier
; returns true if element was successfuly removed
; returns false if
; - identifier is invalid 
; - no set found by specified identifier
; - element was not found in the set 
bool function Remove(Keyword identifier, int value) global native

; removes collection of elements from set
; returns amount of removed elements if any elements was removed
; returns -1 if
; - identifier is invalid 
; - no set found by specified identifier 
int function RemoveRange(Keyword identifier, int[] toRemove) global native

; intersects set specified by identifier with collection of elements
; modifies set in place and keeps only elements that contained in both - set and other collection
; in other words, elements that are not contained in other collection will be removed from the set 
; returns true if collection was changed
bool function Intersect(Keyword identifier, int[] other) global native

; checks if element contained in set specified by identifier
; returns true if element was found
; returns false if
; - identifier is invalid 
; - no set found by specified identifier
; - element was not found in the set
bool function Contains(Keyword identifier, int value) global native

; removes all elements from set specified by identifier
; returns amount of removed elements on success
; returns -1 if
; - identifier is invalid
; - no set found by specified identifier 
int function Clear(Keyword identifier) global native

; copies content of the set identified by identifier to DS:IntArray identified by arrayId
; if no array exist with arrayId then new one will be created
; returns true on success
; returns false if
; - identifier is invalid 
; - arrayId is invalid
; - no set found by specified identifier 
; - non empty array with arrayId exist already
bool function CopyToArray(Keyword identifier, Keyword arrayId) global native

; returns natural array with elements of set identified by identifier
; order of elemets is undefined
; returns None if
; - identifier is invalid
; - no set found by specified identifier 
int[] function ToArray(Keyword identifier) global native

; adds elements of DS:IntArray specified by arrayId to set specified by identifier
; returns amount of inserted elements if any elements was inserted
; returns -1 if
; - identifier is invalid
; - no set found by specified identifier 
; - arrayId is invalid
; - no array found by specified arrayId
int function AddArray(Keyword identifier, Keyword arrayId) global native

; adds elements of DS:IntSet specified by setId to set specified by identifier
; returns amount of inserted elements if any elements was inserted
; returns -1 if
; - identifier is invalid
; - no set found by specified identifier 
; - setId is invalid
; - no set found by specified setId
int function AddSet(Keyword identifier, Keyword setId) global native

; intersects set specified by identifier with DS:IntArray elements
; modifies set in place and keeps only elements that contained in both - set and DS:IntArray
; in other words, elements that are not contained in DS:IntArray will be removed from the set 
; returns true if collection was changed
bool function IntersectArray(Keyword identifier, Keyword arrayId) global native

; intersects set specified by identifier with DS:IntSet elements
; modifies set in place and keeps only elements that contained in both - set and DS:IntSet
; in other words, elements that are not contained in DS:IntSet will be removed from the set 
; returns true if collection was changed
bool function IntersectSet(Keyword identifier, Keyword setId) global native

; removes elements of DS:IntArray specified by arrayId from set specified by identifier
; returns amount of removed elements 
; returns -1 if
; - identifier is invalid 
; - no set found by specified identifier 
; - arrayId is invalid
; - no array found by specified arrayId
int function RemoveArray(Keyword identifier, Keyword arrayId) global native

; removes elements of DS:IntSet specified by setId from set specified by identifier
; returns amount of removed elements 
; returns -1 if
; - identifier is invalid 
; - no set found by specified identifier 
; - setId is invalid
; - no set found by specified setId
int function RemoveSet(Keyword identifier, Keyword setId) global native

; dumps content of collection to log. and to console if DS:Debug.LogToConsole was enabled
function Dump(Keyword identifier) global native
; dumps content of all collections of this type to log. and to console if DS:Debug.LogToConsole was enabled
function DumpAll() global native

; Iterator descriptor object
Struct Iterator
    ; unique iterator id. greater than 0, if valid
    int IteratorId
    ; collection identifier
    Keyword CollectionId
EndStruct

; Value pointer.
Struct Pointer
    ; unique iterator id. greater than 0, if valid
    int IteratorId
    ; collection identifier
    Keyword CollectionId
    ; is pointer contains valid Value
    bool Valid
    ; value of current iteration pointer
    int Value 
    ; is last element in collection reached
    ; will be true when Value contains last element of the collection
    bool Finished
EndStruct

; creates iterator descriptor object to iterate over collection
; when collection is fully iterated then descriptor becomes invalid and can not be used
Iterator function CreateIterator(Keyword identifier) global native

; removes existing iterator descriptor. should be called for iterrupting iteration process
; return true if iterator descriptor was exist and deleted
bool function DeleteIterator(Iterator iterator) global native

; moves pointer to the next value in collection
; iterator - iterator descriptor to advance
Pointer function Next(Iterator iterator) global native

