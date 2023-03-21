#pragma once

#include "Pair.h"
#include "Tuple.h"
#include "String.h"
#include "Array.h"
#include "Vector.h"
#include "Deque.h"
#include "List.h"
#include "Queue.h"
#include "Stack.h"
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
#include <stack>
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

void list_test() {
	custom::List<Test> list;
	custom::List<Test> list1;

	list.emplace_back(1);
	list.emplace_back(2);
	list.emplace_back(3);
	list.emplace_front(4);

	list1 = custom::move(list);
	list = custom::move(list1);

	for (auto it = list.begin(); it != list.end(); ++it)
		std::cout << *it << '\n';
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

	std::cout << map.at(10) << '\n';

	map.print_details();
	std::cout << '\n';
	for(auto& val : map)
		std::cout << val.First << ' ' << val.Second << '\n';
}

void unordered_set_test() {
	custom::UnorderedSet<custom::String> set;
	custom::UnorderedSet<custom::String> set1;

	set.emplace("abc");
	set.emplace("def");
	set.emplace("ghi");
	set.emplace("jkl");
	set1 = custom::move(set);
	set = custom::move(set1);

	set.print_details();
}

void map_test() {
	custom::Map<int, Test> map;
	custom::Map<int, Test> map1;
	
	//for (size_t i = 0; i < 20; ++i)
	//	map.emplace(i, 0);

	map.emplace(0, 0);
	map.emplace(5, 0);
	map.emplace(7, 0);
	map.emplace(1, 0);
	map.emplace(10, 0);
	map.emplace(21, 0);
	map.emplace(44, 0);
	map.emplace(6, 0);
	map.emplace(8, 0);
	map.emplace(15, 0);
	map.emplace(14, 0);
	map.emplace(96, 0);
	map.emplace(2, 0);
	map.emplace(50, 0);
	map.emplace(69, 0);
	map.emplace(19, 0);
	map.emplace(29, 0);
	map.emplace(101, 0);
	map.emplace(40, 0);
	map.emplace(51, 0);
	map.emplace(99, 0);
	map.emplace(99, 0);

	map1.emplace(0, 0);
	map1.emplace(5, 0);
	map1.emplace(7, 0);
	map1.emplace(1, 0);
	map1.emplace(10, 0);

	std::cout << "Copy here\n";
	map1 = map;

	std::cout << "Map print\n";
	map.print_details();
	std::cout << "Map 1 print\n";
	map1.print_details();

	for (auto& val : map1)
		std::cout << val.First << ' ' << val.Second << '\n';

	//map.erase(17);
	//map.print_details();
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

void queue_test() {
	custom::Queue<Test> q;
	custom::Queue<Test> q1;

	q.emplace();
	q.emplace();
	q.emplace();

	q1 = q;
}

void priority_queue_test() {
	custom::PriorityQueue<int> pq;

	pq.emplace(0);
	pq.emplace(5);
	pq.emplace(1);
	pq.emplace(45);
	pq.emplace(101);
	pq.emplace(56);
	pq.emplace(12);
	pq.emplace(7);
	pq.emplace(11);

	pq.print_details();

	while (!pq.empty())
	{
		std::cout << pq.front() << '\n';
		pq.pop();
	}
}

void stack_test() {
	custom::Stack<int> s;

	s.push(0);
	s.push(3);
	s.push(1);
	s.push(2);

	while (!s.empty())
	{
		std::cout << s.top() << '\n';
		s.pop();
	}
}

void vector_test() {
	custom::Vector<int> v;
	v.emplace_back(0);
	v.emplace_back(1);
	v.emplace_back(2);

	custom::Vector<int> v1(v);


	for (auto& val : v1)
		std::cout << val << '\n';

	std::cout << v1.size() << ' ' << v1.capacity() << '\n';
}

TEST_END



STD_BEGIN
template<>
struct less<test::Test> {
	bool operator()(const test::Test& left, const test::Test& right) const {
		return left.value < right.value;
	}
};
STD_END