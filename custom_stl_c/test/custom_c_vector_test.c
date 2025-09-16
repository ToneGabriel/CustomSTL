#include "unity.h"
#include "custom/c_vector.h"


// Setup ==========================


DEFINE_DEFAULT_TYPE_OPERATIONS(unsigned int, myuint_t)
DEFINE_GENERIC_VECTOR(VectorUINT, myuint_t)


static VectorUINT g_customVectorUINTInstance;


void setUp()
{
    g_customVectorUINTInstance = VectorUINT_create();
}

void tearDown()
{
    VectorUINT_destroy(&g_customVectorUINTInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_TRUE_MESSAGE(VectorUINT_empty(&g_customVectorUINTInstance), "Vector should be empty");
    TEST_ASSERT_EQUAL_MESSAGE(0, VectorUINT_size(&g_customVectorUINTInstance), "Initial size should be 0");
    TEST_ASSERT_EQUAL_MESSAGE(GENERIC_VECTOR_DEFAULT_CAPACITY, VectorUINT_capacity(&g_customVectorUINTInstance), "Initial capacity should be GENERIC_VECTOR_DEFAULT_CAPACITY");
}

void test_copy()
{
    myuint_t val = 0;
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);

    VectorUINT other = VectorUINT_create();
    TEST_ASSERT_FALSE_MESSAGE(VectorUINT_equals(&other, &g_customVectorUINTInstance), "Other vector should NOT be equal to original");

    VectorUINT_copy(&other, &g_customVectorUINTInstance);
    TEST_ASSERT_TRUE_MESSAGE(VectorUINT_equals(&other, &g_customVectorUINTInstance), "Other vector should be equal to original");

    VectorUINT_destroy(&other);
}

void test_move()
{
    myuint_t val = 0;
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);

    VectorUINT original_copy = VectorUINT_create();
    VectorUINT_copy(&original_copy, &g_customVectorUINTInstance);

    VectorUINT other = VectorUINT_create();
    TEST_ASSERT_FALSE_MESSAGE(VectorUINT_equals(&other, &g_customVectorUINTInstance), "Other vector should NOT be equal to original");

    VectorUINT_move(&other, &g_customVectorUINTInstance);
    TEST_ASSERT_FALSE_MESSAGE(VectorUINT_equals(&other, &g_customVectorUINTInstance), "Other vector should STILL NOT be equal to original");
    TEST_ASSERT_TRUE_MESSAGE(VectorUINT_equals(&other, &original_copy), "Other vector should be equal to original copy");

    VectorUINT_destroy(&other);
    VectorUINT_destroy(&original_copy);
}

void test_clear()
{
    myuint_t val = 0;
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    TEST_ASSERT_FALSE_MESSAGE(VectorUINT_empty(&g_customVectorUINTInstance), "Vector should NOT be empty");

    VectorUINT_clear(&g_customVectorUINTInstance);
    TEST_ASSERT_TRUE_MESSAGE(VectorUINT_empty(&g_customVectorUINTInstance), "Vector should be empty");
}

void test_push_back_without_realloc()
{
    myuint_t val = 0;
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    TEST_ASSERT_EQUAL_MESSAGE(GENERIC_VECTOR_DEFAULT_CAPACITY, VectorUINT_capacity(&g_customVectorUINTInstance), "Vector capacity should NOT increase");
}

void test_push_back_with_realloc()
{
    myuint_t val = 0;
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);  // add 1 more
    TEST_ASSERT_TRUE_MESSAGE(GENERIC_VECTOR_DEFAULT_CAPACITY < VectorUINT_capacity(&g_customVectorUINTInstance), "Vector capacity should increase");
}

void test_pop_back()
{
    myuint_t val = 0;
    VectorUINT_push_back(&g_customVectorUINTInstance, &val);
    TEST_ASSERT_EQUAL_MESSAGE(1, VectorUINT_size(&g_customVectorUINTInstance), "Vector size should increase after push_back");

    VectorUINT_pop_back(&g_customVectorUINTInstance);
    TEST_ASSERT_EQUAL_MESSAGE(0, VectorUINT_size(&g_customVectorUINTInstance), "Vector size should decrease after pop_back");
}

void test_element_check()
{
    myuint_t val1 = 1;
    myuint_t val2 = 2;
    myuint_t val3 = 3;
    VectorUINT_push_back(&g_customVectorUINTInstance, &val1);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val2);
    VectorUINT_push_back(&g_customVectorUINTInstance, &val3);

    TEST_ASSERT_TRUE_MESSAGE(val1 == *VectorUINT_element_front(&g_customVectorUINTInstance), "Vector front element should be equal to the first push_back");
    TEST_ASSERT_TRUE_MESSAGE(val2 == *VectorUINT_element_at(&g_customVectorUINTInstance, 1), "Vector element at index 1 should be equal to the second push_back");
    TEST_ASSERT_TRUE_MESSAGE(val3 == *VectorUINT_element_back(&g_customVectorUINTInstance), "Vector back element should be equal to the last push_back");
}

// RUN ==========================

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_default_create);
    RUN_TEST(test_copy);
    RUN_TEST(test_move);
    RUN_TEST(test_clear);
    RUN_TEST(test_push_back_without_realloc);
    RUN_TEST(test_push_back_with_realloc);
    RUN_TEST(test_pop_back);
    RUN_TEST(test_element_check);

    return UNITY_END();
}
