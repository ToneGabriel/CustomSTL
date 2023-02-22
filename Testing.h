#pragma once

#include "Pair.h"
#include "Tuple.h"
#include "String.h"
#include "Array.h"
#include "Vector.h"
#include "Deque.h"
#include "List.h"
#include "Queue.h"
#include "UnorderedMap.h"
#include "UnorderedSet.h"
#include "Map.h"
#include "Set.h"

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <deque>
#include <list>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>

#define TEST_BEGIN namespace test {
#define TEST_END }


TEST_BEGIN

class Test {

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

	friend std::ostream& operator<<(std::ostream& os, const Test& test) {
		os << "Test val = " << test.value;
		return os;
	}
};

template<typename T, T... ints>
void print_sequence(custom::IntegerSequence<T, ints...> int_seq)
{
	std::cout << "The sequence of size " << int_seq.size() << ": ";
	((std::cout << ints << ' '), ...);
	std::cout << '\n';
}

void print_sequence_test() {
	print_sequence(custom::IntegerSequence<unsigned, 9, 2, 5, 1, 9, 1, 6>{});
	print_sequence(custom::MakeIntegerSequence<int, 20>{});
	print_sequence(custom::MakeIndexSequence<10>{});
	print_sequence(custom::IndexSequenceFor<float, std::iostream, char>{});
}

void piecewise_pair_test() {
	 custom::Pair<int, Test> p(custom::piecewise_construct,
 						custom::forward_as_tuple(3),
 						custom::forward_as_tuple(3));

	std::cout << p.First << ' ' << p.Second << '\n';
}

void tuple_test() {
	{
		std::tuple<int, Test> tuple(3, Test(3));
		std::cout << std::get<1>(tuple) << '\n';
	}

	std::cout << "\n\n";

	{
		custom::Tuple<int, Test> Tuple(3, Test(3));
		std::cout << custom::get<1>(Tuple) << '\n';
	}
}

void unordered_map_test() {
	custom::UnorderedMap<int, custom::String> map;

	map[10] = "abc";
	map[11] = "gsa";
	map[26] = "af";
	map[101] = " a0";
	map[54] = "69";
	map[1] = "A bv";
	map.emplace(3, "abc");
	map.try_emplace(56, custom::String("shmecher"));
	map.print_details();
}

void unordered_set_test() {
	custom::UnorderedSet<custom::String> set;
	custom::UnorderedSet<custom::String> set1;

	set.emplace("abc");
	set.emplace("def");
	set.emplace("ghi");
	set.emplace("jkl");
	set1 = set;
	set == set1;
	set.print_details();
}

void map_test() {
	custom::Map<int, Test> map;

	for (size_t i = 0; i < 20; i++)
		map.emplace(i, 0);

	map.print_details();
	map.erase(11);
}

void deque_test() {
	custom::Deque<int> dq;

	dq.emplace_back(0);
	dq.emplace_back(1);
	dq.emplace_back(2);
	dq.emplace_back(3);
	dq.emplace_back(4);
	dq.pop_front();
	dq.pop_front();
	dq.emplace_front(5);
	dq.emplace_front(6);

	dq.emplace_front(7);
	dq.emplace_front(8);
	dq.emplace_front(9);
	dq.emplace_front(10);
	dq.emplace_front(11);
	dq.emplace_front(12);
	dq.emplace_front(13);
	dq.emplace_front(14);

	dq.emplace(++dq.begin(), 101);
	dq.print_details();

	for(auto& val : dq)
	std::cout << val << ' ';
	std::cout << '\n';
}

TEST_END