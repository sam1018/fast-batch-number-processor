#pragma once

#include "routines.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>


/*
	Design Decisions:
	*Data type templatized*
		* The question mentions integer type... by making type as template, we allow ourselves the flexibility of having signed, unsigned, long long etc.
		* Only integral type is allowed... ensured by using static_assert
	*Container is sorted vector*: We don't need original number sequence... so to make comparison more efficient, we need either sorted vector or multiset... picking vector, as that is more memory friendly
*/

template<typename T>
class number_sets
{
public:
	using invalid_inputs_type = std::vector<std::string>;

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
	bool add(const std::string& input);

	invalid_inputs_type& get_invalid_inputs();
};


/*
 *	implementation for class number_sets
**/

template<typename T>
number_sets<T>::number_sets()
{
	static_assert(std::is_integral<T>::value, "Integral type required.");
}

template<typename T>
bool number_sets<T>::add(const std::string& input)
{
	bool add_failed = false;

	std::stringstream ss(input);
	std::string token;
	number_set<T> in;

	// splitting input based on comma
	while (std::getline(ss, token, ','))
	{
		try
		{
			in.numbers.push_back(convertTo<T>(token));
		}
		catch (BadConversion&)
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

template<typename T>
typename number_sets<T>::invalid_inputs_type& number_sets<T>::get_invalid_inputs()
{
	return invalid_inputs;
}
