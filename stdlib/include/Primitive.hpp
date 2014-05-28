#pragma once

#include "PrimitiveCPP.hpp"
#include <stdlib.h>
#include <string.h>


extern "C"
{

/*!
 * A small basic primitive value.
 */
typedef struct
{
    PrimitiveCPP internal;
} Primitive;


Primitive* CreatePrimitiveFromCPP(const PrimitiveCPP& cpp);

/*!
 * Creates a new primitive object on the heap with the given string.
 */
Primitive* CreatePrimitiveFromString(const char* string);

/*!
 * Creates a new primitive with the given numeric value.
 */
Primitive* CreatePrimitiveFromDouble(double d);

/*!
 * Creates a new primitive from the given boolean value.
 */
Primitive* CreatePrimitiveFromBoolean(bool b);

/*!
 * Copies the source primitive into the destination primitive.
 */
void CopyPrimitive(Primitive* dest, Primitive* src);

/*!
 * Destroys the given primitive object.
 */
void DestroyPrimitive(Primitive* p);

/*!
 * Sets the element in the primitive 'p' of the key to val.
 */
void PrimitiveSetElement(Primitive* p, Primitive* key, Primitive* val);

/*!
 * Gets the element with the given key from the given primitive.
 */
Primitive* PrimitiveGetElement(Primitive* p, Primitive* key);

/*!
 * Compares two primitives.
 */
Primitive* PrimitiveDoesEqual(Primitive* p1, Primitive* p2);

/*!
 * Compares if two primitives are not equal.
 */
Primitive* PrimitiveDoesNotEqual(Primitive* p1, Primitive* p2);

/*!
 * Makes a primitive negative.
 */
Primitive* PrimitiveNegative(Primitive* p);

/*!
 * Gets a string representing the given primitive.
 */
const char* PrimitiveToString(Primitive* p);

/*!
 * Converts the given primitive to double.
 */
double PrimitiveToDouble(Primitive* p);

/*!
 * Converts the given primitive to a boolean.
 */
bool PrimitiveToBoolean(Primitive* p);

/*!
 * Performs primitive addition.
 */
Primitive* AddPrimitive(Primitive* p1, Primitive* p2);

/*!
 * Performs primitive subtraction.
 */
Primitive* SubtractPrimitive(Primitive* p1, Primitive* p2);

/*!
 * Performs primitive multipication.
 */
Primitive* MultiplyPrimitive(Primitive* p1, Primitive* p2);

/*!
 * Performs primitive division.
 */
Primitive* DividePrimitive(Primitive* p1, Primitive* p2);

/*!
 * Checks if primitive one is less than primitive two.
 */
Primitive* PrimitiveLessThan(Primitive* p1, Primitive* p2);

/*!
 * Checks if primitive one is greater than primitive two.
 */
Primitive* PrimitiveGreaterThan(Primitive* p1, Primitive* p2);


Primitive* PrimitiveLessThanOrEqualTo(Primitive* p1, Primitive* p2);
Primitive* PrimitiveGreaterThanOrEqualTo(Primitive* p1, Primitive* p2);

Primitive* PrimitiveAnd(Primitive* p1, Primitive* p2);

Primitive* PrimitiveOr(Primitive* p1, Primitive* p2);

Primitive* PrimitiveNot(Primitive* p);

/*!
 * Checks if the given primitive is numeric.
 */
bool IsPrimitiveNumeric(Primitive* p);

}
