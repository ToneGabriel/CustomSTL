#include "unity.h"
#include "custom/c_queue.h"


// Setup ==========================


DEFINE_GENERIC_QUEUE(
    QueueINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


static QueueINT _custom_queue_instance;


void setUp()
{
    _custom_queue_instance = QueueINT_create();
}

void tearDown()
{
    QueueINT_destroy(&_custom_queue_instance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_EQUAL_UINT(0, QueueINT_size(&_custom_queue_instance));
    TEST_ASSERT_TRUE(QueueINT_empty(&_custom_queue_instance));
}

void test_copy()
{
    int val = 0;
    QueueINT_insert(&_custom_queue_instance, &val);
    QueueINT_insert(&_custom_queue_instance, &val);
    QueueINT_insert(&_custom_queue_instance, &val);

    QueueINT other = QueueINT_create();
    TEST_ASSERT_FALSE(QueueINT_equals(&other, &_custom_queue_instance));

    QueueINT_copy(&other, &_custom_queue_instance);
    TEST_ASSERT_TRUE(QueueINT_equals(&other, &_custom_queue_instance));

    QueueINT_destroy(&other);
}

void test_move()
{
    int val = 0;
    QueueINT_insert(&_custom_queue_instance, &val);
    QueueINT_insert(&_custom_queue_instance, &val);
    QueueINT_insert(&_custom_queue_instance, &val);

    QueueINT original_copy = QueueINT_create();
    QueueINT_copy(&original_copy, &_custom_queue_instance);

    QueueINT other = QueueINT_create();
    TEST_ASSERT_FALSE(QueueINT_equals(&other, &_custom_queue_instance));

    QueueINT_move(&other, &_custom_queue_instance);
    TEST_ASSERT_FALSE(QueueINT_equals(&other, &_custom_queue_instance));
    TEST_ASSERT_TRUE(QueueINT_equals(&other, &original_copy));

    QueueINT_destroy(&other);
    QueueINT_destroy(&original_copy);
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
