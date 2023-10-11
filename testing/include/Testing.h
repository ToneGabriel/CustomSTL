#pragma once

#pragma region Common tests
#include "__CTMCommonInclude.h"
#include "__STDCommonInclude.h"

#define TEST_BEGIN namespace test {
#define TEST_END }


TEST_BEGIN


#define TEST_HELP_BEGIN namespace test_help {
#define TEST_HELP_END }


TEST_HELP_BEGIN   // helpers (not intended for external use)


class Test      // basic test class
{
public:
	int value = 0;

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

	friend std::ostream& operator<<(std::ostream& os, const Test& test);

public:
    // Main functions

	int test_function(int x);
};  // END Test


TEST_HELP_END

void print_sequence_test();
void piecewise_pair_test();
void tuple_test();
void list_test();
void unordered_map_test();
void unordered_set_test();
void map_test();
void deque_test();
void queue_test();
void priority_queue_test();
void stack_test();
void vector_test();
void string_test();
void array_test();
void function_test();
void invoke_test();
void memory_test();
void chrono_test();

template<typename T, T... ints>
void print_sequence(custom::IntegerSequence<T, ints...> int_seq)
{
	std::cout << "The sequence of size " << int_seq.size() << ": ";
	((std::cout << ints << ' '), ...);
	std::cout << '\n';
}

template<class Type>
void print_numeric_limits_statics() {
	std::cout << std::numeric_limits<Type>::round_style			<< ' ' << custom::NumericLimits<Type>::RoundStyle << '\n';
	std::cout << std::numeric_limits<Type>::has_denorm			<< ' ' << custom::NumericLimits<Type>::HasDenorm << '\n';
	std::cout << std::numeric_limits<Type>::has_denorm_loss		<< ' ' << custom::NumericLimits<Type>::HasDenormLoss << '\n';
	std::cout << std::numeric_limits<Type>::has_infinity		<< ' ' << custom::NumericLimits<Type>::HasInfinity << '\n';
	std::cout << std::numeric_limits<Type>::has_quiet_NaN		<< ' ' << custom::NumericLimits<Type>::HasQuietNaN << '\n';
	std::cout << std::numeric_limits<Type>::has_signaling_NaN	<< ' ' << custom::NumericLimits<Type>::HasSignalingNaN << '\n';
	std::cout << std::numeric_limits<Type>::is_specialized		<< ' ' << custom::NumericLimits<Type>::IsSpecialized << '\n';
	std::cout << std::numeric_limits<Type>::is_signed			<< ' ' << custom::NumericLimits<Type>::IsSigned << '\n';
	std::cout << std::numeric_limits<Type>::is_integer			<< ' ' << custom::NumericLimits<Type>::IsInteger << '\n';
	std::cout << std::numeric_limits<Type>::is_exact			<< ' ' << custom::NumericLimits<Type>::IsExact << '\n';
	std::cout << std::numeric_limits<Type>::is_iec559			<< ' ' << custom::NumericLimits<Type>::IsIEC559 << '\n';
	std::cout << std::numeric_limits<Type>::is_bounded			<< ' ' << custom::NumericLimits<Type>::IsBounded << '\n';
	std::cout << std::numeric_limits<Type>::is_modulo			<< ' ' << custom::NumericLimits<Type>::IsModulo << '\n';
	std::cout << std::numeric_limits<Type>::traps				<< ' ' << custom::NumericLimits<Type>::Traps << '\n';
	std::cout << std::numeric_limits<Type>::tinyness_before		<< ' ' << custom::NumericLimits<Type>::TinynessBefore << '\n';
	std::cout << std::numeric_limits<Type>::digits				<< ' ' << custom::NumericLimits<Type>::Digits << '\n';
	std::cout << std::numeric_limits<Type>::digits10			<< ' ' << custom::NumericLimits<Type>::Digits10 << '\n';
	std::cout << std::numeric_limits<Type>::max_digits10		<< ' ' << custom::NumericLimits<Type>::MaxDigits10 << '\n';
	std::cout << std::numeric_limits<Type>::radix				<< ' ' << custom::NumericLimits<Type>::Radix << '\n';
	std::cout << std::numeric_limits<Type>::min_exponent		<< ' ' << custom::NumericLimits<Type>::MinExponent << '\n';
	std::cout << std::numeric_limits<Type>::min_exponent10		<< ' ' << custom::NumericLimits<Type>::MinExponent10 << '\n';
	std::cout << std::numeric_limits<Type>::max_exponent		<< ' ' << custom::NumericLimits<Type>::MaxExponent << '\n';
	std::cout << std::numeric_limits<Type>::max_exponent10		<< ' ' << custom::NumericLimits<Type>::MaxExponent10 << '\n';
}

template<class Type>
void print_numeric_limits_functions() {
	std::cout << (std::numeric_limits<Type>::min)()				<< ' ' << (custom::NumericLimits<Type>::min)() << '\n';
	std::cout << (std::numeric_limits<Type>::max)()				<< ' ' << (custom::NumericLimits<Type>::max)() << '\n';
	std::cout << std::numeric_limits<Type>::lowest()			<< ' ' << custom::NumericLimits<Type>::lowest() << '\n';
	std::cout << std::numeric_limits<Type>::epsilon()			<< ' ' << custom::NumericLimits<Type>::epsilon() << '\n';
	std::cout << std::numeric_limits<Type>::round_error()		<< ' ' << custom::NumericLimits<Type>::round_error() << '\n';
	std::cout << std::numeric_limits<Type>::infinity()			<< ' ' << custom::NumericLimits<Type>::infinity() << '\n';
	std::cout << std::numeric_limits<Type>::quiet_NaN()			<< ' ' << custom::NumericLimits<Type>::quiet_NaN() << '\n';
	std::cout << std::numeric_limits<Type>::signaling_NaN()		<< ' ' << custom::NumericLimits<Type>::signaling_NaN() << '\n';
	std::cout << std::numeric_limits<Type>::denorm_min()		<< ' ' << custom::NumericLimits<Type>::denorm_min() << '\n';
}


TEST_END
#pragma endregion Common tests


#pragma region Thread tests
#if defined __GNUG__    // thread tests available only on __GNUG__
#include "__CTMThreadInclude.h"
#include "__STDThreadInclude.h"

TEST_BEGIN


TEST_HELP_BEGIN		// helpers (not intended for external use)


struct Employee
{
    std::string _ID;
    custom::Vector<std::string> _LunchPartners;
    custom::Mutex _Mutex;
    
    Employee(std::string id) : _ID(id) { /*Empty*/ }

    std::string output() const;
};  // END Employee


void send_mail(Employee &, Employee &);
void assign_lunch_partner(Employee &e1, Employee &e2);


TEST_HELP_END


void lock_locks_test();
void thread_test();


TEST_END
#endif
#pragma endregion Thread tests



//STD_BEGIN
//template<>
//struct less<test::test_help::Test>
//{
//	bool operator()(const test::test_help::Test& left, const test::test_help::Test& right) const {
//		return left.value < right.value;
//	}
//};
//STD_END