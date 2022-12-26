#include <iostream>
#include "Vector.h"
#include "List.h"



int main()
{
    List<std::string> list(3, "abc");
    list.push_back("xyz");


    auto it = list.at(3);
    std::cout << *it << '\n';


    for (auto& val : list)
        std::cout << val << '\n';

}
