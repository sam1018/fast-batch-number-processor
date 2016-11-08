#pragma once

#include "routines.h"
#include "parse_ints_fast.h"
#include "number_sets_data.h"
#include "add_number_sets_parallel.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <type_traits>
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



/*
	interface for class number_sets
*/

template<typename T, typename CharT = char>
class number_sets : private noncopyable
{
public:
	using data_type = number_sets_data<T, CharT>;
	using string_type = typename data_type::string_type;
	using const_ref_invalid_inputs_type = typename data_type::const_ref_invalid_inputs_type;
	using const_ref_data_container_type = typename data_type::const_ref_data_container_type;

private:
	data_type data;

public:
	// constructor
	number_sets();

	// modifiers

	// returns false for duplicate... true otherwise
	bool add(const string_type& input);
	// another add mechanism that works on the whole input file
	// uses paraller programming to improve performance
	// for now, only implemented for T = int and CharT = char
	bool add_batch_mode(const string_type& filename, int producer_count);

	// getters
	const_ref_invalid_inputs_type get_invalid_inputs() const;
	const number_set<T> get_most_frequent_data() const;
	int get_duplicate_count() const;
	int get_non_duplicate_count() const;
	const_ref_data_container_type get_data() const;

private:
	std::vector<T> produce_number_set(const string_type& input);
	bool consume_number_set(const std::vector<T>& input);
};


/*
	Implementation for class number_sets
*/

template<typename T, typename CharT>
number_sets<T, CharT>::number_sets()
{
	static_assert(std::is_integral<T>::value, "Integral type required.");
}

template<typename T, typename CharT>
bool number_sets<T, CharT>::add(const string_type& input)
{
	bool ret;

	try
	{
		ret = consume_number_set(produce_number_set(input));
	}
	catch (...)
	{
		data.invalid_inputs.push_back(input);
		throw;
	}

	return ret;
}

template<typename T, typename CharT>
typename number_sets<T, CharT>::const_ref_invalid_inputs_type number_sets<T, CharT>::get_invalid_inputs() const
{
	return data.invalid_inputs;
}

template<typename T, typename CharT>
const number_set<T> number_sets<T, CharT>::get_most_frequent_data() const
{
	if (data.most_frequent)
		return *data.most_frequent;

	return number_set<T>{vector<T>{}, 0};
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
std::vector<T> number_sets<T, CharT>::produce_number_set(const string_type& input)
{
	return ::produce_number_set<T, CharT>(input);
}

template<typename T, typename CharT>
int number_sets<T, CharT>::get_duplicate_count() const
{
	return data.duplicate_count;
}

template<typename T, typename CharT>
int number_sets<T, CharT>::get_non_duplicate_count() const
{
	return data.non_duplicate_count;
}

template<typename T, typename CharT>
typename number_sets<T, CharT>::const_ref_data_container_type number_sets<T, CharT>::get_data() const
{
	return data.number_sets;
}

template<typename T, typename CharT>
bool number_sets<T, CharT>::consume_number_set(const std::vector<T>& input)
{
	return ::consume_number_set<T, CharT>(input, data);
}

template<typename T, typename CharT>
bool number_sets<T, CharT>::add_batch_mode(const string_type& filename, int producer_count)
{
	static_assert(std::is_same<T, int>::value && std::is_same<CharT, char>::value, "only T = int and CharT = char is accepted");

	add_number_sets_parallel(filename, data, producer_count);

	return true;
}
