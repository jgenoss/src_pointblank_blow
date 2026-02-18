#pragma once

namespace i3
{
	namespace detail_sprintf
	{
		enum
		{
			SPRINTF_FL_SIGN			= 0x0000001,   /* put plus or minus in front */
			SPRINTF_FL_SIGNSP		= 0x0000002,  /* put space or minus in front */
			SPRINTF_FL_LEFT			= 0x0000004, /* left justify */
			SPRINTF_FL_LEADZERO		= 0x0000008,   /* pad with leading zeros */
			SPRINTF_FL_FORCEUNSIGNED= 0x0000010,
			SPRINTF_FL_ALTERNATE	= 0x0000020,   /* alternate form requested */
			SPRINTF_FL_FORCEOCTAL	= 0x0000040,   /* force leading '0' for octals */
			SPRINTF_FL_UPPER		= 0x0000080,
			SPRINTF_FL_LOWER		= 0x0000100,
			SPRINTF_FL_HEX			= 0x0000200,
			SPRINTF_FL_OCT			= 0x0000400,
			SPRINTF_FL_SCI			= 0x0000800,
			SPRINTF_FL_FIXEDFLOAT	= 0x0001000,
			SPRINTF_FL_GFLOAT		= 0x0002000,
			SPRINTF_FL_INTEGRAL		= 0x0004000,
			SPRINTF_FL_SINGLECHAR	= 0x0008000,
			SPRINTF_FL_NEGATIVE		= 0x0010000,   /* value is negative */
		};

	}
}