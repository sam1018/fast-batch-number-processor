#pragma once

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>


/*
	File: routines.h
	Description: A header only file for various helper functions
	Important Functions:
		* trim: trims whitespace from left and right for a string
		* convertTo: converts a string to a given data type
*/



// source: http://stackoverflow.com/a/217605/2790081

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


// source: https://isocpp.org/wiki/faq/misc-technical-issues#convert-string-to-any

template<typename T, typename CharT>
inline void convert(std::basic_string<CharT> s, T& x, bool failIfLeftoverChars = true)
{
	trim(s);
	if(s[0] == '-' && std::is_unsigned<T>::value)
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
