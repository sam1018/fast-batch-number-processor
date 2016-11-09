#pragma once

#include "routines.h"

#include <unordered_set>

/*
	Contains implementation details for number_sets class
*/

namespace ncr_test
{
	/*
		number_set structure
	*/
	template<typename T>
	struct number_set
	{
		std::vector<T> numbers;

		// unordered_set doesn't allow modifying content
		// but as we know, occurances don't participate in hash key generation,
		// so we can safely make it mutable
		mutable int occurences;

		number_set(const std::vector<T>& _numbers, int _occ = 1) :
			numbers(_numbers),
			occurences(_occ)
		{}
	};

	template<typename T>
	struct hasher
	{
		std::size_t operator()(number_set<T> const& s) const
		{
			return hash_value(s.numbers);
		}
	};

	template<typename T>
	bool operator==(const number_set<T>& lhs, const number_set<T>& rhs)
	{
		return lhs.numbers == rhs.numbers;
	}



	/*
		struct number_sets_data
	*/

	template<typename T, typename CharT = char>
	struct number_sets_data : private noncopyable
	{
		// type definitions
		using string_type = std::basic_string<CharT>;
		using data_container_type = std::unordered_set<number_set<T>, hasher<T>>;
		using const_ref_data_container_type = const std::unordered_set<number_set<T>, hasher<T>>&;
		using invalid_inputs_type = std::vector<std::basic_string<CharT>>;
		using const_ref_invalid_inputs_type = const std::vector<std::basic_string<CharT>>&;


		// variables
		data_container_type number_sets;
		invalid_inputs_type invalid_inputs;
		const number_set<T>* most_frequent;
		int duplicate_count;
		int non_duplicate_count;

		// ctor
		number_sets_data() :
			most_frequent(nullptr),
			duplicate_count(0),
			non_duplicate_count(0)
		{}
	};


	template<typename T, typename CharT>
	bool consume_number_set(const std::vector<T>& input, number_sets_data<T, CharT> &data)
	{
		bool ret = false;

		auto res = data.number_sets.emplace(input);
		if (!res.second)
			res.first->occurences++;

		if (!data.most_frequent || data.most_frequent->occurences < res.first->occurences)
			data.most_frequent = &(*res.first);

		if (res.first->occurences == 1)
			data.non_duplicate_count++;
		else if (res.first->occurences == 2)
		{
			data.duplicate_count += 2;
			data.non_duplicate_count--;
		}
		else
			data.duplicate_count++;

		ret = (res.first->occurences == 1);

		return ret;
	}


	// default parsing using C++ stringstream
	template<typename T, typename CharT>
	std::vector<T> get_numbers(const std::basic_string<CharT>& input)
	{
		std::vector<T> numbers;

		std::basic_stringstream<CharT> ss(input);
		std::basic_string<CharT> token;

		// splitting input based on comma
		while (std::getline(ss, token, CharT(',')))
			numbers.push_back(convertTo<T>(token));

		return numbers;
	}

	template<>
	std::vector<int> get_numbers<int, char>(const std::string& input);

	template<typename T, typename CharT>
	std::vector<T> produce_number_set(const std::basic_string<CharT>& input)
	{
		std::vector<T> numbers;
		bool add_failed = false;

		try
		{
			numbers = get_numbers<T, CharT>(input);
		}
		catch (std::runtime_error&)
		{
			add_failed = true;
		}

		if (numbers.empty())
			add_failed = true;

		if (add_failed)
		{
			throw std::runtime_error("Invalid input");
		}

		sort(numbers.begin(), numbers.end());

		return numbers;
	}

	/*
	Concurrent implementation to add numbers sets
	*/

	void add_number_sets_concurrent(const std::string& filename, number_sets_data<int, char> &data, int producers_count);
}
