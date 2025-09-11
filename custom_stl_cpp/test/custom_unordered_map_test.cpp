#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "custom/string.h"
#include "custom/unordered_map.h"   // unit to be tested


// IMPORTANT
// Prefix all suites and fixtures with "CustomUnorderedMap_". Used in ctest run.


class CustomUnorderedMap_Operations : public ::testing::Test
{
protected:
    custom::unordered_map<int, std::string> _custom_umap_instance;

protected:

    void SetUp() override
    {
        _custom_umap_instance[0] = "Default";
        _custom_umap_instance[1] = "UMap";
        _custom_umap_instance[2] = "Values";
    }

    void TearDown() override
    {
        _custom_umap_instance.clear();
    }
};  // END CustomUnorderedMap_Operations


TEST_F(CustomUnorderedMap_Operations, copy_assign_operator)
{
    custom::unordered_map<int, std::string> other;

    EXPECT_FALSE(this->_custom_umap_instance == other);
    other = this->_custom_umap_instance;
    EXPECT_TRUE(this->_custom_umap_instance == other);
}


TEST_F(CustomUnorderedMap_Operations, move_assign_operator)
{
    custom::unordered_map<int, std::string> other;
    other[3] = "";
    other[4] = "";
    other[7] = "";

    custom::unordered_map<int, std::string> other_copy = other;

    EXPECT_FALSE(this->_custom_umap_instance == other);
    this->_custom_umap_instance = custom::move(other);
    EXPECT_FALSE(this->_custom_umap_instance == other);       // still different
    EXPECT_TRUE(this->_custom_umap_instance == other_copy);   // equal to original
}


TEST_F(CustomUnorderedMap_Operations, clear)
{
    EXPECT_FALSE(this->_custom_umap_instance.empty());

    this->_custom_umap_instance.clear();

    EXPECT_TRUE(this->_custom_umap_instance.empty());
}


TEST_F(CustomUnorderedMap_Operations, find)
{
    EXPECT_NE(this->_custom_umap_instance.find(0), this->_custom_umap_instance.end());  // key 0 is found
    EXPECT_EQ(this->_custom_umap_instance.find(5), this->_custom_umap_instance.end());  // key 5 is not found
}


TEST_F(CustomUnorderedMap_Operations, contains)
{
    EXPECT_TRUE(this->_custom_umap_instance.contains(0));   // key 0 is found
    EXPECT_FALSE(this->_custom_umap_instance.contains(5));  // key 5 is not found
}
