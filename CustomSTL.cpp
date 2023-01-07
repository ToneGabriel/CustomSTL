// #include "Vector.h"
// #include "List.h"
#include "String.h"
// #include "Queue.h"
// #include "UnorderedMap.h"
// #include "UnorderedSet.h"
// #include "Map.h"
// #include "Set.h"
#include "Tuple.h"
// #include "Pair.h"

#include <iostream>
// #include <vector>
// #include <list>
// #include <queue>
// #include <unordered_map>
// #include <unordered_set>
// #include <string>

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
	 custom::Tuple<int, Test, custom::String> tuple(3, Test(3), "abc");
	 std::cout << custom::get<2>(tuple) << '\n';

	 // custom::UnorderedMap<int, custom::String> map;

	 // map[10] = "abc";
	 // map[11] = "gsa";
	 // map[26] = "af";
	 // map[101] = " a0";
	 // map[54] = "69";
	 // map[1] = "A bv";
	 // map.emplace(3, "abc");

	 // map.print_details();
}
