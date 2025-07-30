#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "custom/c_priority_queue.h"


DEFINE_PRIORITY_QUEUE(IntPQ, double)


TEST(PriorityQueueTest, default_init)
{
    IntPQ* pq = IntPQ_create(10);
    IntPQ_destroy(pq);
}
