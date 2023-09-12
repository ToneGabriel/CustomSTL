#pragma once

#include "__LocalTestingInclude.h"
#include "__STDTestingInclude.h"

#define TEST_BEGIN namespace test {
#define TEST_END }


TEST_BEGIN

class Test      // basic test class
{
public:
	int value = 0;

	Test() {
		std::cout << "Default Construct\n";
	}

	Test(int x) {
		value = x;
		std::cout << "Custom Construct " << value << '\n';
	}

	Test(const Test& other) {
		value = other.value;
		std::cout << "Copy Construct " << value << '\n';
	}

	Test(Test&& other) noexcept {
		value = custom::move(other.value);
		std::cout << "Move Construct " << value << '\n';
	}

	~Test() {
		std::cout << "Destruct " << value << '\n';
	}

	Test& operator=(const Test& other) {
		value = other.value;
		std::cout << "Copy Assign " << value << '\n';
		return *this;
	}

	Test& operator=(Test&& other) noexcept {
		value = custom::move(other.value);
		std::cout << "Move Assign " << value << '\n';
		return *this;
	}

	bool operator==(const Test& other) const {
		return value == other.value;
	}

	bool operator!=(const Test& other) const {
		return !(*this == other);
	}

	friend std::ostream& operator<<(std::ostream& os, const Test& test) {
		os << "Test val = " << test.value;
		return os;
	}

	int test_function(int x) {
		return x;
	}
};

TEST_END