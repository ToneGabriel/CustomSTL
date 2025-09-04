#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "custom/string.h"
#include "custom/unordered_set.h"   // unit to be tested


class UnorderedSetTestFixture : public ::testing::Test
{
protected:
    custom::unordered_set<custom::string> _custom_uset_instance;

protected:

    void SetUp() override
    {
        _custom_uset_instance.emplace("Default");
        _custom_uset_instance.emplace("USet");
        _custom_uset_instance.emplace("Values");
    }

    void TearDown() override
    {
        _custom_uset_instance.clear();
    }
};  // END UnorderedSetTestFixture


TEST_F(UnorderedSetTestFixture, copy_assign_operator)
{
    custom::unordered_set<custom::string> other;

    EXPECT_FALSE(this->_custom_uset_instance == other);
    other = this->_custom_uset_instance;
    EXPECT_TRUE(this->_custom_uset_instance == other);
}


TEST_F(UnorderedSetTestFixture, move_assign_operator)
{
    custom::unordered_set<custom::string> other;
    other.emplace("Some");
    other.emplace("Other");
    other.emplace("Values");

    custom::unordered_set<custom::string> other_copy = other;

    EXPECT_FALSE(this->_custom_uset_instance == other);
    this->_custom_uset_instance = custom::move(other);
    EXPECT_FALSE(this->_custom_uset_instance == other);       // still different
    EXPECT_TRUE(this->_custom_uset_instance == other_copy);   // equal to original
}


TEST_F(UnorderedSetTestFixture, clear)
{
    EXPECT_FALSE(this->_custom_uset_instance.empty());

    this->_custom_uset_instance.clear();

    EXPECT_TRUE(this->_custom_uset_instance.empty());
}


TEST_F(UnorderedSetTestFixture, find)
{
    EXPECT_NE(this->_custom_uset_instance.find("Default"), this->_custom_uset_instance.end());  // "Default" is found
    EXPECT_EQ(this->_custom_uset_instance.find("NotFound"), this->_custom_uset_instance.end()); // NotFound is not found
}


TEST_F(UnorderedSetTestFixture, contains)
{
    EXPECT_TRUE(this->_custom_uset_instance.contains("Default"));   // "Default" is found
    EXPECT_FALSE(this->_custom_uset_instance.contains("NotFound")); // NotFound is not found
}
