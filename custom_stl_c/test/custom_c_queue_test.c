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


static QueueINT g_customQueueINTInstance;


void setUp()
{
    QueueINT_initialize(&g_customQueueINTInstance);
}

void tearDown()
{
    QueueINT_destroy(&g_customQueueINTInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_EQUAL_UINT(0, QueueINT_size(&g_customQueueINTInstance));
    TEST_ASSERT_TRUE(QueueINT_empty(&g_customQueueINTInstance));
}

void test_copy()
{
    int val = 0;
    QueueINT_insert(&g_customQueueINTInstance, &val);
    QueueINT_insert(&g_customQueueINTInstance, &val);
    QueueINT_insert(&g_customQueueINTInstance, &val);

    QueueINT other = QueueINT_create();
    TEST_ASSERT_FALSE(QueueINT_equals(&other, &g_customQueueINTInstance));

    QueueINT_copy(&other, &g_customQueueINTInstance);
    TEST_ASSERT_TRUE(QueueINT_equals(&other, &g_customQueueINTInstance));

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
    QueueINT_initialize(&other);
    TEST_ASSERT_FALSE(QueueINT_equals(&other, &g_customQueueINTInstance));

    QueueINT_move(&other, &g_customQueueINTInstance);
    TEST_ASSERT_FALSE(QueueINT_equals(&other, &g_customQueueINTInstance));
    TEST_ASSERT_TRUE(QueueINT_equals(&other, &original_copy));

    QueueINT_destroy(&other);
    QueueINT_destroy(&original_copy);
}


// RUN ==========================

int main()
{
    g_customQueueINTInstance = QueueINT_create();

    UNITY_BEGIN();
    
    RUN_TEST(test_default_create);
    RUN_TEST(test_copy);
    RUN_TEST(test_move);

    return UNITY_END();
}
