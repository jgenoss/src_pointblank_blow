#include "i3CommonType.h"
#include "REAL16.h"

UINT16	REAL16::F2H( REAL32 val)
{
    UINT16 h, hs, he, hm;
    UINT32 x, xs, xe, xm;
    int hes;

	x = *((UINT32 *) &val);
    
	if( (x & 0x7FFFFFFFu) == 0 )
	{  // Signed zero
		h = (UINT16) (x >> 16);  // Return the signed zero
	} 
	else 
	{
		// Not zero
		xs = x & 0x80000000u;  // Pick off sign bit
		xe = x & 0x7F800000u;  // Pick off exponent bits
		xm = x & 0x007FFFFFu;  // Pick off mantissa bits

		if( xe == 0 )
		{
			// Denormal will underflow, return a signed zero
			h = (UINT16) (xs >> 16);
		}
		else if( xe == 0x7F800000u )
		{
			// Inf or NaN (all the exponent bits are set)
			if( xm == 0 )
			{
				// If mantissa is zero ...
				h = (UINT16) ((xs >> 16) | 0x7C00u); // Signed Inf
			}
			else
			{
				h = (UINT16) 0xFE00u; // NaN, only 1st mantissa bit set
			}
		} 
		else
		{
			// Normalized number
			hs = (UINT16) (xs >> 16); // Sign bit
			hes = ((int)(xe >> 23)) - 127 + 15; // Exponent unbias the single, then bias the halfp

			if( hes >= 0x1F )
			{
				// Overflow
				h = (UINT16) ((xs >> 16) | 0x7C00u); // Signed Inf
			}
			else if( hes <= 0 )
			{
				// Underflow
				if( (14 - hes) > 24 )
				{
					// Mantissa shifted all the way off & no rounding possibility
					hm = (UINT16) 0u;  // Set mantissa to zero
				}
				else
				{
					xm |= 0x00800000u;  // Add the hidden leading bit
					hm = (UINT16) (xm >> (14 - hes)); // Mantissa
					if( (xm >> (13 - hes)) & 0x00000001u ) // Check for rounding
						hm += (UINT16) 1u; // Round, might overflow into exp bit, but this is OK
				}
				h = (hs | hm); // Combine sign bit and mantissa bits, biased exponent is zero
			}
			else
			{
				he = (UINT16) (hes << 10); // Exponent
				hm = (UINT16) (xm >> 13); // Mantissa
				if( xm & 0x00001000u ) // Check for rounding
					h = (hs | he | hm) + (UINT16) 1u; // Round, might overflow to inf, this is OK
				else
					h = (hs | he | hm);  // No rounding
			}
		}
	}

	return h;
}

REAL32 REAL16::H2F( UINT16 val)
{
	UINT32 hs, he, hm;
	UINT32 xs, xe, xm;
    INT32 xes;
	INT32 e;
	UINT32 f;

	if( (val & 0x7FFF) == 0 )
	{  
		// Signed zero
		return 0.0f;
	}

	hs = val & 0x8000u;  // Sign bit
	he = val & 0x7C00u;  // exponent bits
	hm = val & 0x03FFu;  // mantissa bits

	if( he == 0 ) 
	{
		// Denormal will convert to normalized
		e = -1; // The following loop figures out how much extra to adjust the exponent
        do 
		{
            e++;
            hm <<= 1;
        } while( (hm & 0x0400u) == 0 ); // Shift until leading bit overflows into exponent bit

		xs = ((UINT32) hs) << 16; // Sign bit
		xes = ((INT32) (he >> 10)) - 15 + 127 - e; // Exponent unbias the halfp, then bias the single
		xe = (UINT32) (xes << 23); // Exponent
		xm = ((UINT32) (hm & 0x03FFu)) << 13; // Mantissa
		f = (xs | xe | xm); // Combine sign bit, exponent bits, and mantissa bits
	}
	else if( he == 0x7C00u )
	{
		// Inf or NaN (all the exponent bits are set)
		if( hm == 0 )
		{ // If mantissa is zero ...
			f = (((UINT32) hs) << 16) | ((UINT32) 0x7F800000u); // Signed Inf
		}
		else
		{
			f = (UINT32) 0xFFC00000u; // NaN, only 1st mantissa bit set
		}
	} 
	else
	{ 
		// Normalized number
		xs = ((UINT32) hs) << 16; // Sign bit
		xes = ((INT32) (he >> 10)) - 15 + 127; // Exponent unbias the halfp, then bias the single
		xe = (UINT32) (xes << 23); // Exponent
		xm = ((UINT32) hm) << 13; // Mantissa
		f = (xs | xe | xm); // Combine sign bit, exponent bits, and mantissa bits
    }

	return *((REAL32 *) &f);
}
