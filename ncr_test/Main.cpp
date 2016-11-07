#include "number_sets.h"

#include <assert.h>

#include <map>
#include <chrono>
#include <random>
#include <fstream>
#include <unordered_map>

using namespace std;
using namespace std::chrono;


/*
	tests class number_sets
*/

// a lame version of checking duplicate number sets
// for cross checking
vector<number_set<int>> simple_number_sets_impl(ifstream& infile)
{
	string line;
	vector<int> numbers;
	map<vector<int>, int> all_sets;

	while (getline(infile, line))
	{
		numbers.clear();
		try
		{
			string token;
			auto ss = stringstream(line);
			while (std::getline(ss, token, ','))
			{
				numbers.push_back(convertTo<int>(token));
			}

			sort(numbers.begin(), numbers.end());

			if (all_sets.find(numbers) == all_sets.end())
				all_sets[numbers] = 1;
			else
				all_sets[numbers]++;
		}
		catch (...)
		{
		}
	}

	vector<number_set<int>> res;

	for (auto x : all_sets)
		res.push_back(number_set<int>{ x.first, x.second });

	sort(res.begin(), res.end(), [](const auto& a, const auto& b) {
		if (a.occurences == b.occurences)
			if (a.numbers.size() == b.numbers.size())
				return a.numbers[0] > b.numbers[0];
			else
				return a.numbers.size() > b.numbers.size();
		return a.occurences > b.occurences;
	});

	return res;
}

void process_input_file(ifstream& infile)
{
	number_sets<int> x;

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

void test_input_file_result(ifstream& infile)
{
	number_sets<int> x;

	string line;
	while (getline(infile, line))
	{
		try
		{
			x.add(line);
		}
		catch (exception&)
		{
		}
	}

	vector<number_set<int>> sets;

	for (auto item : x.get_data())
		sets.push_back(number_set<int>{item.numbers, item.occurences});

	sort(sets.begin(), sets.end(), [](const auto& a, const auto& b) {
		if (a.occurences == b.occurences)
			if (a.numbers.size() == b.numbers.size())
				return a.numbers[0] > b.numbers[0];
			else
				return a.numbers.size() > b.numbers.size();
		return a.occurences > b.occurences;
	});

	infile.clear();
	infile.seekg(0, ios::beg);

	assert(sets == simple_number_sets_impl(infile));
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
		"1,2,3,4s,6",
		"123,456,12-456"
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

		for (size_t i = 0; i < vi.size(); ++i)
		{
			if (i != 0)
				ss << ", ";
			ss << vi[i];
		}

		d.add(ss.str());
	}

	d.add("2, 3, 4");

	assert(d.get_duplicate_count() == 103 && d.get_non_duplicate_count() == 1);
}

void test_non_copyable()
{
	// following code should fail to compile due to being non copyable
	//number_sets<int> a;

	//number_sets<int> b(a);

	//number_sets<int> c;

	//c = a;
}

void generate_large_data_set(const string& filename, int total_lines, int nums_per_line, int range)
{
	ofstream ofile(filename);

	for (int line = 0; line < total_lines; ++line)
	{
		for (int i = 0; i < nums_per_line; ++i)
		{
			if (i != 0)
				ofile << ", ";
			ofile << rand() % range;
		}
		ofile << "\n";
	}
}

void test_large_data_set()
{
	string filename = "large_data.txt";
	const int total_lines = 1'000'000;
	const int nums_per_line = 10;
	const int range = 20;

	cout << "Generating large data set\n";
	cout << "Data set size: sets-" << total_lines << ", numbers per set-" << nums_per_line << "\n";
	generate_large_data_set(filename, total_lines, nums_per_line, range);
	cout << "Finished generating large data set\n";


	cout << "Creating number sets\n";
	system_clock::time_point start = system_clock::now();

	ifstream ifile(filename);
	number_sets<int> sets;

	string line;
	while (getline(ifile, line))
		sets.add(line);

	cout << "Finished\n";
	cout << "Duplicates: " << sets.get_duplicate_count() << " Non duplicates: " << sets.get_non_duplicate_count() << "\n";
	auto most_frequent = sets.get_most_frequent_data();
	cout << "Most Frequent: Occurence-" << most_frequent->occurences << " numbers-" << most_frequent->numbers << "\n";

	system_clock::time_point end = system_clock::now();

	cout << "Time taken: " << duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0 << "s\n";
}

int main()
{
	cout << "Input file results:\n";
	process_input_file(ifstream("input.txt"));

	cout << "\n";

#ifdef _DEBUG
	// Run in debug mode to perform these tests...
	cout << "Performing Tests (Will assert on fail)...\n";

	test_input_file_result(ifstream("input.txt"));

	test_invalid_inputs();

	test_different_integral_types();

	test_different_char_types();

	test_duplicates();

	test_non_copyable();

	cout << "Successfully ran all tests.\n";

#else
	// This is too slow to run in debug mode
	test_large_data_set();

#endif // _DEBUG

}
