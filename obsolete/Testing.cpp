#include <Testing.h>


TEST_BEGIN

Test::Test()
{
	std::cout << "Default Construct\n";
}

Test::Test(int x)
{
	TestValue = x;
	std::cout << "Custom Construct " << TestValue << '\n';
}

Test::Test(const Test& other)
{
	TestValue = other.TestValue;
	std::cout << "Copy Construct " << TestValue << '\n';
}

Test::Test(Test&& other) noexcept
{
	TestValue = custom::move(other.TestValue);
	std::cout << "Move Construct " << TestValue << '\n';
}

Test::~Test()
{
	std::cout << "Destruct " << TestValue << '\n';
}

Test& Test::operator=(const Test& other)
{
	TestValue = other.TestValue;
	std::cout << "Copy Assign " << TestValue << '\n';
	return *this;
}

Test& Test::operator=(Test&& other) noexcept
{
	TestValue = custom::move(other.TestValue);
	std::cout << "Move Assign " << TestValue << '\n';
	return *this;
}

bool Test::operator==(const Test& other) const {
	return TestValue == other.TestValue;
}

bool Test::operator!=(const Test& other) const {
	return !(*this == other);
}

void Test::operator()() const {
	std::cout << "TestValue= " << TestValue << '\n';
}

std::ostream& operator<<(std::ostream& os, const Test& test)
{
	os << "TestValue = " << test.TestValue;
	return os;
}

bool operator<(const Test& test1, const Test& test2)
{
	return test1.TestValue < test2.TestValue;
}

int Test::test_function(int x)
{
	return x;
}


void print_sequence_test()
{
	print_sequence(custom::integer_sequence<unsigned, 9, 2, 5, 1, 9, 1, 6>{});
	print_sequence(custom::make_integer_sequence<int, 20>{});
	print_sequence(custom::make_index_sequence<10>{});
	print_sequence(custom::index_sequence_for<float, std::iostream, char>{});
}

void piecewise_pair_test()
{
	 custom::pair<int, Test> p(
		 custom::piecewise_construct,
		 custom::forward_as_tuple(3),
		 custom::forward_as_tuple(3));

	std::cout << p.first << ' ' << p.second << '\n';
}

void tuple_test()
{
	{
		std::tuple<int, Test> tuple1(3, Test(3));
		std::tuple<Test, int, float> tuple2(Test(3), 3, 3.3);
		auto tuple3 = std::tuple_cat(std::move(tuple1), std::move(tuple2));
		std::cout << std::get<4>(tuple3) << '\n';
		std::cout << std::get<1>(tuple1) << '\n';
	}

	std::cout << "\n\n";

	{
		custom::tuple<int, Test> tuple1(3, Test(3));
		custom::tuple<Test, int, float> tuple2(Test(3), 3, 3.3);
		auto tuple3 = custom::tuple_cat(custom::move(tuple1), custom::move(tuple2));
		std::cout << custom::get<4>(tuple3) << '\n';
		std::cout << custom::get<1>(tuple1) << '\n';
	}
}

void list_test()
{
	custom::list<Test> list;
	custom::list<Test> list1;

	list.emplace_back(15);
	list.emplace_back(8);
	list.emplace_back(1);
	list.emplace_back(6);
	list.emplace_back(5);
	list.emplace_back(8);
	list.sort();

	list1.emplace_back(15);
	list1.emplace_back(4);
	list1.emplace_back(10);
	list1.emplace_back(7);
	list1.sort();

	list.merge(list1);

	for (auto it = list.begin(); it != list.end(); ++it)
		std::cout << *it << '\n';

	for (auto it = list1.begin(); it != list1.end(); ++it)
		std::cout << *it << '\n';
}

void forward_list_test()
{
	custom::forward_list<Test> flist;
	flist.emplace_front(1);
	flist.emplace_front(2);
	flist.emplace_front(3);

	for (auto& val : flist)
		std::cout << val << '\n';
}

void unordered_map_test()
{
	custom::unordered_map<int, custom::string> map;

	map[10] = "abc";
	map[11] = "gsa";
	map[26] = "af";
	map[101] = " a0";
	map[54] = "69";
	map[1] = "A bv";
	map.emplace(3, "abc");
	map.try_emplace(56, custom::string("shmecher"));

	std::cout << map.at(10) << '\n';

	map.print_details();
	std::cout << '\n';
	for (auto& val : map)
		std::cout << val.first << ' ' << val.second << '\n';
}

void unordered_set_test()
{
	custom::unordered_set<custom::string> set;
	custom::unordered_set<custom::string> set1;

	set.emplace("abc");
	set.emplace("def");
	set.emplace("ghi");
	set.emplace("jkl");
	set1 = custom::move(set);
	set = custom::move(set1);

	set.print_details();
}

