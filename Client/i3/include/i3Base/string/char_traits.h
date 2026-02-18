#pragma once

#include <iosfwd>
#pragma pack(push, _CRT_PACKING)

namespace i3
{

	typedef		int				mbstate_t;

	template<typename Ch>
	struct char_traits_base
	{
		typedef Ch					char_type;
		typedef std::streamoff		off_type;
		typedef mbstate_t			state_type;		

		static void			assign(char_type& Left, const char_type& Right) {	Left = Right;	}	// assign an element
		static bool			eq(const char_type& Left, const char_type& Right)	{	return (Left == Right); }// test for element equality
		static bool			lt(const char_type& Left, const char_type& Right)	{	return (Left < Right);	}	// test if _Left precedes _Right

	};

	template<typename Ch> 
	struct char_traits : public char_traits_base<Ch>
	{
	
		typedef long				int_type;
		typedef std::streampos		pos_type;
		
		static int		compare( const char_type* First1, const char_type * First2, size_t Count)
		{	
			for (; 0 < Count; --Count, ++First1, ++First2)
				if (!eq(*First1, *First2))
					return (lt(*First1, *First2) ? -1 : +1);
			return (0);
		}	// compare [_First1, _First1 + _Count) with [_First2, ...)
		
		static size_t	length( const char_type* First)
		{	
			size_t Count;
			for (Count = 0; !eq(*First, char_type()); ++First)
				++Count;
			return (Count);
		}	// find length of null-terminated sequence
		
		static char_type*	copy(char_type* First1, const char_type* First2, size_t Count)
		{	
			return _Copy_s(First1, Count, First2, Count);
		}	// assume there is enough space in the destination buffer

		static char_type*	_Copy_s(char_type* First1, size_t  Dest_size, const char_type* First2, size_t  Count)
		{	
			char_type*  Next =  First1;
			for (; 0 <  Count; --Count, ++Next, ++First2)
				assign(*Next, *First2);
			return ( First1);
		}	// copy [_First1, _First1 + _Count) to [_First2, ...)

		static const char_type* find(const char_type * First, size_t Count, const char_type& ch)
		{	
			for (; 0 < Count; --Count, ++First)
				if (eq(*First, ch))
					return (First);
			return (0);
		}	// look for _Ch in [_First, _First + _Count)
		static char_type* move(char_type * First1, const char_type * First2, size_t  Count)
		{	
			return _Move_s( First1,  Count,  First2,  Count);
		}	// move [_First1, _First1 + _Count) to [_First2, ...)

		static char_type * _Move_s(char_type *First1, size_t Dest_size, const char_type *First2, size_t Count)
		{	

			char_type *Next = First1;
			if (First2 < Next && Next < First2 + Count)
				for (Next += Count, First2 += Count; 0 < Count; --Count)
					assign(*--Next, *--First2);
			else
				for (; 0 < Count; --Count, ++Next, ++First2)
					assign(*Next, *First2);
			return (First1);
		}	// move [_First1, _First1 + _Count) to [_First2, ...)
		static char_type*	assign( char_type* First, size_t Count, char_type ch)
		{	
			char* Next = First;
			for (; 0 < Count; --Count, ++Next)
				assign(*Next, ch);
			return (First);
		}	// assign _Count * _Ch to [_First, ...)

		static char_type		to_char_type(const int_type& Meta) {	 return (char_type)Meta; 	}// convert metacharacter to character
		static int_type			to_int_type(const char_type& ch)	{	return (int_type)ch;	}	// convert character to metacharacter
		static bool				eq_int_type(const int_type& Left, const int_type& Right) 	{	return (Left == Right);	}// test for metacharacter equality
		static int_type			eof() {	 return (int_type)EOF;	}	// return end-of-file metacharacter
		static int_type			not_eof(const int_type& Meta) {	return  Meta != eof() ? (int_type) Meta : (int_type) !eof(); }	// return anything but EOF
		using char_traits_base<Ch>::assign;

	};

	template<>
	struct char_traits<char> : public char_traits_base<char>
	{
		
