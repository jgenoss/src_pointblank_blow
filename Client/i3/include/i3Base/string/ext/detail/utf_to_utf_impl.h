#pragma once

namespace i3
{

	namespace detail_utf
	{
		static const uint32_t illegal = 0xFFFFFFFFu;
		static const uint32_t incomplete = 0xFFFFFFFEu;
		static const uint32_t utf_error_mask = incomplete;
		static const uint32_t utf_error_mask_result = incomplete;
	}

	namespace detail_utf8
	{
		I3_EXPORT_BASE	uint32_t decode( const char*& pos, const char* end);
		I3_EXPORT_BASE	int	encode(uint32_t cp, char(&out)[4]);
	}

	namespace detail_utf16
	{
		I3_EXPORT_BASE	uint32_t decode( const wchar_t*& pos, const wchar_t* end);
		I3_EXPORT_BASE 	int		encode( uint32_t cp, wchar_t(&out)[2]);	
	}

}
