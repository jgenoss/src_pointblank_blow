#pragma once


namespace i3
{
	typedef _locale_t				c_locale_type;

	namespace detail {	I3_EXPORT_BASE extern c_locale_type	g_locale; }

	inline c_locale_type	global_c_locale() { return detail::g_locale; }
	inline c_locale_type	gcloc() { return detail::g_locale; }	
}
