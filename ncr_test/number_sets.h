#pragma once

#include "routines.h"
#include "number_sets_impl.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <type_traits>
#include <unordered_set>

/*
	class number_sets
*/

namespace ncr_test
{
	template<typename T, typename CharT = char>
	class number_sets
	{
	public:
		using data_type = number_sets_data<T, CharT>;
		using string_type = typename data_type::string_type;
		using const_ref_invalid_inputs_type = typename data_type::const_ref_invalid_inputs_type;
		using const_ref_data_container_type = typename data_type::const_ref_data_container_type;

	private:
		data_type data;

	public:

		/*
			ctors
		*/

		number_sets() {
			static_assert(std::is_integral<T>::value, "Integral type required.");
		}

		/*
			Modifiers
		*/

		// returns false for duplicate... true otherwise
		bool add(const string_type& input) {
			bool ret;

			try
			{
				ret = consume_number_set<T, CharT>(
					produce_number_set<T, CharT>(input), data);
			}
			catch (...)
			{
				data.invalid_inputs.push_back(input);
				throw;
			}

			return ret;
		}

		// another add mechanism that works on the whole input file
		// uses concurrency to improve performance
		// supported for T = int and CharT = char
		void add_batch_mode(const string_type& filename, int producer_count) {
			static_assert(std::is_same<T, int>::value && std::is_same<CharT, char>::value, "only T = int and CharT = char is accepted");
			add_number_sets_concurrent(filename, data, producer_count);
		}

		/*
			getters
		*/

		const_ref_invalid_inputs_type get_invalid_inputs() const {
			return data.invalid_inputs;
		}
		const number_set<T> get_most_frequent_data() const {
			return data.most_frequent ?
				*data.most_frequent :
				number_set<T>{ vector<T>{}, 0 };
		}
		int get_duplicate_count() const {
			return data.duplicate_count;
		}
		int get_non_duplicate_count() const {
			return data.non_duplicate_count;
		}
		const_ref_data_container_type get_data() const {
			return data.number_sets;
		}
	};
}
