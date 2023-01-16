// #include "Utility.h"
#include "String.h"
// #include "Vector.h"
// #include "List.h"
// #include "Queue.h"
#include "UnorderedMap.h"
#include "UnorderedSet.h"
// #include "Map.h"
// #include "Set.h"

#include <iostream>
// #include <string>
// #include <vector>
// #include <list>
// #include <queue>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>

class Test {

public:
	int value = 0;

	Test() {
		std::cout << "Default Construct\n";
	}

	Test(int x) {
		std::cout << "Custom Construct\n";
		value = x;
	}

	Test(const Test& other) {
		std::cout << "Copy Construct\n";
		value = other.value;
	}

	Test(Test&& other) noexcept {
		std::cout << "Move Construct\n";
		value = std::move(other.value);
	}

	~Test() {
		std::cout << "Destruct\n";
	}

	Test& operator=(const Test& other) {
		std::cout << "Copy Assign\n";
		value = other.value;
		return *this;
	}

	Test& operator=(Test&& other) noexcept {
		std::cout << "Move Assign\n";
		value = std::move(other.value);
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Test& test) {
		os << "Test val = " << test.value;
		return os;
	}

};

int main()
{
	//std::unordered_map<int, int> umap;
	//std::map<int, int> map;
	//std::set<int, int> set;
	// custom::Pair<int, Test> p(custom::piecewise_construct,
	// 						custom::forward_as_tuple(3),
	// 						custom::forward_as_tuple(3));

	// std::cout << p.First << ' ' << p.Second << '\n';

	// =====================================================================

	//{
	//	std::tuple<int, Test> tuple(3, Test(3));
	//	std::cout << std::get<1>(tuple) << '\n';
	//}
	//std::cout << "\n\n";
	//{
	//	custom::Tuple<int, Test> Tuple(3, Test(3));
	//	std::cout << custom::get<1>(Tuple) << '\n';
	//}

	// =====================================================================

	//custom::UnorderedSet<custom::String> set;
	//custom::UnorderedSet<custom::String> set1;

	//set.emplace("abc");
	//set.emplace("def");
	//set.emplace("ghi");
	//set.emplace("jkl");
	//set1 = set;
	//set == set1;
	//set.print_details();

	// =====================================================================

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