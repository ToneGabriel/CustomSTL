#pragma once

#include "__CTMCommonInclude.h"
#include "__STDCommonInclude.h"

#define TEST_BEGIN namespace test {
#define TEST_END }

TEST_BEGIN

class Test      // basic test class
{
public:
	int TestValue = 0;

public:
    // Constructors & Operators

	Test();
	Test(int x);
	Test(const Test& other);
	Test(Test&& other) noexcept;
	~Test();

	Test& operator=(const Test& other);
	Test& operator=(Test&& other) noexcept;
	bool operator==(const Test& other) const;
	bool operator!=(const Test& other) const;
	void operator()() const;

	friend std::ostream& operator<<(std::ostream&, const Test&);
	friend bool operator<(const Test&, const Test&);

public:
    // Main functions

	int test_function(int x);
};  // END Test


void print_sequence_test();
void piecewise_pair_test();
void tuple_test();
void list_test();
void forward_list_test();
void unordered_map_test();
void unordered_set_test();
void map_test();
void deque_test();
void queue_test();
void priority_queue_test();
void stack_test();
void vector_test();
void string_test();
void string_view_test();
void array_test();
void function_test();
void invoke_test();
void memory_test();
void chrono_test();
void complex_test();
void bitset_test();

template<typename T, T... ints>
void print_sequence(custom::integer_sequence<T, ints...> int_seq)
{
	std::cout << "The sequence of size " << int_seq.size() << ": ";
	((std::cout << ints << ' '), ...);
	std::cout << '\n';
}

template<class Type>
void print_numeric_limits_statics() {
	std::cout << std::numeric_limits<Type>::round_style			<< ' ' << custom::numeric_limits<Type>::RoundStyle << '\n';
	std::cout << std::numeric_limits<Type>::has_denorm			<< ' ' << custom::numeric_limits<Type>::HasDenorm << '\n';
	std::cout << std::numeric_limits<Type>::has_denorm_loss		<< ' ' << custom::numeric_limits<Type>::HasDenormLoss << '\n';
	std::cout << std::numeric_limits<Type>::has_infinity		<< ' ' << custom::numeric_limits<Type>::HasInfinity << '\n';
	std::cout << std::numeric_limits<Type>::has_quiet_NaN		<< ' ' << custom::numeric_limits<Type>::has_quiet_NaN << '\n';
	std::cout << std::numeric_limits<Type>::has_signaling_NaN	<< ' ' << custom::numeric_limits<Type>::has_signaling_NaN << '\n';
	std::cout << std::numeric_limits<Type>::is_specialized		<< ' ' << custom::numeric_limits<Type>::is_specialized << '\n';
	std::cout << std::numeric_limits<Type>::is_signed			<< ' ' << custom::numeric_limits<Type>::is_signed << '\n';
	std::cout << std::numeric_limits<Type>::is_integer			<< ' ' << custom::numeric_limits<Type>::is_integer << '\n';
	std::cout << std::numeric_limits<Type>::is_exact			<< ' ' << custom::numeric_limits<Type>::is_exact << '\n';
	std::cout << std::numeric_limits<Type>::is_iec559			<< ' ' << custom::numeric_limits<Type>::is_iec559 << '\n';
	std::cout << std::numeric_limits<Type>::is_bounded			<< ' ' << custom::numeric_limits<Type>::is_bounded << '\n';
	std::cout << std::numeric_limits<Type>::is_modulo			<< ' ' << custom::numeric_limits<Type>::is_modulo << '\n';
	std::cout << std::numeric_limits<Type>::traps				<< ' ' << custom::numeric_limits<Type>::traps << '\n';
	std::cout << std::numeric_limits<Type>::tinyness_before		<< ' ' << custom::numeric_limits<Type>::tinyness_before << '\n';
	std::cout << std::numeric_limits<Type>::digits				<< ' ' << custom::numeric_limits<Type>::digits << '\n';
	std::cout << std::numeric_limits<Type>::digits10			<< ' ' << custom::numeric_limits<Type>::digits10 << '\n';
	std::cout << std::numeric_limits<Type>::max_digits10		<< ' ' << custom::numeric_limits<Type>::max_digits10 << '\n';
	std::cout << std::numeric_limits<Type>::radix				<< ' ' << custom::numeric_limits<Type>::radix << '\n';
	std::cout << std::numeric_limits<Type>::min_exponent		<< ' ' << custom::numeric_limits<Type>::min_exponent << '\n';
	std::cout << std::numeric_limits<Type>::min_exponent10		<< ' ' << custom::numeric_limits<Type>::min_exponent10 << '\n';
	std::cout << std::numeric_limits<Type>::max_exponent		<< ' ' << custom::numeric_limits<Type>::max_exponent << '\n';
	std::cout << std::numeric_limits<Type>::max_exponent10		<< ' ' << custom::numeric_limits<Type>::max_exponent10 << '\n';
}

template<class Type>
void print_numeric_limits_functions() {
	std::cout << (std::numeric_limits<Type>::min)()				<< ' ' << (custom::numeric_limits<Type>::min)() << '\n';
	std::cout << (std::numeric_limits<Type>::max)()				<< ' ' << (custom::numeric_limits<Type>::max)() << '\n';
	std::cout << std::numeric_limits<Type>::lowest()			<< ' ' << custom::numeric_limits<Type>::lowest() << '\n';
	std::cout << std::numeric_limits<Type>::epsilon()			<< ' ' << custom::numeric_limits<Type>::epsilon() << '\n';
	std::cout << std::numeric_limits<Type>::round_error()		<< ' ' << custom::numeric_limits<Type>::round_error() << '\n';
	std::cout << std::numeric_limits<Type>::infinity()			<< ' ' << custom::numeric_limits<Type>::infinity() << '\n';
	std::cout << std::numeric_limits<Type>::quiet_NaN()			<< ' ' << custom::numeric_limits<Type>::quiet_NaN() << '\n';
	std::cout << std::numeric_limits<Type>::signaling_NaN()		<< ' ' << custom::numeric_limits<Type>::signaling_NaN() << '\n';
	std::cout << std::numeric_limits<Type>::denorm_min()		<< ' ' << custom::numeric_limits<Type>::denorm_min() << '\n';
}

TEST_END