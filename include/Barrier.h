#pragma once

#if defined __GNUG__
#include "Utility.h"
#include "Thread.h"

CUSTOM_BEGIN

template<class CompletionFunction>
class Barrier                       // Barrier adaptor for pthread_barrier_t
{
    // TODO: implement
};

CUSTOM_END

#elif defined _MSC_VER
#error NO Semaphore implementation
#endif      // __GNUG__ and _MSC_VER