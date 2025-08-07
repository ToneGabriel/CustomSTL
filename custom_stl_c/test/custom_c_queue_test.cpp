#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "custom/c_queue.h"


DEFINE_GENERIC_QUEUE(
    QueueINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


TEST(QueueTest, default_init)
{
    QueueINT queue = QueueINT_create();
    QueueINT_destroy(&queue);
}
