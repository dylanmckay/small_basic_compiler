#include "include/Primitive.hpp"
#include "include/TextWindow.hpp"

#include "include/dlmalloc.hpp"

#include <stdio.h>


int main()
{
    char* ptr = (char*) dlmalloc(512);

    ptr[0] = 'h';
    ptr[1] = 'e';
    ptr[2] = 'y';
    ptr[3] = 0;

    puts(ptr);

    dlfree(ptr);

    /*
    Primitive* prim1 = CreatePrimitiveFromString("hello");
    Primitive* prim2 = CreatePrimitiveFromString(" world");

    Primitive* result = AddPrimitive(prim1, prim2);

    __SB__TextWindowLLLLWriteLine(result);


    DestroyPrimitive(result);
    DestroyPrimitive(prim2);
    DestroyPrimitive(prim1);*/

    return 0;
}