		typedef		int					int_type;		
		typedef		std::streampos		pos_type;
		
	
		static int				compare( const char_type* First1, const char_type * First2, size_t Count){	return (::memcmp(First1, First2, Count));	}	
		static size_t			length( const char_type* First)	{	return (::strlen(First)); }
		static char_type*		copy(char_type * First1, const char_type * First2, size_t  Count){	return _Copy_s( First1,  Count,  First2,  Count);	}
		static char_type*		_Copy_s(char_type* First1, size_t  Dest_size, const char_type* First2, size_t  Count) 
			{	::memcpy_s(First1, Dest_size, First2, Count);		return First1;	}
		static const char_type* find(const char_type * First, size_t Count, const char_type& ch){ return ((const char_type*)::memchr(First, ch, Count)); }
		static char_type*		move(char_type * First1, const char_type * First2, size_t  Count)
			{	return _Move_s( First1,  Count,  First2,  Count);	}
		static char_type *		_Move_s(char_type *First1, size_t Dest_size, const char_type *First2, size_t Count)
		{	::memmove_s(First1, Dest_size, First2, Count);	return First1;	}
		
		static char_type*		assign( char_type* First, size_t Count, char_type ch){	return (char_type*)::memset(First, ch, Count);	}

		static char_type		to_char_type(const int_type& Meta) {	 return (char_type)Meta; 	}// convert metacharacter to character
		static int_type			to_int_type(const char_type& ch)	{	return (unsigned char)ch;	}	// convert character to metacharacter
		static bool				eq_int_type(const int_type& Left, const int_type& Right) 	{	return (Left == Right);	}// test for metacharacter equality
		static int_type			eof() {	 return EOF;	}	// return end-of-file metacharacter
		static int_type			not_eof(const int_type& Meta) {	return  Meta != eof() ? Meta : !eof(); }	// return anything but EOF
		
		using char_traits_base<char>::assign;
	};

#ifdef _NATIVE_WCHAR_T_DEFINED	
	template<>
	struct char_traits<wchar_t> : public char_traits_base<wchar_t>
	{
		typedef		wint_t				int_type;
		typedef		std::wstreampos		pos_type;
		
		static int				compare( const char_type* First1, const char_type * First2, size_t Count){	return (::wmemcmp(First1, First2, Count));	}	
		static size_t			length( const char_type* First)	{	return (::wcslen(First)); }
		static char_type*		copy(char_type * First1, const char_type * First2, size_t  Count){	return _Copy_s( First1,  Count,  First2,  Count);	}
		static char_type*		_Copy_s(char_type* First1, size_t  Dest_size, const char_type* First2, size_t  Count) 
			{	::wmemcpy_s(First1, Dest_size, First2, Count);		return First1;	}
		
		static const char_type* find(const char_type * First, size_t Count, const char_type& ch){ return ((const char_type*)::wmemchr(First, ch, Count)); }
		static char_type*		move(char_type * First1, const char_type * First2, size_t  Count)
			{	return _Move_s( First1,  Count,  First2,  Count);	}
		static char_type *		_Move_s(char_type *First1, size_t Dest_size, const char_type *First2, size_t Count)
		{	::wmemmove_s(First1, Dest_size, First2, Count);	return First1;	}
		
		static char_type*		assign( char_type* First, size_t Count, char_type ch){	return ::wmemset(First, ch, Count);	}
		
		static char_type		to_char_type(const int_type& Meta) {	 return Meta; 	}// convert metacharacter to character
		static int_type			to_int_type(const char_type& ch)	{	return ch;	}	// convert character to metacharacter
		static bool				eq_int_type(const int_type& Left, const int_type& Right) 	{	return (Left == Right);	}// test for metacharacter equality
		static int_type			eof() {	 return WEOF;	}	// return end-of-file metacharacter
		static int_type			not_eof(const int_type& Meta) {	return  Meta != eof() ? Meta : !eof(); }	// return anything but EOF
		using char_traits_base<wchar_t>::assign;
	};
#endif

	template<>
	struct char_traits<unsigned short> : public char_traits_base<unsigned short>
	{
		typedef		wint_t				int_type;	
		typedef		std::wstreampos		pos_type;		
		
