#pragma once

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

// source: http://stackoverflow.com/a/217605/2790081

std::string& ltrim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
std::string& rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

// trim from both ends
std::string& trim(std::string &s)
{
	return ltrim(rtrim(s));
}


// source: https://isocpp.org/wiki/faq/misc-technical-issues#convert-string-to-any

class BadConversion : public std::runtime_error
{
public:
	BadConversion(const std::string& s)
		: std::runtime_error(s)
	{ }
};

template<typename T>
inline void convert(const std::string& _s, T& x, bool failIfLeftoverChars = true)
{
	string s = _s;

	trim(s);
	if(s[0] == '-' && std::is_unsigned<T>::value)
		throw BadConversion(s);

	std::istringstream i(s);
	char c;
	if (!(i >> x) || (failIfLeftoverChars && i.get(c)))
		throw BadConversion(s);
}

template<typename T>
inline T convertTo(const std::string& s, bool failIfLeftoverChars = true)
{
	T x;
	convert(s, x, failIfLeftoverChars);
	return x;
}
