#pragma once 

#include <istream>

#pragma pack(push, _CRT_PACKING)

namespace i3
{
	template <class B, class C, class T, class A> inline
	stack_basic_string<B, C, T, A> operator+( const stack_basic_string<B, C, T, A>& left, const stack_basic_string<B, C, T, A>& right) { return stack_basic_string<B, C, T, A>(left) += right;	}

	template <class B, class C, class T, class A> inline
	stack_basic_string<B, C, T, A> operator+( const C* left, const stack_basic_string<B, C, T, A>& right) {	return stack_basic_string<B, C, T, A>(left) += right; }

	template <class B, class C, class T, class A> inline
	stack_basic_string<B, C, T, A> operator+( const C left, const stack_basic_string<B, C, T, A>& right)  {	return stack_basic_string<B, C, T, A>(1, left) += right; }

	template <class B, class C, class T, class A> inline
	stack_basic_string<B, C, T, A> operator+( const stack_basic_string<B, C, T, A>& left, const C* right) {	return stack_basic_string<B, C, T, A>(left) += right;	}

	template <class B, class C, class T, class A> inline
	stack_basic_string<B, C, T, A> operator+( const stack_basic_string<B, C, T, A>& left, const C right) {	return stack_basic_string<B, C, T, A>(left) += right;	}
	
	template <class B, class C, class T, class A> inline
	bool	operator==(const stack_basic_string<B, C, T, A>& left, const stack_basic_string<B, C, T, A>& right) {	return left.compare(right) == 0;	}
	
	template <class B, class C, class T, class A> inline
	bool	operator==(const C* left, const stack_basic_string<B, C, T, A>& right) {	return right.compare(left) == 0 ; }

	template <class B, class C, class T, class A> inline
	bool	operator==(const stack_basic_string<B, C, T, A>& left, const C* right) {	return left.compare(right) == 0;	}
	
	template <class B, class C, class T, class A> inline
	bool	operator!=(const stack_basic_string<B, C, T, A>& left, const stack_basic_string<B, C, T, A>& right) {	return !(left == right); }

	template <class B, class C, class T, class A> inline
	bool	operator!=(const C* left, const stack_basic_string<B, C, T, A>& right) {	return !(left == right); }

	template <class B, class C, class T, class A> inline
	bool	operator!=(const stack_basic_string<B, C, T, A>& left, const C* right) {	return !(left == right); }

	template <class B, class C, class T, class A> inline
	bool	operator<(const stack_basic_string<B, C, T, A>& left, const stack_basic_string<B, C, T, A>& right) {	return left.compare(right) < 0; }

	template <class B, class C, class T, class A> inline
	bool	operator<(const C* left, const stack_basic_string<B, C, T, A>& right) {	return right.compare(left) > 0; }

	template <class B, class C, class T, class A> inline
	bool	operator<(const stack_basic_string<B, C, T, A>& left, const C* right) {	return left.compare(right) < 0; }

	template <class B, class C, class T, class A> inline
	bool	operator>(const stack_basic_string<B, C, T, A>& left, const stack_basic_string<B, C, T, A>& right) {	return right < left; }

	template <class B, class C, class T, class A> inline
	bool	operator>(const C* left, const stack_basic_string<B, C, T, A>& right) {	return right < left; }
	
	template <class B, class C, class T, class A> inline
	bool	operator>(const stack_basic_string<B, C, T, A>& left, const C* right) {	return right < left; }

//

	template <class B, class C, class T, class A> inline
	bool	operator<=(const stack_basic_string<B, C, T, A>& left, const stack_basic_string<B, C, T, A>& right) {	return !(right < left); }

	template <class B, class C, class T, class A> inline
	bool	operator<=(const C* left, const stack_basic_string<B, C, T, A>& right) {	return !(right < left); }
	
	template <class B, class C, class T, class A> inline
	bool	operator<=(const stack_basic_string<B, C, T, A>& left, const C* right) {	return !(right < left); }

//
	template <class B, class C, class T, class A> inline
	bool	operator>=(const stack_basic_string<B, C, T, A>& left, const stack_basic_string<B, C, T, A>& right) {	return !(left < right); }

	template <class B, class C, class T, class A> inline
	bool	operator>=(const C* left, const stack_basic_string<B, C, T, A>& right) {	return !(left < right); }
	
