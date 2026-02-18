#include "pch.h"
#include "URLUtil.h"

namespace URL
{
	// HEX Values array
	char hexVals[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	
	// UNSAFE String
	i3::string UnSafeString = "\"<>%\\^[]`+$,@:;/!#?=&";


	// PURPOSE OF THIS FUNCTION IS TO CONVERT A GIVEN CHAR TO URL HEX FORM
	i3::string convert(char val) 
	{
		i3::string csRet;
		csRet += "%";
		csRet += decToHex(val, 16);	
		return  csRet;
	}

	// THIS IS A HELPER FUNCTION.
	// PURPOSE OF THIS FUNCTION IS TO GENERATE A HEX REPRESENTATION OF GIVEN CHARACTER
	i3::string decToHex(char num, int radix)
	{
		i3::string csTmp;
		int num_char;
		num_char = (int) num;

		// ISO-8859-1 
		// IF THE IF LOOP IS COMMENTED, THE CODE WILL FAIL TO GENERATE A 
		// PROPER URL ENCODE FOR THE CHARACTERS WHOSE RANGE IN 127-255(DECIMAL)
		if (num_char < 0)
			num_char = 256 + num_char;

		while (num_char >= radix)
		{
			int temp = num_char % radix;
			num_char = (int)floor((float)num_char / radix);
			csTmp = hexVals[temp];
		}

		csTmp += hexVals[num_char%16];

		if(csTmp.size() < 2)
		{
			csTmp += '0';
		}

		i3::string strdecToHex(csTmp);
		// Reverse the String
		i3::reverse(strdecToHex.begin(),strdecToHex.end());

		return strdecToHex;
	}

	// PURPOSE OF THIS FUNCTION IS TO CHECK TO SEE IF A CHAR IS URL UNSAFE.
	// true = UNSAFE, false = SAFE
	bool isUnsafe(char compareChar)
	{
		bool bcharfound = false;
		int m_strLen = 0;

		m_strLen = UnSafeString.size();
		for(int ichar_pos = 0; ichar_pos < m_strLen ;ichar_pos++)
		{
			char tmpsafeChar = UnSafeString[ichar_pos]; 
			if(tmpsafeChar == compareChar)
			{ 
				bcharfound = true;
				break;
			} 
		}
		int char_ascii_value = 0;
		//char_ascii_value = __toascii(compareChar);
		char_ascii_value = (int) compareChar;

		// found no unsafe chars, return false	
		if(bcharfound == false &&  char_ascii_value > 32 && char_ascii_value < 123)
		{
			return false;
		}
		
		return true;
	}

	// PURPOSE OF THIS FUNCTION IS TO CONVERT A STRING 
	// TO URL ENCODE FORM.
	i3::string Encode( const i3::string & in_str)
	{	
		int ichar_pos;
		i3::string csEncode;
		i3::string csEncoded;	
		int m_length;

		csEncode = in_str;
		m_length = csEncode.size();

		for(ichar_pos = 0; ichar_pos < m_length; ichar_pos++)
		{
			char ch = csEncode[ichar_pos];
			//if (ch < ' ') 
			//{
			//	ch = ch;
			//}		
			if(!isUnsafe(ch))
			{
				// Safe Character				
				csEncoded += ch;
			}
			else
			{
				// get Hex Value of the Character
				csEncoded += convert(ch);
			}
		}

		return csEncoded;
	}

	i3::string Decode( const i3::string & in_str)
	{
		i3::string ret;
		char ch;
		int ii;

		size_t len = in_str.length();

		for (size_t i = 0; i < len; i++ )
		{
			if ( int(in_str[i]) == 37 ) 
			{
				if ( i+2 >= len )
					return in_str;

				int iret = sscanf( in_str.substr(i+1,2).c_str(), "%x", &ii );

				if (iret < 1)
					return "";

				ch = static_cast<char>(ii);

				ret += ch;

				i = i + 2;
			} 
			else 
			{
				ret += in_str[i];
			}
		}
		return (ret);
	}
};
