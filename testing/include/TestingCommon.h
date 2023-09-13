#pragma once

#include "__CTMCommonInclude.h"
#include "__STDCommonInclude.h"

#define TEST_BEGIN namespace test {
#define TEST_END }

TEST_BEGIN

class Test      // basic test class
{
public:
	int value = 0;

public:
    // Constructors & Operators

	Test();
	Test(int x);
	Test(const Test& other);
	Test(Test&& other) noexcept;
	~Test();

	Test& operator=(const Test& other);
	Test& operator=(Test&& other) noexcept;
	bool operator==(const Test& other) const;
	bool operator!=(const Test& other) const;

	friend std::ostream& operator<<(std::ostream& os, const Test& test);

public:
    // Main functions

	int test_function(int x);
};  // END Test


template<typename T, T... ints>
void print_sequence(custom::IntegerSequence<T, ints...> int_seq);

void print_sequence_test();
void piecewise_pair_test();
void tuple_test();
void list_test();
void unordered_map_test();
void unordered_set_test();
void map_test();
void deque_test();
void queue_test();
void priority_queue_test();
void stack_test();
void vector_test();
void string_test();
void array_test();
void function_test();
void invoke_test();
void memory_test();
void print_numeric_limits_statics();
void print_numeric_limits_functions();
void chrono_test();

TEST_END