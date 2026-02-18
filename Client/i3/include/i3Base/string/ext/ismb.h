#pragma once

#include "locale_util.h"
#include <mbstring.h>

namespace i3
{
	// 대부분 인라인 처리하도록 할것..
	// mb계열이므로 char타입이면 충분하리라 본다..
	
	namespace detail
	{
		const unsigned char lead_byte_mask	= 0x04;    /* MBCS 1st (lead) byte */
		const unsigned char second_byte_mask = 0x08;    /* MBCS 2nd byte*/
		const unsigned char single_byte_mask = 0x03;
		const unsigned short single_byte_loc_mask = _BLANK | _PUNCT | _ALPHA | _DIGIT; 
	}
	/*
	inline bool ismbblead( char c)
	{
		return (gcloc()->mbcinfo->mbctype+1)[unsigned char(c)] & detail::lead_byte_mask;
	}

	inline bool ismbbtrail( char c)
	{
		return (gcloc()->mbcinfo->mbctype+1)[unsigned char(c)] & detail::second_byte_mask;
	}
	*/
	inline bool ismbblead( char c)
	{
		return _ismbblead_l(c, gcloc() ) != FALSE;
	}

	inline bool ismbbtrail( char c)
	{
		return _ismbbtrail_l(c, gcloc() ) != FALSE;
	}

/*
	enum mb_type
	{
		mb_illegal	= _MBC_ILLEGAL,
		mb_single	= _MBC_SINGLE,
		mb_lead		= _MBC_LEAD,
		mb_trail	= _MBC_TRAIL,
		mb_start	= _MBC_SINGLE,
	};
 
	inline mb_type mbbtype( char c, mb_type prev = mb_start)
	{
		unsigned char t = (gcloc()->mbcinfo->mbctype+1)[unsigned char(c)];
		if ( prev == mb_lead )
			return ( t & detail::second_byte_mask) ? mb_trail : mb_illegal;
				
		if (t & detail::lead_byte_mask ) return mb_lead;
			
		unsigned short l = (gcloc()->locinfo->pctype)[unsigned char(c)];

		if ( (t & detail::single_byte_mask) && 
			( l & detail::single_byte_loc_mask ) )
			return mb_single;

		return mb_illegal;
	}
*/

	// target_str은 소스문자열의 중간즈음이다..
	inline bool ismbslead(const char* target_str)
	{
		return ismbblead(*target_str);
	}
	
	inline bool ismbstrail(const char* target_str)
	{
		return ismbbtrail(*target_str);
	}
	
//			영문			: 0
//			한글 1st byte	: 1
//			한글 2nd byte	: 2

	inline int	curr_mbb_byte( char val, int prev = 0)
	{
		if (prev == 1)
			return 2;
		return ( _ismbblead_l(val, gcloc() ) ) ? 1 : 0;
	}

	//
	//  i3String::WhatIsCode의 대체함수 제거...  
	//  기존함수의 경우, 곳곳에서 잘 쓰인 부분도 있지만 잘 못 쓰인 부분도 있어서..
	//  그냥 없앤다...
	
/*
	inline bool is_not_sb( const char* str, size_t idx)
	{
		const unsigned char* mbc_table = gcloc()->mbcinfo->mbctype+1;

		{
			unsigned char t = mbc_table[unsigned char(str[idx])];
			if ( t & detail::lead_byte_mask ) 
				return true;
		}

		if (idx > 0) 
		{
			--idx;
			unsigned char t = mbc_table[unsigned char(str[idx])];	
			if ( t & detail::lead_byte_mask ) 
				return true;
		}

		return false;
	}
*/		
/*
	inline bool is_not_sb( const char* str, size_t idx)
	{
		{
			if ( ismbblead( str[idx] ) ) 
				return true;
		}

		if (idx > 0) 
		{
			--idx;
			if ( ismbblead( str[idx] ) ) 
				return true;
		}
		return false;
	}

	inline int	what_is_code( const char* str, size_t idx)
	{
		if ( ismbblead( str[idx] ) ) 
				return 1;

		if (idx > 0) 
		{
			--idx;
			if ( ismbblead( str[idx] ) ) 
				return 2;
		}
		return 0;
	}
*/

}