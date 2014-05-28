#include "../include/Math.hpp"

#include <stdlib.h>
#include <time.h>

extern "C"
{

void __SBC__InitializeMathLibrary()
{
    srand(time(NULL));
}

Primitive* __SB__MathLLLLGetRandomNumber(Primitive* max)
{
    double limit = PrimitiveToDouble(max);

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do {
        retval = rand() / divisor;
    } while (retval > limit);

    return CreatePrimitiveFromDouble(retval);
}


}