		static int				compare( const char_type* First1, const char_type * First2, size_t Count){	return (::wmemcmp((const wchar_t*)First1, (const wchar_t*)First2, Count));	}	
		static size_t			length( const char_type* First)	{	return (::wcslen((const wchar_t*)First)); }
		static char_type*		copy(char_type * First1, const char_type * First2, size_t  Count){	return _Copy_s( First1,  Count,  First2,  Count);	}
		static char_type*		_Copy_s(char_type* First1, size_t  Dest_size, const char_type* First2, size_t  Count) 
			{	::wmemcpy_s((wchar_t *)First1, Dest_size, (const wchar_t *)First2, Count);		return First1;	}
		static const char_type* find(const char_type * First, size_t Count, const char_type& ch){ return ((const char_type*)::wmemchr((const wchar_t*)First, ch, Count)); }				
		
		static char_type*		move(char_type * First1, const char_type * First2, size_t  Count)
			{	return _Move_s( First1,  Count,  First2,  Count);	}
		static char_type *		_Move_s(char_type *First1, size_t Dest_size, const char_type *First2, size_t Count)
		{	::wmemmove_s((wchar_t *)First1, Dest_size, (const wchar_t *)First2, Count);	return First1;	}
		static char_type*		assign( char_type* First, size_t Count, char_type ch){	return (char_type*)::wmemset( (wchar_t *)First, ch, Count);	}
				
		static char_type		to_char_type(const int_type& Meta) {	 return Meta; 	}// convert metacharacter to character
		static int_type			to_int_type(const char_type& ch)	{	return ch;	}	// convert character to metacharacter
		static bool				eq_int_type(const int_type& Left, const int_type& Right) 	{	return (Left == Right);	}// test for metacharacter equality
		static int_type			eof() {	 return WEOF;	}	// return end-of-file metacharacter
		static int_type			not_eof(const int_type& Meta) {	return  Meta != eof() ? Meta : !eof(); }	// return anything but EOF

		using char_traits_base<unsigned short>::assign;
	};

	template<>
	struct char_traits<short> : public char_traits_base<short>
	{
		typedef		wint_t				int_type;	
		typedef		std::wstreampos		pos_type;		

		static int				compare( const char_type* First1, const char_type * First2, size_t Count){	return (::wmemcmp((const wchar_t*)First1, (const wchar_t*)First2, Count));	}	
		static size_t			length( const char_type* First)	{	return (::wcslen((const wchar_t*)First)); }
		static char_type*		copy(char_type * First1, const char_type * First2, size_t  Count){	return _Copy_s( First1,  Count,  First2,  Count);	}
		static char_type*		_Copy_s(char_type* First1, size_t  Dest_size, const char_type* First2, size_t  Count) 
		{	::wmemcpy_s((wchar_t *)First1, Dest_size, (const wchar_t *)First2, Count);		return First1;	}
		static const char_type* find(const char_type * First, size_t Count, const char_type& ch){ return ((const char_type*)::wmemchr((const wchar_t*)First, ch, Count)); }				

		static char_type*		move(char_type * First1, const char_type * First2, size_t  Count)
		{	return _Move_s( First1,  Count,  First2,  Count);	}
		static char_type *		_Move_s(char_type *First1, size_t Dest_size, const char_type *First2, size_t Count)
		{	::wmemmove_s((wchar_t *)First1, Dest_size, (const wchar_t *)First2, Count);	return First1;	}
		static char_type*		assign( char_type* First, size_t Count, char_type ch){	return (char_type*)::wmemset( (wchar_t *)First, ch, Count);	}

		static char_type		to_char_type(const int_type& Meta) {	 return Meta; 	}// convert metacharacter to character
		static int_type			to_int_type(const char_type& ch)	{	return ch;	}	// convert character to metacharacter
		static bool				eq_int_type(const int_type& Left, const int_type& Right) 	{	return (Left == Right);	}// test for metacharacter equality
		static int_type			eof() {	 return WEOF;	}	// return end-of-file metacharacter
		static int_type			not_eof(const int_type& Meta) {	return  Meta != eof() ? Meta : !eof(); }	// return anything but EOF

		using char_traits_base<short>::assign;
	};


}	

#pragma pack(pop)