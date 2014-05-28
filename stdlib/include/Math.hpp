#pragma once

#include "Primitive.hpp"

extern "C"
{

void InitializeMathLibrary();

Primitive* __SB__MathLLLLGetRandomNumber(Primitive* max);

}
