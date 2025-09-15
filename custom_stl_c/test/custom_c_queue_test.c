#include "unity.h"
#include "custom/c_queue.h"


// Setup ==========================


DEFINE_GENERIC_QUEUE(
    QueueINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DESTROY
)


static QueueINT g_customQueueINTInstance;


void setUp()
{
    g_customQueueINTInstance = QueueINT_create();
}

void tearDown()
{
    QueueINT_destroy(&g_customQueueINTInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_TRUE_MESSAGE(QueueINT_empty(&g_customQueueINTInstance), "Queue should be empty");
    TEST_ASSERT_EQUAL_MESSAGE(0, QueueINT_size(&g_customQueueINTInstance), "Initial size should be 0");
}

void test_copy()
{
    int val = 0;
    QueueINT_insert(&g_customQueueINTInstance, &val);
    QueueINT_insert(&g_customQueueINTInstance, &val);
    QueueINT_insert(&g_customQueueINTInstance, &val);

    QueueINT other = QueueINT_create();
    TEST_ASSERT_FALSE_MESSAGE(QueueINT_equals(&other, &g_customQueueINTInstance), "Other queue should NOT be equal to original");

    QueueINT_copy(&other, &g_customQueueINTInstance);
    TEST_ASSERT_TRUE_MESSAGE(QueueINT_equals(&other, &g_customQueueINTInstance), "Other queue should be equal to original");

    QueueINT_destroy(&other);
}

void test_move()
{
    int val = 0;
    QueueINT_insert(&g_customQueueINTInstance, &val);
    QueueINT_insert(&g_customQueueINTInstance, &val);
    QueueINT_insert(&g_customQueueINTInstance, &val);

    QueueINT original_copy = QueueINT_create();
    QueueINT_copy(&original_copy, &g_customQueueINTInstance);

    QueueINT other = QueueINT_create();
    TEST_ASSERT_FALSE_MESSAGE(QueueINT_equals(&other, &g_customQueueINTInstance), "Other queue should NOT be equal to original");

    QueueINT_move(&other, &g_customQueueINTInstance);
    TEST_ASSERT_FALSE_MESSAGE(QueueINT_equals(&other, &g_customQueueINTInstance), "Other queue should STILL NOT be equal to original");
    TEST_ASSERT_TRUE_MESSAGE(QueueINT_equals(&other, &original_copy),"Other queue should be equal to original copy");

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
