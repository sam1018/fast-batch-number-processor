#pragma once

#include "routines.h"

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
	*Container is sorted vector*: We don't need original number sequence... so to make comparison more efficient, we need either sorted vector or multiset... picking vector, as that is more memory friendly
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
	using argument_type = number_set<T>;
	using result_type = std::size_t;
	result_type operator()(argument_type const& s) const
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
class number_sets
{
public:
	// type definitions
	using string_type = std::basic_string<CharT>;
	using stringstream_type = std::basic_stringstream<CharT>;
	using data_container_type = std::unordered_set<number_set<T>, hasher<T>>;
	using const_ref_data_container_type = const std::unordered_set<number_set<T>, hasher<T>>&;
	using const_ref_invalid_inputs_type = const std::vector<std::basic_string<CharT>>&;

private:
	// variables
	data_container_type data;
	std::vector<string_type> invalid_inputs;
	const number_set<T>* most_frequent;
	int duplicate_count;
	int non_duplicate_count;

public:
	// constructor
	number_sets();

	// modifiers

	// returns true for duplicate... false otherwise
	bool add(const string_type& input);

	// getters
	const_ref_invalid_inputs_type get_invalid_inputs() const;
	const number_set<T>* get_most_frequent_data() const;
	int get_duplicate_count() const;
	int get_non_duplicate_count() const;
	const_ref_data_container_type get_data() const;

private:
	std::vector<T> parse_numbers(const string_type& input);
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
	auto numbers = parse_numbers(input);

	sort(numbers.begin(), numbers.end());

	auto res = data.emplace(numbers);
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

	return res.first->occurences > 1;
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

template<typename T, typename CharT>
std::vector<T> number_sets<T, CharT>::parse_numbers(const string_type& input)
{
	bool add_failed = false;

	stringstream_type ss(input);
	string_type token;
	std::vector<T> numbers;

	// splitting input based on comma
	while (std::getline(ss, token, CharT(',')))
	{
		try
		{
			numbers.push_back(convertTo<T>(token));
		}
		catch (std::runtime_error&)
		{
			add_failed = true;
			break;
		}
	}

	if (numbers.empty())
		add_failed = true;

	if (add_failed)
	{
		invalid_inputs.push_back(input);
		throw std::runtime_error("Invalid input");
	}

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
