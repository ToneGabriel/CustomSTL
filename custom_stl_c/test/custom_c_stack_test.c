#include "unity.h"
#include "custom/c_stack.h"


// Setup ==========================


DEFINE_GENERIC_STACK(
    StackINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


static StackINT _custom_stack_instance;


void setUp(void)
{
    _custom_stack_instance = StackINT_create();
}

void tearDown(void)
{
    StackINT_destroy(&_custom_stack_instance);
}


// Tests ==========================


void test_default_create(void)
{
    TEST_ASSERT_EQUAL_UINT(0, StackINT_size(&_custom_stack_instance));
    TEST_ASSERT_TRUE(StackINT_empty(&_custom_stack_instance));
}

void test_copy(void)
{
    int val = 0;
    StackINT_insert(&_custom_stack_instance, &val);
    StackINT_insert(&_custom_stack_instance, &val);
    StackINT_insert(&_custom_stack_instance, &val);

    StackINT other = StackINT_create();
    TEST_ASSERT_FALSE(StackINT_equals(&other, &_custom_stack_instance));

    StackINT_copy(&other, &_custom_stack_instance);
    TEST_ASSERT_TRUE(StackINT_equals(&other, &_custom_stack_instance));

    StackINT_destroy(&other);
}

void test_move(void)
{
    int val = 0;
    StackINT_insert(&_custom_stack_instance, &val);
    StackINT_insert(&_custom_stack_instance, &val);
    StackINT_insert(&_custom_stack_instance, &val);

    StackINT original_copy = StackINT_create();
    StackINT_copy(&original_copy, &_custom_stack_instance);

    StackINT other = StackINT_create();
    TEST_ASSERT_FALSE(StackINT_equals(&other, &_custom_stack_instance));

    StackINT_move(&other, &_custom_stack_instance);
    TEST_ASSERT_FALSE(StackINT_equals(&other, &_custom_stack_instance));
    TEST_ASSERT_TRUE(StackINT_equals(&other, &original_copy));

    StackINT_destroy(&other);
    StackINT_destroy(&original_copy);
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
