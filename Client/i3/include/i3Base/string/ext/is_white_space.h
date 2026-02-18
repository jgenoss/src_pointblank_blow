#pragma once

// ctype.h / wctype.h
#include "locale_util.h"

/*
namespace i3
{
	inline bool is_white_space(int c) {  return (gcloc()->locinfo->pctype[c] & 0x08) != 0; }
}
*/

namespace i3
{
	inline bool is_white_space(int c)
	{
		static const int bit_flag = (1 << (' ' - 1)) | (1 << ('\t' - 1)) | (1 << ('\n' - 1)) | (1 << ('\r' - 1)) | (1 << ('\f' - 1));
		return c-- <= ' ' && ((1 << c) & bit_flag) != 0;
	}

}