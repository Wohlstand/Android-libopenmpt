/* SPDX-License-Identifier: BSL-1.0 OR BSD-3-Clause */

#ifndef MPT_FORMAT_DEFAULT_INTEGER_HPP
#define MPT_FORMAT_DEFAULT_INTEGER_HPP


#include "mpt/base/detect.hpp"

#if 1
#define MPT_FORMAT_CXX17_INT 1
#else
#define MPT_FORMAT_CXX17_INT 0
#endif

#if MPT_FORMAT_CXX17_INT
#include "mpt/base/algorithm.hpp"
#endif // MPT_FORMAT_CXX17_INT
#include "mpt/base/namespace.hpp"
#include "mpt/base/utility.hpp"
#include "mpt/format/helpers.hpp"
#include "mpt/string_transcode/transcode.hpp"

#if MPT_FORMAT_CXX17_INT
#include <charconv>
#endif // MPT_FORMAT_CXX17_INT
#if !MPT_FORMAT_CXX17_INT
#include <ios>
#include <locale>
#include <sstream>
#endif // !MPT_FORMAT_CXX17_INT
#include <string>
#if MPT_FORMAT_CXX17_INT
#include <system_error>
#endif // MPT_FORMAT_CXX17_INT
#include <type_traits>



namespace mpt {
inline namespace MPT_INLINE_NS {


#if MPT_FORMAT_CXX17_INT

template <typename Tstring, typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline Tstring to_chars_string(const T & x) {
	std::string str(1, '\0');
	bool done = false;
	while (!done) {
		if constexpr (std::is_same<T, bool>::value) {
			std::to_chars_result result = std::to_chars(str.data(), str.data() + str.size(), static_cast<int>(x));
			if (result.ec != std::errc{}) {
				str.resize(mpt::exponential_grow(str.size()), '\0');
			} else {
				str.resize(result.ptr - str.data());
				done = true;
			}
		} else {
			std::to_chars_result result = std::to_chars(str.data(), str.data() + str.size(), x);
			if (result.ec != std::errc{}) {
				str.resize(mpt::exponential_grow(str.size()), '\0');
			} else {
				str.resize(result.ptr - str.data());
				done = true;
			}
		}
	}
	return mpt::convert_formatted_simple<Tstring>(str);
}

template <typename Tstring, typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline Tstring format_value_default(const T & x) {
	return mpt::transcode<Tstring>(mpt::to_chars_string<typename mpt::select_format_string_type<Tstring>::type>(x));
}

#endif // MPT_FORMAT_CXX17_INT


#if !MPT_FORMAT_CXX17_INT

template <typename Tstring, typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline Tstring to_stream_string(const T & x) {
	using stream_char_type = typename mpt::select_format_char_type<typename Tstring::value_type>::type;
	std::basic_ostringstream<stream_char_type> s;
	s.imbue(std::locale::classic());
	if constexpr (std::is_same<T, bool>::value) {
		s << static_cast<int>(x);
	} else if constexpr (mpt::is_character<T>::value) {
		s << (x + 0); // force integral promotion
	} else {
		s << x;
	}
	return mpt::convert_formatted_simple<Tstring>(s.str());
}

template <typename Tstring, typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline Tstring format_value_default(const T & x) {
	return mpt::transcode<Tstring>(mpt::to_stream_string<typename mpt::select_format_string_type<Tstring>::type>(x));
}

#endif // !MPT_FORMAT_CXX17_INT


template <typename Tstring, typename T, std::enable_if_t<std::is_enum<T>::value, bool> = true>
inline Tstring format_value_default(const T & x) {
	return mpt::format_value_default<Tstring>(mpt::to_underlying(x));
}


} // namespace MPT_INLINE_NS
} // namespace mpt



#endif // MPT_FORMAT_DEFAULT_INTEGER_HPP
