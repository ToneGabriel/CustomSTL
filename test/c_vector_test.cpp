#include <gtest/gtest.h>
#include <c_vector.h>

TEST(custom_vector_test, default_constructor)
{
    custom::vector<int> vec;
    EXPECT_EQ(vec.size(), 0);
    EXPECT_TRUE(vec.empty());
}

TEST(custom_vector_test, push_back)
{
    custom::vector<int> vec;
    vec.push_back(10);
    EXPECT_EQ(vec.size(), 1);
    EXPECT_EQ(vec[0], 10);
}

TEST(custom_vector_test, out_of_bounds)
{
    custom::vector<int> vec;
    vec.push_back(5);
    EXPECT_THROW(vec.at(1), std::out_of_range);
}

TEST(custom_vector_test, iterator)
{
    custom::vector<int> vec = {1, 2, 3, 4, 5};
    int sum = 0;
    for (int v : vec) sum += v;
    EXPECT_EQ(sum, 15);
}