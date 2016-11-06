#pragma once

#include "routines.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>

/*
	File: number_sets.h
	Description: header only class number_sets

	Design Decisions:
	*Data type templatized*
		* The question mentions integer type... by making type as template, we allow ourselves the flexibility of having signed, unsigned, long long etc.
		* Only integral type is allowed... ensured by using static_assert
	*Container is sorted vector*: We don't need original number sequence... so to make comparison more efficient, we need either sorted vector or multiset... picking vector, as that is more memory friendly
*/


/*
	interface for class number_sets
*/

template<typename T, typename CharT = char>
class number_sets
{
public:
	using string_type = std::basic_string<CharT>;
	using invalid_inputs_type = std::vector<string_type>;

private:
	using stringstream_type = std::basic_stringstream<CharT>;

private:
	template<typename T>
	struct number_set
	{
		std::vector<T> numbers;
	};

	std::vector<number_set<T>> valid_inputs;
	invalid_inputs_type invalid_inputs;

public:
	number_sets();

	// returns false if failed to insert input
	// true otherwise
	bool add(const string_type& input);

	invalid_inputs_type& get_invalid_inputs();
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
	bool add_failed = false;

	stringstream_type ss(input);
	string_type token;
	number_set<T> in;

	// splitting input based on comma
	while (std::getline(ss, token, CharT(',')))
	{
		try
		{
			in.numbers.push_back(convertTo<T>(token));
		}
		catch (std::runtime_error&)
		{
			add_failed = true;
			break;
		}
	}

	if (in.numbers.empty())
		add_failed = true;

	if(add_failed)
		invalid_inputs.push_back(input);

	return !add_failed;
}

template<typename T, typename CharT>
typename number_sets<T, CharT>::invalid_inputs_type& number_sets<T, CharT>::get_invalid_inputs()
{
	return invalid_inputs;
}
