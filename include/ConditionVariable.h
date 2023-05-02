#pragma once

#if defined __GNUG__
#include "Utility.h"
#include "Thread.h"

#include <chrono>


CUSTOM_BEGIN

// TODO: check

CUSTOM_END

#elif defined _MSC_VER
#error NO Condition Variable implementation
#endif