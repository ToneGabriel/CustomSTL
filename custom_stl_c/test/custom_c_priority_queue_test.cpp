#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "custom/c_priority_queue.h"


DEFINE_GENERIC_PRIORITY_QUEUE(
    IntPQ,
    int,
    DEFAULT_TYPE_REF_LESS,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


TEST(PriorityQueueTest, default_init)
{
    IntPQ pq = IntPQ_create();
    IntPQ_destroy(&pq);
}
