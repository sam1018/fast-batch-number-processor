#include "number_sets.h"

#include <assert.h>

#include <random>
#include <fstream>
#include <unordered_map>

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
		try
		{
			x.add(line);
		}
		catch (exception& e)
		{
			cout << e.what() << ": " << line << "\n";
		}
	}

	auto most_frequent = x.get_most_frequent_data();

	cout << "\nMost frequent: Occurence " << most_frequent->occurences << "\nData: ";

	for (auto x : most_frequent->numbers)
		cout << x << " ";

	cout << "\n";

	cout << "Duplicates: " << x.get_duplicate_count() << " Non Duplicates: " << x.get_non_duplicate_count() << "\n";

	vector<number_set<int>> sets;

	for (auto item : x.get_data())
		sets.push_back(number_set<int>{item.numbers, item.occurences});

	sort(sets.begin(), sets.end(), [](const auto& a, const auto& b) {
		return a.occurences > b.occurences;
	});

	cout << "\n";
	cout << "Occurences\tNumber Set\n";
	int count = 0;
	for (auto item : sets)
	{
		count += item.occurences;
		cout << item.occurences << "\t\t" << item.numbers << "\n";
	}

	cout << "Total: " << count << "\n";
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
	{
		try
		{
			x.add(valid_inputs[i]);
		}
		catch (...)
		{
			assert(false); // add should be successful... shouldn't throw...
		}
	}

	for (size_t i = 0; i < invalid_inputs.size(); ++i)
	{
		try
		{
			x.add(invalid_inputs[i]);
			assert(false); // shouldn't succeed
		}
		catch (...)
		{
		}
	}

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

	auto f = [](auto& set, auto& input) {
		try
		{
			set.add(input);
		}
		catch (...) {}
	};

	for (size_t i = 0; i < inputs.size(); ++i)
	{
		f(d, inputs[i]);
		f(ud, inputs[i]);
		f(lld, inputs[i]);
		f(ulld, inputs[i]);
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

	for (auto s : get_test_vector_string<CharT>())
	{
		try
		{
			d.add(s);
		}
		catch (...)
		{
			assert(false); // shouldn't throw
		}
	}
}

void test_different_char_types()
{
	test_different_char_types_typed<char>();
	test_different_char_types_typed<wchar_t>();
}

void test_duplicates()
{
	number_sets<int> d;

	d.add("1");
	assert(d.get_duplicate_count() == 0 && d.get_non_duplicate_count() == 1);

	d.add("1");
	assert(d.get_duplicate_count() == 2 && d.get_non_duplicate_count() == 0);

	d.add("1");
	assert(d.get_duplicate_count() == 3 && d.get_non_duplicate_count() == 0);

	vector<int> vi;

	for (int i = 0; i < 100; ++i)
		vi.push_back(rand());

	for (int i = 0; i < 100; ++i)
	{
		std::random_device rd;
		std::mt19937 g(rd());

		std::shuffle(vi.begin(), vi.end(), g);

		stringstream ss;

		for (int i = 0; i < vi.size(); ++i)
		{
			if (i != 0)
				ss << ", ";
			ss << vi[i];
		}

		d.add(ss.str());
	}
}

int main()
{
	cout << "Input file results:\n";
	process_input_file();

	cout << "\nPerforming Tests (Will assert on fail)...\n";
	test_invalid_inputs();

	test_different_integral_types();

	test_different_char_types();

	test_duplicates();

	cout << "Successfully ran all tests.\n";
}
