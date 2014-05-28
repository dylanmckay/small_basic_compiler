#include "../include/Primitive.hpp"


#include <new>

extern "C"
{

Primitive* CreatePrimitiveFromCPP(const PrimitiveCPP& cpp)
{
    Primitive* p = (Primitive*) malloc(sizeof(Primitive));

    // Use placement new to call the constructor manually.
    new(p) Primitive();

    p->internal = cpp;

    return p;
}

Primitive* CreatePrimitiveFromString(const char* string)
{
    return CreatePrimitiveFromCPP(PrimitiveCPP(string));
}

Primitive* CreatePrimitiveFromDouble(double d)
{
    return CreatePrimitiveFromCPP(PrimitiveCPP(d));
}

Primitive* CreatePrimitiveFromBoolean(bool b)
{
    return CreatePrimitiveFromCPP(PrimitiveCPP(b));
}

void CopyPrimitive(Primitive* dest, Primitive* src)
{
    dest->internal = src->internal;
}

void DestroyPrimitive(Primitive* p)
{
    p->internal.~PrimitiveCPP();

    free(p);
}

void PrimitiveSetElement(Primitive* p, Primitive* key, Primitive* val)
{
    p->internal.setElement(key->internal, val->internal);
}

Primitive* PrimitiveGetElement(Primitive* p, Primitive* key)
{
    return CreatePrimitiveFromCPP(p->internal.getElement(key->internal));
}

Primitive* PrimitiveDoesEqual(Primitive* p1, Primitive* p2)
{
    return CreatePrimitiveFromCPP(p1->internal == p2->internal);
}

Primitive* PrimitiveDoesNotEqual(Primitive* p1, Primitive* p2)
{
    return CreatePrimitiveFromCPP(p1->internal != p2->internal);
}

Primitive* PrimitiveNegative(Primitive* p)
{
    return CreatePrimitiveFromCPP(!p->internal);
}

const char* PrimitiveToString(Primitive* p)
{
    return p->internal.toString();
}

double PrimitiveToDouble(Primitive* p)
{
    return p->internal.toDouble();
}

bool PrimitiveToBoolean(Primitive* p)
{
    return p->internal.toBoolean();
}

Primitive* AddPrimitive(Primitive* p1, Primitive* p2)
{
    return CreatePrimitiveFromCPP(p1->internal + p2->internal);
}

Primitive* SubtractPrimitive(Primitive* p1, Primitive* p2)
{
    return CreatePrimitiveFromCPP(p1->internal - p2->internal);
}

Primitive* MultiplyPrimitive(Primitive* p1, Primitive* p2)
{
    return CreatePrimitiveFromCPP(p1->internal * p2->internal);
}

Primitive* DividePrimitive(Primitive* p1, Primitive* p2)
{
    return CreatePrimitiveFromCPP(p1->internal / p2->internal);
}

Primitive* PrimitiveLessThan(Primitive* p1, Primitive* p2)
{
    return CreatePrimitiveFromCPP(p1->internal < p2->internal);
}

Primitive* PrimitiveGreaterThan(Primitive* p1, Primitive* p2)
{
    return CreatePrimitiveFromCPP(p1->internal > p2->internal);
}

Primitive* PrimitiveLessThanOrEqualTo(Primitive* p1, Primitive* p2)
{
    return CreatePrimitiveFromCPP(p1->internal <= p2->internal);
}

Primitive* PrimitiveGreaterThanOrEqualTo(Primitive* p1, Primitive* p2)
{
    return CreatePrimitiveFromCPP(p1->internal >= p2->internal);
}

Primitive* PrimitiveAnd(Primitive* p1, Primitive* p2)
{
    return CreatePrimitiveFromBoolean(p1->internal.toBoolean() && p2->internal.toBoolean());
}

Primitive* PrimitiveOr(Primitive* p1, Primitive* p2)
{
    return CreatePrimitiveFromBoolean(p1->internal.toBoolean() || p2->internal.toBoolean());
}

Primitive* PrimitiveNot(Primitive* p)
{
    return CreatePrimitiveFromBoolean(!p->internal.toBoolean());
}


} // End 'extern "C"'.

/*
#include <iostream>

int main()
{
    //Primitive* p = CreatePrimitiveFromString("poop");
    //Primitive* key = CreatePrimitiveFromString("key");
    //Primitive* val = CreatePrimitiveFromString("val");

    PrimitiveCPP p;
    PrimitiveCPP key = "key";
    PrimitiveCPP val = "val";

    p.setElement(key, val);

    //PrimitiveSetElement(p, key, val);

    std::cout << key.toString() << std::endl;
}*/