	template <class B, class C, class T, class A> inline
	bool	operator>=(const stack_basic_string<B, C, T, A>& left, const C* right){	return !(left < right); }

	
	// stack_basic_string INSERTERS AND EXTRACTORS
	template<class BuffSize, class Ch, class traits, class Alloc> inline 
	std::basic_istream<Ch, traits>& operator>>( std::basic_istream<Ch, traits>& _Istr, stack_basic_string<BuffSize, Ch, traits, Alloc>& _Str)
	{	// extract a string
		typedef std::ctype<Ch> _Ctype;
		typedef std::basic_istream<Ch, traits> _Myis;
		typedef stack_basic_string<BuffSize, Ch, traits, Alloc> _Mystr;
		typedef typename _Mystr::size_type _Mysizt;

		std::ios_base::iostate _State = std::ios_base::goodbit;
		bool _Changed = false;
		const typename _Myis::sentry _Ok(_Istr);

		if (_Ok)
		{	// state okay, extract characters
			const _Ctype& _Ctype_fac = _USE(_Istr.getloc(), _Ctype);
			_Str.erase();

			_TRY_IO_BEGIN
			_Mysizt _Size = 0 < _Istr.width()
				&& (_Mysizt)_Istr.width() < _Str.max_size()
					? (_Mysizt)_Istr.width() : _Str.max_size();
			typename traits::int_type _Meta = _Istr.rdbuf()->sgetc();

			for (; 0 < _Size; --_Size, _Meta = _Istr.rdbuf()->snextc())
				if(traits::eq_int_type(traits::eof(), _Meta))
				{	// end of file, quit
					_State |= std::ios_base::eofbit;
					break;
				}
				else if (_Ctype_fac.is(_Ctype::space,
					traits::to_char_type(_Meta)))
					break;	// whitespace, quit
				else
				{	// add character to string
					_Str.append(1, traits::to_char_type(_Meta));
					_Changed = true;
				}

			_CATCH_IO_(_Istr)
		}

		_Istr.width(0);
		if (!_Changed)
			_State |= std::ios_base::failbit;
		_Istr.setstate(_State);
		return (_Istr);
	}

	template<class BuffSize, class Ch,	class traits,	class Alloc> inline
	std::basic_istream<Ch, traits>& getline( std::basic_istream<Ch, traits>& _Istr, stack_basic_string<BuffSize, Ch, traits, Alloc>& _Str, const Ch _Delim)
	{	// get characters into string, discard delimiter
		typedef std::basic_istream<Ch, traits> _Myis;
		std::ios_base::iostate _State = std::ios_base::goodbit;
		bool _Changed = false;
		const typename _Myis::sentry _Ok(_Istr, true);

		if (_Ok)
		{	// state okay, extract characters
			_TRY_IO_BEGIN
			_Str.erase();
			const typename traits::int_type _Metadelim =
				traits::to_int_type(_Delim);
			typename traits::int_type _Meta = _Istr.rdbuf()->sgetc();

			for (; ; _Meta = _Istr.rdbuf()->snextc())
				if (traits::eq_int_type(traits::eof(), _Meta))
					{	// end of file, quit
					_State |= std::ios_base::eofbit;
					break;
					}
				else if (traits::eq_int_type(_Meta, _Metadelim))
					{	// got a delimiter, discard it and quit
					_Changed = true;
					_Istr.rdbuf()->sbumpc();
					break;
					}
				else if (_Str.max_size() <= _Str.size())
					{	// string too large, quit
					_State |= std::ios_base::failbit;
					break;
					}
				else
					{	// got a character, add it to string
					_Str += traits::to_char_type(_Meta);
					_Changed = true;
					}
			_CATCH_IO_(_Istr)
		}

		if (!_Changed)
			_State |= std::ios_base::failbit;
		_Istr.setstate(_State);
		return (_Istr);
	}

	template<class BuffSize, class Ch, class traits, class Alloc> inline
	std::basic_istream<Ch, traits>& getline(	std::basic_istream<Ch, traits>& _Istr, stack_basic_string<BuffSize, Ch, traits, Alloc>& _Str)
	{	// get characters into string, discard newline
		return (getline(_Istr, _Str, _Istr.widen('\n')));
	}

	template<class BuffSize, class Ch, class traits, class Alloc> inline
	std::basic_ostream<Ch, traits>& operator<<( std::basic_ostream<Ch, traits>& _Ostr, const stack_basic_string<BuffSize, Ch, traits, Alloc>& _Str)
	{	// insert a string
		typedef std::basic_ostream<Ch, traits> _Myos;
		typedef stack_basic_string<BuffSize, Ch, traits, Alloc> _Mystr;
		typedef typename _Mystr::size_type _Mysizt;

		std::ios_base::iostate _State = std::ios_base::goodbit;
		_Mysizt _Size = _Str.size();
		_Mysizt _Pad = _Ostr.width() <= 0 || (_Mysizt)_Ostr.width() <= _Size
				? 0 : (_Mysizt)_Ostr.width() - _Size;
		const typename _Myos::sentry _Ok(_Ostr);

		if (!_Ok)
			_State |= std::ios_base::badbit;
		else
		{	// state okay, insert characters
		_TRY_IO_BEGIN
			if ((_Ostr.flags() & std::ios_base::adjustfield) != std::ios_base::left)
				for (; 0 < _Pad; --_Pad)	// pad on left
					if (traits::eq_int_type(traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill())))
					{	// insertion failed, quit
						_State |= std::ios_base::badbit;
						break;
					}

			if (_State == std::ios_base::goodbit)
				for (_Mysizt _Count = 0; _Count < _Size; ++_Count)
					if (traits::eq_int_type(traits::eof(),	_Ostr.rdbuf()->sputc(_Str[_Count])))
					{	// insertion failed, quit
						_State |= std::ios_base::badbit;
						break;
					}

			if (_State == std::ios_base::goodbit)
				for (; 0 < _Pad; --_Pad)	// pad on right
					if (traits::eq_int_type(traits::eof(), _Ostr.rdbuf()->sputc(_Ostr.fill())))
					{	// insertion failed, quit
						_State |= std::ios_base::badbit;
						break;
					}
			_Ostr.width(0);
			_CATCH_IO_(_Ostr)
		}

		_Ostr.setstate(_State);
		return (_Ostr);
	}




}

#pragma pack(pop)