#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "custom/c_list.h"


DEFINE_GENERIC_LIST(
    ListINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


TEST(ListTest, default_init)
{
    ListINT list = ListINT_create();
    ListINT_destroy(&list);
}
