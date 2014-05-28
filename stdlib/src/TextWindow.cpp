#include "../include/TextWindow.hpp"

#include <stdio.h>

extern "C"
{

void __SB__TextWindowLLLLWriteLine(Primitive* text)
{
    puts(PrimitiveToString(text));
}

Primitive* __SB__TextWindowLLLLRead()
{
    char* linePtr = NULL;
    size_t size;

    getline(&linePtr, &size, stdin);

    // Place a null terminator before the new line.
    for(unsigned int i=0; i<strlen(linePtr); i++)
        if(linePtr[i] == '\r' || linePtr[i] == '\n')
            linePtr[i] = '\0';

    Primitive* result = CreatePrimitiveFromString(linePtr);

    free(linePtr);

    return result;
}

}
