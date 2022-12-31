#include <iostream>
#include "Vector.h"
#include "List.h"
#include "Queue.h"
#include "UnorderedMap.h"
#include "UnorderedSet.h"
#include "String.h"

#include <vector>
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
	}

	Test& operator=(Test&& other) noexcept {
		std::cout << "Move Assign\n";
	}

};


int main()
{
	custom::Vector<int> a1(3, 5);
	custom::Vector<int> a2(3, 5);

	if (a1 == a2)
		std::cout << "Da\n";
	else
		std::cout << "Nu\n";


	//custom::UnorderedMap<custom::UnorderedMap<int, int>, int, custom::UnorderedMapHash> map;

	//map.emplace(custom::UnorderedMap<int, int>(3), 3);
	//map.emplace(custom::UnorderedMap<int, int>(5), 4);
	//map.emplace(custom::UnorderedMap<int, int>(10), 5);

	//map.print_details();

	// =================================================================================================
	
	//custom::UnorderedMap<int, std::string> map;

	//map[10] = "abc";
	//map[11] = "gsa";
	//map[26] = "af";
	//map[101] = " a0";
	//map[54] = "69";
	//map[1] = "A bv";
	//map.emplace(3, "abc");

	//map.print_details();
}
