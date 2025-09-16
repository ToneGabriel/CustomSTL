#include "unity.h"
#include "custom/c_queue.h"


// Setup ==========================


DEFINE_DEFAULT_TYPE_OPERATIONS(unsigned int, myuint_t)
DEFINE_GENERIC_QUEUE(QueueUINT, myuint_t)


static QueueUINT g_customQueueUINTInstance;


void setUp()
{
    g_customQueueUINTInstance = QueueUINT_create();
}

void tearDown()
{
    QueueUINT_destroy(&g_customQueueUINTInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_TRUE_MESSAGE(QueueUINT_empty(&g_customQueueUINTInstance), "Queue should be empty");
    TEST_ASSERT_EQUAL_MESSAGE(0, QueueUINT_size(&g_customQueueUINTInstance), "Initial size should be 0");
}

void test_copy()
{
    myuint_t val = 0;
    QueueUINT_insert(&g_customQueueUINTInstance, &val);
    QueueUINT_insert(&g_customQueueUINTInstance, &val);
    QueueUINT_insert(&g_customQueueUINTInstance, &val);

    QueueUINT other = QueueUINT_create();
    TEST_ASSERT_FALSE_MESSAGE(QueueUINT_equals(&other, &g_customQueueUINTInstance), "Other queue should NOT be equal to original");

    QueueUINT_copy(&other, &g_customQueueUINTInstance);
    TEST_ASSERT_TRUE_MESSAGE(QueueUINT_equals(&other, &g_customQueueUINTInstance), "Other queue should be equal to original");

    QueueUINT_destroy(&other);
}

void test_move()
{
    myuint_t val = 0;
    QueueUINT_insert(&g_customQueueUINTInstance, &val);
    QueueUINT_insert(&g_customQueueUINTInstance, &val);
    QueueUINT_insert(&g_customQueueUINTInstance, &val);

    QueueUINT original_copy = QueueUINT_create();
    QueueUINT_copy(&original_copy, &g_customQueueUINTInstance);

    QueueUINT other = QueueUINT_create();
    TEST_ASSERT_FALSE_MESSAGE(QueueUINT_equals(&other, &g_customQueueUINTInstance), "Other queue should NOT be equal to original");

    QueueUINT_move(&other, &g_customQueueUINTInstance);
    TEST_ASSERT_FALSE_MESSAGE(QueueUINT_equals(&other, &g_customQueueUINTInstance), "Other queue should STILL NOT be equal to original");
    TEST_ASSERT_TRUE_MESSAGE(QueueUINT_equals(&other, &original_copy),"Other queue should be equal to original copy");

    QueueUINT_destroy(&other);
    QueueUINT_destroy(&original_copy);
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
