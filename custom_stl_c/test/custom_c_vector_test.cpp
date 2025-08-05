#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "custom/c_vector.h"


DEFINE_GENERIC_VECTOR(
    VectorINT,
    _VectorINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


TEST(VectorTest, default_init)
{
    VectorINT vec = VectorINT_create(10);
    VectorINT_destroy(&vec);
}