void map_test()
{
	custom::map<int, int> map;

	map.emplace(5, 5);
	map.emplace(0, 0);
	map.emplace(7, 7);
	map.emplace(1, 1);
	map.emplace(10, 10);
	map.emplace(21, 21);
	map.emplace(44, 44);
	map.emplace(6, 6);
	map.emplace(8, 8);
	map.emplace(15, 15);
	map.emplace(14, 14);
	map.emplace(96, 96);
	map.emplace(2, 2);
	map.emplace(50, 50);
	map.emplace(69, 69);
	map.emplace(19, 19);
	map.emplace(29, 29);
	map.emplace(101, 101);
	map.emplace(40, 40);
	map.emplace(51, 51);
	map.emplace(99, 99);
	map.emplace(99, 99);

	std::cout << "Map before erase\n";
	map.print_details();

	std::cout << "Current elem= " << (*map.erase(10)).first << '\n';

	std::cout << "Map after erase\n";
	map.print_details();

	for (auto& val : map)
		std::cout << val.first << ' ' << val.second << '\n';
}

void deque_test()
{
	custom::deque<int> dq;
	dq.emplace_back(0);
	dq.emplace_back(1);
	dq.emplace_back(2);
	dq.emplace_back(3);
	dq.emplace_front(4);
	auto it = dq.emplace(dq.end() - 1, 11);
	it = dq.emplace(it, 12);

	it = dq.erase(dq.begin());
	it = dq.erase(it);

	for (auto& val : dq)
		std::cout << val << ' ';
	std::cout << '\n';

	dq.print_details();
}

void queue_test()
{
	custom::queue<Test> q;

	for (size_t i = 0; i < 50; ++i)
		q.emplace(i);

	while (!q.empty())
	{
		std::cout << q.front() << '\n';
		q.pop();
	}
}

