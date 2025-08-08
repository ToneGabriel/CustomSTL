#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "custom/c_stack.h"


DEFINE_GENERIC_STACK(
    StackINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


TEST(StackTest, default_init)
{
    StackINT queue = StackINT_create();
    StackINT_destroy(&queue);
}
