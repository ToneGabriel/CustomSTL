#include <iostream>
#include "Vector.h"
// #include "List.h"
// #include "Queue.h"
// #include "UnorderedMap.h"
// #include "UnorderedSet.h"
// #include "String.h"

//#include <vector>
//#include <list>
//#include <queue>
//#include <unordered_map>
//#include <unordered_set>
//#include <string>
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
	}

	Test(Test&& other) noexcept {
		std::cout << "Move Construct\n";
	}

	~Test() {
		std::cout << "Destruct\n";
	}

	Test& operator=(const Test& other) {
		std::cout << "Copy Assign\n";
		return *this;
	}

	Test& operator=(Test&& other) noexcept {
		std::cout << "Move Assign\n";
		return *this;
	}

};

int main()
{
	custom::Vector<int> vec;

	vec.push_back(3);
	vec.push_back(3);

	for(auto& val:vec)
		std::cout<<val<<'\n';

	// custom::UnorderedMap<int, std::string> map;

	// map[10] = "abc";
	// map[11] = "gsa";
	// map[26] = "af";
	// map[101] = " a0";
	// map[54] = "69";
	// map[1] = "A bv";
	// map.emplace(3, "abc");

	// map.print_details();
}
