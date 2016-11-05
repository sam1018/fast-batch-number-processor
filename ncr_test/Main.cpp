/*
	tests class number_sets
*/

#include "number_sets.h"

#include <assert.h>

#include <fstream>

using namespace std;

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

	auto res = x.get_invalid_inputs();
	assert(invalid_inputs.size() == res.size());

	for (size_t i = 0; i < invalid_inputs.size(); ++i)
		assert(invalid_inputs[i] == res[i]);
}

void test_different_types()
{
	vector<string> inputs = {
		to_string(0),
		to_string(INT_MAX),
		to_string(INT_MIN),
		to_string(UINT_MAX),
		to_string(_I64_MAX),
		to_string(_I64_MIN),
		to_string(_UI64_MAX)
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

	auto compare = [](auto& v1, auto& v2) {
		assert(v1.size() == v2.size());

		for (size_t i = 0; i < v1.size(); ++i)
			assert(v1[i] == v2[i]);
	};

	compare(d.get_invalid_inputs(), vector<string>({ to_string(UINT_MAX), to_string(_I64_MAX), to_string(_I64_MIN), to_string(_UI64_MAX) }));
	compare(ud.get_invalid_inputs(), vector<string>({ to_string(INT_MIN), to_string(_I64_MAX), to_string(_I64_MIN), to_string(_UI64_MAX) }));
	compare(lld.get_invalid_inputs(), vector<string>({ to_string(_UI64_MAX) }));
	compare(ulld.get_invalid_inputs(), vector<string>({ to_string(INT_MIN), to_string(_I64_MIN) }));

	//number_sets<float> f; // should give static_assert, float is not an integral type
	//number_sets<string> s; // should give static_assert, string is not an integral type
}

int main()
{
	process_input_file();

	test_invalid_inputs();

	test_different_types();
}
