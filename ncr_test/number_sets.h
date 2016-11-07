#pragma once

#include "routines.h"
#include "parse_ints_fast.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_set>

/*
	File: number_sets.h
	Description: header only class number_sets

	Design Decisions:
	*Data type templatized*
		* The question mentions integer type... by making type as template, we allow ourselves the flexibility of having signed, unsigned, long long etc.
		* Only integral type is allowed... ensured by using static_assert
	*Non copyable*
		* Due to one pointer member - most_frequent, we need to take care of copy-ability
		* Making class non-copyable, as not required by the question
	*Container type:
		* *All number sets container is unordered_set*: Using unordered_set, STL implementation for hash table... hash value is calculated based on the numbers in a set
		* *Individual number set container is sorted vector*: Sorted vector is used to hold the numbers in a set... 
			sorting is required because, the order of numbers alter hash value... also sets may be compared for euqlity by unordered_set
*/

template<typename T>
struct number_set
{
	std::vector<T> numbers;

	// unordered_set doesn't allow modifying content
	// but as we know, occurances doesn't participate in hash key generation,
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
	interface for class number_sets
*/

template<typename T, typename CharT = char>
class number_sets : private noncopyable
{
public:
	// type definitions
	using string_type = std::basic_string<CharT>;
	using data_container_type = std::unordered_set<number_set<T>, hasher<T>>;
	using const_ref_data_container_type = const std::unordered_set<number_set<T>, hasher<T>>&;
	using invalid_inputs_type = std::vector<std::basic_string<CharT>>;
	using const_ref_invalid_inputs_type = const std::vector<std::basic_string<CharT>>&;

private:
	// variables
	data_container_type data;
	invalid_inputs_type invalid_inputs;
	const number_set<T>* most_frequent;
	int duplicate_count;
	int non_duplicate_count;

public:
	// constructor
	number_sets();

	// modifiers

	// returns false for duplicate... true otherwise
	bool add(const string_type& input);

	// getters
	const_ref_invalid_inputs_type get_invalid_inputs() const;
	const number_set<T>* get_most_frequent_data() const;
	int get_duplicate_count() const;
	int get_non_duplicate_count() const;
	const_ref_data_container_type get_data() const;

private:
	std::vector<T> get_sorted_number_set(const string_type& input);
};


/*
	Implementation for class number_sets
*/

template<typename T, typename CharT>
number_sets<T, CharT>::number_sets() :
	most_frequent(nullptr),
	duplicate_count(0),
	non_duplicate_count(0)
{
	static_assert(std::is_integral<T>::value, "Integral type required.");
}

template<typename T, typename CharT>
bool number_sets<T, CharT>::add(const string_type& input)
{
	auto res = data.emplace(get_sorted_number_set(input));
	if (!res.second)
		res.first->occurences++;

	if (!most_frequent || most_frequent->occurences < res.first->occurences)
		most_frequent = &(*res.first);

	if (res.first->occurences == 1)
		non_duplicate_count++;
	else if (res.first->occurences == 2)
	{
		duplicate_count += 2;
		non_duplicate_count--;
	}
	else
		duplicate_count++;

	return res.first->occurences == 1;
}

template<typename T, typename CharT>
typename number_sets<T, CharT>::const_ref_invalid_inputs_type number_sets<T, CharT>::get_invalid_inputs() const
{
	return invalid_inputs;
}

template<typename T, typename CharT>
const number_set<T>* number_sets<T, CharT>::get_most_frequent_data() const
{
	return most_frequent;
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

// fast parsing, for int output and char input
template<>
std::vector<int> get_numbers<int, char>(const std::string& input)
{
	parse_ints_fast parser;
	return parser.get_values(input);
}

template<typename T, typename CharT>
std::vector<T> number_sets<T, CharT>::get_sorted_number_set(const string_type& input)
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
		invalid_inputs.push_back(input);
		throw std::runtime_error("Invalid input");
	}

	sort(numbers.begin(), numbers.end());

	return numbers;
}

template<typename T, typename CharT>
int number_sets<T, CharT>::get_duplicate_count() const
{
	return duplicate_count;
}

template<typename T, typename CharT>
int number_sets<T, CharT>::get_non_duplicate_count() const
{
	return non_duplicate_count;
}

template<typename T, typename CharT>
typename number_sets<T, CharT>::const_ref_data_container_type number_sets<T, CharT>::get_data() const
{
	return data;
}
