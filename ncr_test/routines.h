#pragma once

#include <cctype>
#include <locale>
#include <vector>
#include <iterator>
#include <algorithm> 
#include <functional> 


/*
	File: routines.h
	Description: A header only file for various helper functions
	Important Functions:
		* hash_value: generates hash value for a container
		* convertTo: converts a string to a given data type
		* noncopyable class
		* trim: trims whitespace from left and right for a string
		* operator << overload for vector
*/

/*
hash_value
*/

// source: boost

template <typename SizeT>
inline void hash_combine_impl(SizeT& seed, SizeT value)
{
	seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <class T>
inline void hash_combine(std::size_t& seed, T const& v)
{
	std::hash<T> hasher;
	return hash_combine_impl(seed, hasher(v));
}

template <class It>
inline std::size_t hash_range(It first, It last)
{
	std::size_t seed = 0;

	for (; first != last; ++first)
	{
		hash_combine(seed, *first);
	}

	return seed;
}

template <class T, class A>
std::size_t hash_value(std::vector<T, A> const& v)
{
	return hash_range(v.begin(), v.end());
}

/*
convertTo
*/

// source: https://isocpp.org/wiki/faq/misc-technical-issues#convert-string-to-any

template<typename T, typename CharT>
inline void convert(std::basic_string<CharT> s, T& x, bool failIfLeftoverChars = true)
{
	trim(s);
	if (s[0] == '-' && std::is_unsigned<T>::value)
		throw std::runtime_error("Conversion Failed");

	std::basic_istringstream<CharT> i(s);
	CharT c;
	if (!(i >> x) || (failIfLeftoverChars && i.get(c)))
		throw std::runtime_error("Conversion Failed");
}

template<typename T, typename CharT>
inline T convertTo(const std::basic_string<CharT>& s, bool failIfLeftoverChars = true)
{
	T x;
	convert(s, x, failIfLeftoverChars);
	return x;
}


/*
class noncopyable
*/

// Source: boost

class noncopyable
{
protected:
	noncopyable() = default;
	~noncopyable() = default;

	noncopyable(const noncopyable&) = delete;
	noncopyable& operator=(const noncopyable&) = delete;
};




/* 
	trim
*/

template<typename CharT>
std::basic_string<CharT>& ltrim(std::basic_string<CharT> &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

template<typename CharT>
std::basic_string<CharT>& rtrim(std::basic_string<CharT> &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

template<typename CharT>
std::basic_string<CharT>& trim(std::basic_string<CharT> &s)
{
	return ltrim(rtrim(s));
}

/*
	operator << overload for vector
*/

template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v)
{
	if (!v.empty())
	{
		out << '[';
		std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
		out << "\b\b]";
	}
	return out;
}
