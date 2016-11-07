#pragma once

#include <string>
#include <vector>

/*
	File: parse_ints_fast.h
	Description: header only class parse_ints_fast
	A fast implementation for parsing ints from a comma separated string of ints
	With support for error detection
*/

class parse_ints_fast
{
private:
	enum class num_state_enum
	{
		ready_to_start, // can start reading new number
		started, // reading number
		finished // finished reading... looking for comma or end of string
	};

	enum class char_state_enum
	{
		space,
		comma,
		minus,
		digit,
		invalid
	};

	num_state_enum num_state;
	bool negative;
	long long int num; // long long used for overflow detection
	std::vector<int> result;

private:
	int char_to_int(char ch)
	{
		return static_cast<int>(ch - '0');
	}

	char_state_enum get_char_state(char ch)
	{
		if (isspace(ch))
			return char_state_enum::space;
		if (ch == ',')
			return char_state_enum::comma;
		if (ch == '-')
			return char_state_enum::minus;
		if (isdigit(ch))
			return char_state_enum::digit;

		return char_state_enum::invalid;
	}

	void handle_char_ready_to_start_state(char ch)
	{
		switch (get_char_state(ch))
		{
		case char_state_enum::space:
			// ok... nothing to do
			break;
		case char_state_enum::comma:
			// not ok
			throw std::runtime_error("Invalid Input");
			break;
		case char_state_enum::minus:
			// ok
			negative = true;
			num_state = num_state_enum::started;
			break;
		case char_state_enum::digit:
			// ok
			num = char_to_int(ch);
			num_state = num_state_enum::started;
			break;
		case char_state_enum::invalid:
		default:
			throw std::runtime_error("Invalid Input");
			break;
		}
	}

	void test_overflow()
	{
		auto test_num = negative ? num * -1 : num;

		if (test_num > INT_MAX || test_num < INT_MIN)
			throw std::runtime_error("Int overflow");
	}

	void finalize_reading()
	{
		if (num_state == num_state_enum::finished || num_state == num_state_enum::started)
		{
			test_overflow();
			num = negative ? num * -1 : num;
			negative = false;
			result.push_back(static_cast<int>(num));
			num_state = num_state_enum::ready_to_start;
		}
	}

	void handle_char_started_state(char ch)
	{
		switch (get_char_state(ch))
		{
		case char_state_enum::space:
			// ok... end num reading
			num_state = num_state_enum::finished;
			break;
		case char_state_enum::comma:
			// ok... end num reading
			num_state = num_state_enum::finished;
			finalize_reading();
			break;
		case char_state_enum::minus:
			// not ok
			throw std::runtime_error("Invalid Input");
			break;
		case char_state_enum::digit:
			// ok
			num = num * 10 + char_to_int(ch);
			break;
		case char_state_enum::invalid:
		default:
			throw std::runtime_error("Invalid Input");
			break;
		}
	}

	void handle_char_finished_state(char ch)
	{
		switch (get_char_state(ch))
		{
		case char_state_enum::space:
			// ok... nothing to do
			break;
		case char_state_enum::comma:
			// ok
			finalize_reading();
			break;
		case char_state_enum::minus:
		case char_state_enum::digit:
		case char_state_enum::invalid:
		default:
			throw std::runtime_error("Invalid Input");
			break;
		}
	}

	std::vector<int> get_values_impl(std::string::const_iterator begin, std::string::const_iterator end)
	{
		for (; begin != end; ++begin)
		{
			switch (num_state)
			{
			case num_state_enum::ready_to_start:
				handle_char_ready_to_start_state(*begin);
				break;
			case num_state_enum::started:
				handle_char_started_state(*begin);
				break;
			case num_state_enum::finished:
				handle_char_finished_state(*begin);
			default:
				break;
			}

			test_overflow();
		}

		finalize_reading();

		return result;
	}

public:
	parse_ints_fast() :
		num_state(num_state_enum::ready_to_start),
		negative(false)
	{}

	std::vector<int> get_values(const std::string& s)
	{
		result.reserve(std::count(s.begin(), s.end(), ',') + 1);
		return get_values_impl(s.begin(), s.end());
	}
};
