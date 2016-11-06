#include "number_sets.h"

#include <assert.h>

#include <fstream>

using namespace std;


/*
	tests class number_sets
*/


void process_input_file()
{
	number_sets<int> x;

	ifstream infile("input.txt");

	string line;
	while (getline(infile, line))
	{
		if (!x.add(line))
			cout << "Invalid input: " << line << "\n";
	}


}

void test_invalid_inputs()
{
	number_sets<int> x;

	vector<string> valid_inputs = {
		"1",
		"1,2",
		"1,2,3,4",
		"1       ,           2,          3,               4                ",
		"123456789,     12",
		"-1234, 1324",
		"0, 0, 0, 0"
	};

	vector<string> invalid_inputs = {
		"",
		"       ,       ,      ",
		"avcd adsad gfdg 2132132",
		"1000000000000",
		"123,,123",
		"123 123",
		"123.456",
		"1,2,3,4s,6"
	};

	for (size_t i = 0; i < valid_inputs.size(); ++i)
		assert(x.add(valid_inputs[i]) == true);

	for (size_t i = 0; i < invalid_inputs.size(); ++i)
		assert(x.add(invalid_inputs[i]) == false);

	assert(x.get_invalid_inputs() == invalid_inputs);
}

void test_different_integral_types()
{
	vector<string> inputs = {
		to_string(0),
		to_string(INT_MAX),
		to_string(INT_MIN),
		to_string(UINT_MAX),
		to_string(_I64_MAX),
		to_string(_I64_MIN),
		to_string(_UI64_MAX),
		"123.456"
	};

	number_sets<int> d;
	number_sets<unsigned int> ud;
	number_sets<long long int> lld;
	number_sets<unsigned long long int> ulld;

	for (size_t i = 0; i < inputs.size(); ++i)
	{
		d.add(inputs[i]);
		ud.add(inputs[i]);
		lld.add(inputs[i]);
		ulld.add(inputs[i]);
	}

	assert(d.get_invalid_inputs()		== vector<string>({ to_string(UINT_MAX), to_string(_I64_MAX), to_string(_I64_MIN), to_string(_UI64_MAX), "123.456" }));
	assert(ud.get_invalid_inputs()		== vector<string>({ to_string(INT_MIN), to_string(_I64_MAX), to_string(_I64_MIN), to_string(_UI64_MAX), "123.456" }));
	assert(lld.get_invalid_inputs()		== vector<string>({ to_string(_UI64_MAX), "123.456" }));
	assert(ulld.get_invalid_inputs()	== vector<string>({ to_string(INT_MIN), to_string(_I64_MIN), "123.456" }));

	// following should static_assert
	//number_sets<float> f; // should give static_assert, float is not an integral type
	//number_sets<string> s; // should give static_assert, string is not an integral type
}

template<typename CharT>
vector<basic_string<CharT>> get_test_vector_string()
{
	return vector<basic_string<CharT>>{"123, 456, 789", "123, 456, 789"};
}

template<>
vector<basic_string<wchar_t>> get_test_vector_string()
{
	return vector<basic_string<wchar_t>>{L"123, 456, 789", L"123, 456, 789"};
}

template<typename CharT>
void test_different_char_types_typed()
{
	number_sets<int, CharT> d;

	vector<basic_string<CharT>> inputs = {  };

	for(auto s : get_test_vector_string<CharT>())
		assert(d.add(s) == true);
}

void test_different_char_types()
{
	test_different_char_types_typed<char>();
	test_different_char_types_typed<wchar_t>();
}

int main()
{
	process_input_file();

	test_invalid_inputs();

	test_different_integral_types();

	test_different_char_types();
}
