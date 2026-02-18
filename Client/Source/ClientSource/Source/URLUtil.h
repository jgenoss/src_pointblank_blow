#if !defined( __URL_UTIL_H__)
#define __URL_UTIL_H__

namespace URL
{
	i3::string convert(char val);
	i3::string decToHex(char num, int radix);
	bool isUnsafe(char compareChar);

	i3::string Encode( const i3::string & in_str);
	i3::string Decode( const i3::string & in_str);
};

#endif // __URL_UTIL_H__