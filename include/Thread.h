#pragma once
#include "Utility.h"

CUSTOM_BEGIN

using ThreadID_t = unsigned int;

struct Thread_t
{
    void* _Handler;
    ThreadID_t _ID;
};


class Thread
{

};

CUSTOM_END