#include "unity.h"
#include "custom/c_vector.h"


// Setup ==========================


DEFINE_GENERIC_VECTOR(
    VectorINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


static VectorINT g_customVectorINTInstance;


void setUp()
{
    g_customVectorINTInstance = VectorINT_create(GENERIC_VECTOR_DEFAULT_CAPACITY);
}

void tearDown()
{
    VectorINT_destroy(&g_customVectorINTInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_EQUAL_UINT(0, VectorINT_size(&g_customVectorINTInstance));
    TEST_ASSERT_EQUAL_UINT(GENERIC_VECTOR_DEFAULT_CAPACITY, VectorINT_capacity(&g_customVectorINTInstance));
    TEST_ASSERT_TRUE(VectorINT_empty(&g_customVectorINTInstance));
}

void test_copy()
{
    int val = 0;
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);

    VectorINT other = VectorINT_create(0);
    TEST_ASSERT_FALSE(VectorINT_equals(&other, &g_customVectorINTInstance));

    VectorINT_copy(&other, &g_customVectorINTInstance);
    TEST_ASSERT_TRUE(VectorINT_equals(&other, &g_customVectorINTInstance));

    VectorINT_destroy(&other);
}

void test_move()
{
    int val = 0;
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);

    VectorINT original_copy = VectorINT_create(0);
    VectorINT_copy(&original_copy, &g_customVectorINTInstance);

    VectorINT other = VectorINT_create(GENERIC_VECTOR_DEFAULT_CAPACITY);
    TEST_ASSERT_FALSE(VectorINT_equals(&other, &g_customVectorINTInstance));

    VectorINT_move(&other, &g_customVectorINTInstance);
    TEST_ASSERT_FALSE(VectorINT_equals(&other, &g_customVectorINTInstance));   // still not equal
    TEST_ASSERT_TRUE(VectorINT_equals(&other, &original_copy));             // equal to original

    VectorINT_destroy(&other);
    VectorINT_destroy(&original_copy);
}


// RUN ==========================

int main()
{
    UNITY_BEGIN();
    
    RUN_TEST(test_default_create);
    RUN_TEST(test_copy);
    RUN_TEST(test_move);

    return UNITY_END();
}
