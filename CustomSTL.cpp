#include <iostream>
#include "Vector.h"
#include "List.h"
#include "UnorderedMap.h"
#include "UnorderedSet.h"
#include "String.h"

class Test {

public:

	Test() {
		std::cout << "Default Construct\n";
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

}
