#pragma once

namespace i3
{
	template<class OutIt, class InputRng, class SearchRng, class RepRng> inline
	OutIt replace_last_copy(OutIt out, const InputRng& in, const SearchRng& s, const RepRng& rep)
	{
		return find_format_copy(out, in, last_finder(s), const_formatter(rep) );
	}
	
	template<class Seq, class SearchRng, class RepRng> inline
	Seq replace_last_copy(const Seq& in, const SearchRng& s, const RepRng& rep)
	{
		return find_format_copy(in, last_finder(s), const_formatter(rep) );
	}

	template<class Seq, class SearchRng, class RepRng> inline
	void replace_last(Seq& inout, const SearchRng& s, const RepRng& rep)
	{
		find_format(inout, last_finder(s), const_formatter(rep) );
	}
	
}

