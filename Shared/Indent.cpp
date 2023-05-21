#include "Indent.h"


char indentValues[] = "                ";

static_assert(sizeof(indentValues) == Indent::MaxIndent + 1, "Max indent mismatch");

const char* Indent::Get(int n)
{
	return &indentValues[MaxIndent - n];
}