void priority_queue_test()
{
	custom::priority_queue<int> pq;

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

void stack_test()
{
	custom::stack<int> s;

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

void vector_test()
{
	custom::vector<int> v;
	v.emplace_back(0);
	v.emplace_back(1);
	v.emplace_back(2);

	custom::vector<int> v1(v);


	for (auto& val : v1)
		std::cout << val << '\n';

	std::cout << v1.size() << ' ' << v1.capacity() << '\n';
}

void string_test()
{
	custom::string s("abcd");
	custom::string s1("abcd");
	s.insert(++s.begin(),s1.begin(),--s1.end());

	std::string ss("abcd");
	std::string ss1("abcd");
	ss.insert(++ss.begin(),ss1.begin(),--ss1.end());
	std::cout << ss << '\n';

	s.print_details();
}

void string_view_test()
{
	custom::string str = "cdef";
	custom::string_view sv1 = "abcdef";
	custom::string_view sv2(str);
	std::cout << sv1.find(sv2, 1) << '\n';

	// =====================================
	
	custom::string_view sv3 = "abcd";
	custom::string str1(sv3);
	std::cout << str1 << '\n';
}

void array_test()
{
	custom::array<Test, 5> arr;
	arr.fill(Test(3));

	for (auto& val : arr)
		std::cout << val << '\n';
}

void function_test()
{
	custom::function<void()> fct1(unordered_map_test);
	custom::function<void()> fct2(Test{});
	fct1.swap(fct2);
	fct1();
}

void invoke_test()
{
	Test t;
	std::cout << custom::invoke(&Test::test_function, t, 3) << '\n';
}

void memory_test()
{
	// unique_ptr===============
	//custom::unique_ptr<Test> up = custom::make_unique<Test>(3);

	// shared_ptr===============
	custom::shared_ptr<Test[]> sharedArray(new Test[5]);

    for (int i = 0; i < 5; ++i)
        sharedArray[i] = i; // Populating the array

    for (int i = 0; i < 5; ++i)
        std::cout << sharedArray[i] << " "; // Printing array elements

    std::cout << '\n';
}

void chrono_test()
{
	{
		using namespace std::chrono;
		//using namespace std::chrono_literals;

		microseconds ms1(100000);
		time_point<system_clock, microseconds> absoluteTime1(ms1);

		auto _seconds1 		= time_point_cast<seconds>(absoluteTime1);
		auto _nanoseconds1 	= duration_cast<nanoseconds>(absoluteTime1 - _seconds1);

		std::cout << absoluteTime1.time_since_epoch().count() << '\n';
		std::cout << _seconds1.time_since_epoch().count() << '\n';
		std::cout << _nanoseconds1.count() << '\n';
	}

	// ================================================================

	{
		using namespace custom::chrono;

		microseconds ms2(100000);
		time_point<system_clock, microseconds> absoluteTime2(ms2);

		auto _seconds2 		= time_point_cast<seconds>(absoluteTime2);
		auto _nanoseconds2 	= duration_cast<nanoseconds>(absoluteTime2 - _seconds2);

		std::cout << absoluteTime2.time_since_epoch().count() << '\n';
		std::cout << _seconds2.time_since_epoch().count() << '\n';
		std::cout << _nanoseconds2.count() << '\n';
	}
}

void complex_test()
{
	std::complex<float> cf(3.5, 8.0);

	std::cout << "number= " << cf.real() << '\n' << cf.imag() << '\n';
	std::cout << "abs= " 	<< std::abs(cf) << '\n';
	std::cout << "norm= " 	<< std::norm(cf) << '\n';
	std::cout << "conj= " 	<< std::conj(cf) << '\n';
	std::cout << "proj= " 	<< std::proj(cf) << '\n';
	std::cout << "polar= " 	<< std::polar(-2.5, 4.3) << '\n';

	std::cout << "exp= " 	<< std::exp(cf) << '\n';
	std::cout << "log= " 	<< std::log(cf) << '\n';
	std::cout << "log10= " 	<< std::log10(cf) << '\n';
	std::cout << "sqrt= " 	<< std::sqrt(cf) << '\n';
	std::cout << "pow c2= "	<< std::pow(cf, 2.0f) << '\n';
	std::cout << "pow 2c= "	<< std::pow(2.0f, cf) << '\n';
	std::cout << "pow cc= "	<< std::pow(cf, cf) << '\n';

	std::cout << "sin= " 	<< std::sin(cf) << '\n';
	std::cout << "asin= " 	<< std::asin(cf) << '\n';
	std::cout << "sinh= " 	<< std::sinh(cf) << '\n';
	std::cout << "asinh= " 	<< std::asinh(cf) << '\n';

	std::cout << "cos= " 	<< std::cos(cf) << '\n';
	std::cout << "acos= " 	<< std::acos(cf) << '\n';
	std::cout << "cosh= " 	<< std::cosh(cf) << '\n';
	std::cout << "acosh= " 	<< std::acosh(cf) << '\n';

	std::cout << "tan= " 	<< std::tan(cf) << '\n';
	std::cout << "atan= " 	<< std::atan(cf) << '\n';
	std::cout << "tanh= " 	<< std::tanh(cf) << '\n';
	std::cout << "atanh= " 	<< std::atanh(cf) << '\n';

	std::cout << "======================\n";

	custom::complex<float> cf1(3.5, 8.0);

	std::cout << "number= " << cf1.real() << '\n' << cf1.imag() << '\n';
	std::cout << "abs= " 	<< custom::abs(cf1) << '\n';
	std::cout << "norm= " 	<< custom::norm(cf1) << '\n';
	std::cout << "conj= " 	<< custom::conj(cf1) << '\n';
	std::cout << "proj= " 	<< custom::proj(cf1) << '\n';
	std::cout << "polar= " 	<< custom::polar(-2.5, 4.3) << '\n';

	std::cout << "exp= " 	<< custom::exp(cf1) << '\n';
	std::cout << "log= " 	<< custom::log(cf1) << '\n';
	std::cout << "log10= " 	<< custom::log10(cf1) << '\n';
	std::cout << "sqrt= " 	<< custom::sqrt(cf1) << '\n';
	std::cout << "pow c2= "	<< custom::pow(cf1, 2.0f) << '\n';
	std::cout << "pow 2c= "	<< custom::pow(2.0f, cf1) << '\n';
	std::cout << "pow cc= "	<< custom::pow(cf1, cf1) << '\n';

	std::cout << "sin= " 	<< custom::sin(cf1) << '\n';
	std::cout << "asin= " 	<< custom::asin(cf1) << '\n';
	std::cout << "sinh= " 	<< custom::sinh(cf1) << '\n';
	std::cout << "asinh= " 	<< custom::asinh(cf1) << '\n';

	std::cout << "cos= " 	<< custom::cos(cf1) << '\n';
	std::cout << "acos= " 	<< custom::acos(cf1) << '\n';
	std::cout << "cosh= " 	<< custom::cosh(cf1) << '\n';
	std::cout << "acosh= " 	<< custom::acosh(cf1) << '\n';

	std::cout << "tan= " 	<< custom::tan(cf1) << '\n';
	std::cout << "atan= " 	<< custom::atan(cf1) << '\n';
	std::cout << "tanh= " 	<< custom::tanh(cf1) << '\n';
	std::cout << "atanh= " 	<< custom::atanh(cf1) << '\n';
}

void bitset_test()
{
	std::string str1 = "aBaaBBaB";
    std::bitset<8> bs1(str1, 0, str1.size(), 'a', 'B');
	std::cout << bs1 << '\n' << bs1.count() << '\n';

	custom::string str2 = "aBaaBBaB";
	custom::bitset<8> bs2(str2, 0, str2.size(), 'a', 'B');
	std::cout << bs2 << '\n' << bs2.count() << '\n';
}

TEST_END