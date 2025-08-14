#include "unity.h"
#include "custom/c_list.h"


// Setup ==========================


DEFINE_GENERIC_LIST(
    ListINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


static ListINT _custom_list_instance;


void setUp(void)
{
    _custom_list_instance = ListINT_create();
}

void tearDown(void)
{
    ListINT_destroy(&_custom_list_instance);
}


// Tests ==========================


void test_default_create(void)
{
    TEST_ASSERT_EQUAL_UINT(0, ListINT_size(&_custom_list_instance));
    TEST_ASSERT_TRUE(ListINT_empty(&_custom_list_instance));
}

void test_copy(void)
{
    int val = 0;
    ListINT_push_back(&_custom_list_instance, &val);
    ListINT_push_back(&_custom_list_instance, &val);
    ListINT_push_back(&_custom_list_instance, &val);

    ListINT other = ListINT_create();
    TEST_ASSERT_FALSE(ListINT_equals(&other, &_custom_list_instance));

    ListINT_copy(&other, &_custom_list_instance);
    TEST_ASSERT_TRUE(ListINT_equals(&other, &_custom_list_instance));

    ListINT_destroy(&other);
}

void test_move(void)
{
    int val = 0;
    ListINT_push_back(&_custom_list_instance, &val);
    ListINT_push_back(&_custom_list_instance, &val);
    ListINT_push_back(&_custom_list_instance, &val);

    ListINT original_copy = ListINT_create();
    ListINT_copy(&original_copy, &_custom_list_instance);

    ListINT other = ListINT_create();
    TEST_ASSERT_FALSE(ListINT_equals(&other, &_custom_list_instance));

    ListINT_move(&other, &_custom_list_instance);
    TEST_ASSERT_FALSE(ListINT_equals(&other, &_custom_list_instance));
    TEST_ASSERT_TRUE(ListINT_equals(&other, &original_copy));

    ListINT_destroy(&other);
    ListINT_destroy(&original_copy);
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
