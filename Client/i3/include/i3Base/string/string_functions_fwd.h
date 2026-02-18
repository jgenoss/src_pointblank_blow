#pragma once

#include "../itl/range/detail/enable_if_iterator.h"
#include "../itl/undef_minmax_begin.h"

// NOTICE : 여기에 스트링 함수 include를 하면 안됩니다..

// NOTICE : 향후 문자열 함수 추가될때마다, 각 함수별h 구현파일 작성 후 이곳에도 빠뜨리지 않고 함수선언을 기록하도록
//          합니다.           

namespace i3
{
	// string/algorithm folder

	template<class OutIt, class Range> inline 
	typename iterator_return<OutIt>::type to_lower_copy(OutIt Output, const Range& Input);

	template<class Seq, class Range> inline 
	typename non_iterator_return<Seq>::type to_lower_copy(Seq& Output, const Range& Input);

	template<class Seq> inline
	Seq to_lower_copy( const Seq& Input);

	template<class InOutRange> inline
	void to_lower(InOutRange& InOut);

	template<class OutIt, class Range> inline 
	typename iterator_return<OutIt>::type to_upper_copy(OutIt Output, const Range& Input);

	template<class Seq, class Range> inline 
	typename non_iterator_return<Seq>::type to_upper_copy(Seq& Output, const Range& Input);

	template<class Seq> inline
	Seq to_upper_copy( const Seq& Input);

	template<class InOutRange> inline
	void to_upper(InOutRange& InOut);

	// string/compare folder

	template<class Rng1, class Rng2> inline
	bool	generic_is_equal( const Rng1& lhs, const Rng2& rhs );
	template<class Rng1, class Rng2> inline
	bool	generic_is_iequal( const Rng1& lhs, const Rng2& rhs );
	template<class Rng1, class Rng2> inline
	bool	generic_is_nequal( const Rng1& lhs, const Rng2& rhs, std::size_t n );
	template<class Rng1, class Rng2> inline
	bool	generic_is_niequal( const Rng1& lhs, const Rng2& rhs, std::size_t n );

	template<class Rng1, class Rng2> inline 
	bool generic_is_less(const Rng1& lhs, const Rng2& rhs);
	template<class Rng1, class Rng2> inline 
	bool generic_is_iless(const Rng1& lhs, const Rng2& rhs);
	template<class Rng1, class Rng2> inline 
	bool generic_is_nless(const Rng1& lhs, const Rng2& rhs, std::size_t n);
	template<class Rng1, class Rng2> inline 
	bool generic_is_niless(const Rng1& lhs, const Rng2& rhs, std::size_t n);	

	template<class Rng1, class Rng2> inline
	int	generic_compare( const Rng1& lhs, const Rng2& rhs );
	template<class Rng1, class Rng2> inline
	int	generic_icompare( const Rng1& lhs, const Rng2& rhs );
	template<class Rng1, class Rng2> inline
	int	generic_ncompare( const Rng1& lhs, const Rng2& rhs, std::size_t n );
	template<class Rng1, class Rng2> inline
	int	generic_nicompare( const Rng1& lhs, const Rng2& rhs, std::size_t n );

	// string/ext folder
	template<class Range> __forceinline
	double atod(const Range& rng);
	template<class Range> __forceinline
	float  atof(const Range& rng);
	template<class Range> inline 
	int atoi(const Range& rng);

	template<class Lit, class Rng> inline typename enable_if_iterator<Lit>::type 
	change_file_ext(Lit name, const Rng& ext);
	template<class Seq, class Rng> inline typename disable_if_iterator<Seq>::type 
	change_file_ext(Seq& name, const Rng& ext);
	template<class Lit, class InRng, class ExtRng> inline typename enable_if_iterator<Lit>::type 
	change_file_ext_copy(Lit out, const InRng& src, const ExtRng& ext);
	template<class Seq, class InRng, class ExtRng> inline typename disable_if_iterator<Seq>::type 
	change_file_ext_copy(Seq& out, const InRng& src, const ExtRng& ext);

	template<class Rng>
	int contain_char( const Rng& rng, typename range_value<Rng>::type ch );
	template<class Rng>
	int contain_char_rev( const Rng& rng, typename range_value<Rng>::type ch );
	template<class Range> inline
	int contain_non_ascii( const Range& rng );

	template<class Range1, class Range2> inline
	int contain_string(const Range1& str_src, const Range2& str_find);
	template<class Range1, class Range2> inline
	int icontain_string(const Range1& str_src, const Range2& str_find);
	template<class Range1, class Range2> inline
	bool contain_string_head(const Range1& str_src, const Range2& str_find);
	template<class Range1, class Range2> inline
	bool icontain_string_head(const Range1& str_src, const Range2& str_find);
	template<class Range1, class Range2> inline
	bool contain_string_tail(const Range1& str_src, const Range2& str_find);
	template<class Range1, class Range2> inline
	bool icontain_string_tail(const Range1& str_src, const Range2& str_find);

	template<class Rng, class OutIt> inline
	typename enable_if_iterator<OutIt, size_t>::type 
	erase_space(const Rng& in, OutIt out);
	template<class Rng, class OutSeq> inline 
	typename disable_if_iterator<OutSeq, size_t>::type 
	erase_space(const Rng& in, OutSeq& out);

	template<class DestRange> inline
	typename i3::non_iterator_return<DestRange>::type erase_string(
	DestRange& dst, size_t pos, size_t num = 1 );
	template<class DestIter> inline
	typename i3::iterator_return<DestIter>::type erase_string(
	DestIter dst, size_t pos, size_t num = 1 );

	template<class Range, class Lit> inline typename i3::enable_if_iterator<Lit>::type
	extract_directoryname(const Range& path, Lit out);
	template<class Range, class Seq> inline typename i3::disable_if_iterator<Seq>::type
	extract_directoryname(const Range& path, Seq& out);

	template<class Range, class Ch> 
	void extract_directoryname_filename_iterator_range( 
		const Range& path, i3::iterator_range<Ch*>& outDir, 
		i3::iterator_range<Ch*>& outFile);

	template<class Range, class Lit1, class Lit2> inline 
	typename i3::enable_if<mpl::and_< is_iterator<Lit1>, is_iterator<Lit2> > >::type
	extract_directoryname_filename( const Range& path, Lit1 outDir, Lit2 outFile);
	template<class Range, class Lit1, class Seq2> inline 
	typename i3::enable_if<mpl::and_< is_iterator<Lit1>, mpl::not_<is_iterator<Seq2> > > >::type
	extract_directoryname_filename( const Range& path, Lit1 outDir, Seq2& outFile);
	template<class Range, class Seq1, class Lit2> inline 
	typename i3::enable_if<mpl::and_< mpl::not_<is_iterator<Seq1> >, is_iterator<Lit2> > >::type
	extract_directoryname_filename( const Range& path, Seq1& outDir, Lit2 outFile);
	template<class Range, class Seq1, class Seq2> inline 
	typename i3::enable_if< mpl::not_< mpl::or_< is_iterator<Seq1>, is_iterator<Seq2> > > >::type
	extract_directoryname_filename( const Range& path, Seq1& outDir, Seq2& outFile);

	template<class Range, class Lit> inline typename i3::enable_if_iterator<Lit>::type
	extract_fileext(const Range& path, Lit out);
	template<class Range, class Seq> inline typename i3::disable_if_iterator<Seq>::type
	extract_fileext(const Range& path, Seq& out);

	template<class Range, class Lit> inline typename i3::enable_if_iterator<Lit>::type
	extract_filename(const Range& path, Lit out);
	template<class Range, class Seq> inline typename i3::disable_if_iterator<Seq>::type
	extract_filename(const Range& path, Seq& out);
	template<class Range, class Lit> inline typename i3::enable_if_iterator<Lit>::type
	extract_filetitle(const Range& path, Lit out);
	template<class Range, class Seq> inline typename i3::disable_if_iterator<Seq>::type
	extract_filetitle(const Range& path, Seq& out);

	template<class Range, class Ch> 
	void extract_filetitle_fileext_iterator_range( const Range& path, i3::iterator_range<Ch*>& outtitle, 
		i3::iterator_range<Ch*>& outext);

	template<class Range, class Lit1, class Lit2> inline 
	typename i3::enable_if<mpl::and_< is_iterator<Lit1>, is_iterator<Lit2> > >::type
	extract_filetitle_fileext( const Range& path, Lit1 outTitle, Lit2 outExt);
	template<class Range, class Lit1, class Seq2> inline 
	typename i3::enable_if<mpl::and_< is_iterator<Lit1>, mpl::not_<is_iterator<Seq2> > > >::type
	extract_filetitle_fileext( const Range& path, Lit1 outTitle, Seq2& outExt);
	template<class Range, class Seq1, class Lit2> inline 
	typename i3::enable_if<mpl::and_< mpl::not_<is_iterator<Seq1> >, is_iterator<Lit2> > >::type
	extract_filetitle_fileext( const Range& path, Seq1& outTitle, Lit2 outExt);
	template<class Range, class Seq1, class Seq2> inline 
	typename i3::enable_if< mpl::not_< mpl::or_< is_iterator<Seq1>, is_iterator<Seq2> > > >::type
	extract_filetitle_fileext( const Range& path, Seq1& outTitle, Seq2& outExt);

	namespace detail {	template<class T>	struct lazy_rcstring; }

	template<class FmtRange> inline typename i3::lazy_disable_if< i3::is_base_of<sprintf_formatter_base, FmtRange>, 
	detail::lazy_rcstring<FmtRange> >::type format_rcstring(const FmtRange& rng);
	template<class FSeq> inline 
	i3::rc_basic_string<typename i3::range_value<FSeq>::type > format_rcstring( const sprintf_formatter<FSeq>& fmter);


	namespace detail {	template<class T>	struct lazy_string; }

	template<class FmtRange> inline typename i3::lazy_disable_if< i3::is_base_of<sprintf_formatter_base, FmtRange>, 
	detail::lazy_string<FmtRange> >::type format_string(const FmtRange& rng);

	template<class FSeq> inline 
	i3::basic_string<typename i3::range_value<FSeq>::type > format_string( const sprintf_formatter<FSeq>& fmter);

	template<class Real, class OutLit> inline
	typename i3::enable_if_iterator<OutLit>::type
	ftoa(Real val, OutLit lit, int fractLen = -1);
	template<class Real, class OutSeq> inline 
	typename i3::disable_if_iterator<OutSeq>::type
	ftoa(Real val, OutSeq& seq, int fractLen = -1);

	template<class Integral, class Range> inline typename i3::enable_if<
		mpl::and_<is_integral<Integral>, 
		mpl::not_<is_iterator<Range> > >,
		void>::type			itoa( Integral val, Range& out);

	template<class Integral, class Range> inline typename i3::enable_if<
		mpl::and_<is_integral<Integral>, 
		mpl::not_<is_iterator<Range> > >,
		void>::type		itoa( Integral val, int radix, Range& out);

	template<class Integral, class Lit> inline typename i3::enable_if<
		mpl::and_<is_integral<Integral>, 
		is_iterator<Lit> >,
		void>::type			itoa( Integral val, Lit out, size_t buff_size);

	template<class Integral, class Lit> inline typename i3::enable_if<
		mpl::and_<is_integral<Integral>, 
		is_iterator<Lit> >,
		void>::type		itoa( Integral val, int radix, Lit out, size_t buff_size);

	template<class Integral, class Range> inline typename i3::enable_if<
		mpl::and_<is_integral<Integral>, 
		mpl::not_<is_iterator<Range> > >,
		void>::type			itoa_comma( Integral val, Range& out);


	template<class Integral, class Lit> inline typename i3::enable_if<
		mpl::and_<is_integral<Integral>, 
		is_iterator<Lit> >,
		void>::type			itoa_comma( Integral val, Lit out, size_t buff_size);

	template<class Range, class DelimitRange> inline
	typename i3::range_iterator<const Range>::type generic_find_first_not_of(const Range& input, const DelimitRange& delimit);
	template<class Range> inline
	typename i3::range_iterator<const Range>::type generic_find_first_not_of(const Range& input, typename i3::range_value<Range>::type delimit_c);
	template<class InIt, class DelimitRange> inline
	typename InIt generic_find_first_not_of( InIt beg, InIt end, const DelimitRange& delimit);
	template<class InIt> inline
	typename InIt generic_find_first_not_of( InIt beg, InIt end, typename i3::iterator_value<InIt>::type delimit_c);

	template<class Range, class DelimitRange> inline
	typename i3::range_iterator<const Range>::type generic_find_first_of(const Range& input, const DelimitRange& delimit);
	template<class Range> inline
	typename i3::range_iterator<const Range>::type generic_find_first_of(const Range& input, typename i3::range_value<Range>::type delimit_c);
	template<class InIt, class DelimitRange> inline
	typename InIt generic_find_first_of( InIt beg, InIt end, const DelimitRange& delimit);
	template<class InIt> inline
	typename InIt generic_find_first_of( InIt beg, InIt end, typename i3::iterator_value<InIt>::type delimit_c);

	template<class Range, class DelimitRange> inline
	typename i3::range_iterator<const Range>::type generic_find_last_not_of(const Range& input, const DelimitRange& delimit);
	template<class Range> inline
	typename i3::range_iterator<const Range>::type generic_find_last_not_of(const Range& input, typename i3::range_value<Range>::type delimit_c);
	template<class InIt, class DelimitRange> inline
	typename InIt generic_find_last_not_of( InIt beg, InIt end, const DelimitRange& delimit);
	template<class InIt> inline
	typename InIt generic_find_last_not_of( InIt beg, InIt end, typename i3::iterator_value<InIt>::type delimit_c);

	template<class Range, class DelimitRange> inline
	typename i3::range_iterator<const Range>::type generic_find_last_of(const Range& input, const DelimitRange& delimit);
	template<class Range> inline
	typename i3::range_iterator<const Range>::type generic_find_last_of(const Range& input, typename i3::range_value<Range>::type delimit_c);
	template<class InIt, class DelimitRange> inline
	typename InIt generic_find_last_of( InIt beg, InIt end, const DelimitRange& delimit);
	template<class InIt> inline
	typename InIt generic_find_last_of( InIt beg, InIt end, typename i3::iterator_value<InIt>::type delimit_c);

	template<class Seq, class Range> inline 
	typename i3::disable_if_iterator<Seq, std::size_t>::type 
	generic_string_cat(Seq& s, const Range& rng);
	template<class Literal, class Range> inline 
	typename i3::enable_if_iterator<Literal, std::size_t>::type 
	generic_string_cat(Literal ctn, const Range& rng);

	template<class Seq, class Range> inline
	typename i3::disable_if_iterator<Seq, size_t>::type generic_string_copy(Seq& out, const Range& r);
	template<class Seq> inline
	typename i3::disable_if_iterator<Seq, size_t>::type generic_string_copy(Seq& out, const Seq& r);
	template<class It, class Range> inline
	typename i3::enable_if_iterator<It, size_t>::type generic_string_copy(It out, const Range& r);
	template<class Seq, class InIt> inline
	typename i3::disable_if_iterator<Seq, size_t>::type generic_string_copy(Seq& out, InIt f, InIt l);
	template<class It, class InIt> inline
	typename i3::enable_if_iterator<It, size_t>::type generic_string_copy(It out, InIt f, InIt l);

	template<class Seq, class FwdRange> 
	typename i3::disable_if_iterator<Seq, std::size_t>::type 
	generic_string_ncat(Seq& out, const FwdRange& src_rng, size_t len);
	template<class It, class FwdRange> 
	typename i3::enable_if_iterator<It, std::size_t>::type 
	generic_string_ncat(It out, const FwdRange& src_rng, size_t len);

	template<class Seq> inline
	typename i3::disable_if_iterator<Seq, std::size_t>::type 
	generic_string_ncopy(Seq& out, const Seq& src_rng, size_t len);
	template<class Seq, class FwdRange> inline
	typename i3::disable_if_iterator<Seq, std::size_t>::type 
	generic_string_ncopy(Seq& out, const FwdRange& src_rng, size_t len);
	template<class It, class FwdRange> inline
	typename i3::enable_if_iterator<It, std::size_t>::type 
	generic_string_ncopy(It out, const FwdRange& src_rng, size_t len);

	template<class InOutLit, class BeforeRng, class AfterRng> inline 
	typename i3::enable_if_iterator<InOutLit>::type
	generic_string_replace_all(InOutLit out, const BeforeRng& before, const AfterRng& after);
	template<class InOutSeq, class BeforeRng, class AfterRng> inline
	typename i3::disable_if_iterator<InOutSeq>::type
	generic_string_replace_all(InOutSeq& out, const BeforeRng& before, const AfterRng& after);
	template<class InOutLit, class BeforeRng, class AfterRng> inline 
	typename i3::enable_if_iterator<InOutLit>::type
	generic_string_ireplace_all(InOutLit out, const BeforeRng& before, const AfterRng& after);
	template<class InOutSeq, class BeforeRng, class AfterRng> inline
	typename i3::disable_if_iterator<InOutSeq>::type
	generic_string_ireplace_all(InOutSeq& out, const BeforeRng& before, const AfterRng& after);

	template<class OutLit, class InputRng, class BeforeRng, class AfterRng> inline 
	typename i3::enable_if_iterator<OutLit>::type
	generic_string_replace_all_copy(OutLit out, const InputRng& in, const BeforeRng& before, const AfterRng& after);
	template<class OutSeq, class InputRng, class BeforeRng, class AfterRng> inline
	typename i3::disable_if_iterator<OutSeq>::type
	generic_string_replace_all_copy(OutSeq& out, const InputRng& in, const BeforeRng& before, const AfterRng& after);
	template<class OutLit, class InputRng, class BeforeRng, class AfterRng> inline 
	typename i3::enable_if_iterator<OutLit>::type
	generic_string_ireplace_all_copy(OutLit out, const InputRng& in, const BeforeRng& before, const AfterRng& after);
	template<class OutSeq, class InputRng, class BeforeRng, class AfterRng> inline
	typename i3::disable_if_iterator<OutSeq>::type
	generic_string_ireplace_all_copy(OutSeq& out, const InputRng& in, const BeforeRng& before, const AfterRng& after);

	template<class Literal> inline typename enable_if_iterator<Literal,
	size_t>::type generic_string_size(Literal i);
	template<class Seq> inline typename disable_if_iterator<Seq,
	size_t>::type generic_string_size(const Seq& s);

	template<class Rng, class OutSeq>  inline
	typename disable_if_iterator<OutSeq, bool>::type 
	get_environment_variable( const Rng& input, OutSeq& out);

	template<class OutLit> inline typename i3::enable_if_iterator<OutLit>::type 
	get_exec_path( OutLit out);
	template<class OutSeq> inline typename i3::disable_if_iterator<OutSeq>::type
	get_exec_path(OutSeq& out);

	template<class Rng> inline typename i3::range_iterator<Rng>::type
	get_first_valid_character(const Rng& str);

	template<class Seq> typename i3::disable_if_iterator<Seq, 
	DWORD>::type get_module_filename( Seq& seq );
	template<class S,class C, class T, class A> inline
	DWORD get_module_filename(i3::stack_basic_string<S,C,T,A>& seq);
	inline DWORD	get_module_filename(char* out, DWORD size);
	inline DWORD	get_module_filename(wchar_t* out, DWORD size);

	template<class Lit, class Rng> inline typename i3::enable_if_iterator<Lit>::type
	get_prefix_name(Lit dest, const Rng& src);
	template<class Seq, class Rng> inline typename i3::disable_if_iterator<Seq>::type
	get_prefix_name(Seq& dest, const Rng& src);

	template<class DestRange, class SrcRange> inline
	typename i3::non_iterator_return<DestRange>::type insert_string(
	DestRange& dst, size_t dst_pos, const SrcRange& src,  size_t src_len = std::numeric_limits<size_t>::max() );
	template<class DestIter, class SrcRange> inline
	typename i3::iterator_return<DestIter>::type insert_string(
	DestIter dst, size_t dst_pos, const SrcRange& src, size_t src_len = std::numeric_limits<size_t>::max() );

	template<class Lit, class SrcRange, class SuffixRange> inline
		typename i3::enable_if_iterator<Lit>::type
		insert_suffix_to_filename(Lit ctn, const SrcRange& src, 
		const SuffixRange& suffix);
	template<class Seq, class SrcRange, class SuffixRange> inline
		typename i3::disable_if_iterator<Seq>::type
		insert_suffix_to_filename(Seq& ctn, const SrcRange& src, 
		const SuffixRange& suffix);

	template<class PathRange, class WildCardRange>  
	bool is_matched_wildcard( const PathRange& filename, const WildCardRange& wildcard);

	I3_EXPORT_BASE bool is_utf8( const char* str, std::size_t len);


	template<class Literal, class DelimitRng> inline typename enable_if_iterator<Literal,
	void>::type ltrim(Literal inout, const DelimitRng& delimit);
	template<class Seq, class DelimitRng> inline typename disable_if_iterator<Seq,
	void>::type ltrim(Seq& inout, const DelimitRng& delimit);
	template<class OutRange, class InputRange, class DelimitRng> inline typename disable_if_iterator<OutRange,
	void>::type ltrim_copy(OutRange& out, const InputRange& input, const DelimitRng& delimit);
	template<class OutLit, class InputRange, class DelimitRng> inline typename enable_if_iterator<OutLit,
	void>::type ltrim_copy(OutLit out, size_t buff_size, const InputRange& input, const DelimitRng& delimit);

	template<class Literal> inline typename enable_if_iterator<Literal,
	void>::type ltrim(Literal inout);
	template<class Seq> inline typename disable_if_iterator<Seq,
	void>::type ltrim(Seq& inout);
	template<class OutRange, class InputRange> inline typename disable_if_iterator<OutRange,
	void>::type ltrim_copy(OutRange& out, const InputRange& input);
	template<class OutLit, class InputRange> inline typename enable_if_iterator<OutLit,
	void>::type ltrim_copy(OutLit out, size_t buff_size, const InputRange& input);


	template<class Literal, class DelimitRng> inline typename enable_if_iterator<Literal,
	void>::type rtrim(Literal inout, const DelimitRng& delimit);
	template<class Seq, class DelimitRng> inline typename disable_if_iterator<Seq,
	void>::type rtrim(Seq& inout, const DelimitRng& delimit);
	template<class OutRange, class InputRange, class DelimitRng> inline typename disable_if_iterator<OutRange,
	void>::type rtrim_copy(OutRange& out, const InputRange& input, const DelimitRng& delimit);
	template<class OutLit, class InputRange, class DelimitRng> inline typename enable_if_iterator<OutLit,
	void>::type rtrim_copy(OutLit out, size_t buff_size, const InputRange& input, const DelimitRng& delimit);

	template<class Literal> inline typename enable_if_iterator<Literal,
	void>::type rtrim(Literal inout);
	template<class Seq> inline typename disable_if_iterator<Seq,
	void>::type rtrim(Seq& inout);
	template<class OutRange, class InputRange> inline typename disable_if_iterator<OutRange,
	void>::type rtrim_copy(OutRange& out, const InputRange& input);
	template<class OutLit, class InputRange> inline typename enable_if_iterator<OutLit,
	void>::type rtrim_copy(OutLit out, size_t buff_size, const InputRange& input);

	template<class Literal, class DelimitRng> inline typename enable_if_iterator<Literal,
	void>::type lrtrim(Literal inout, const DelimitRng& delimit);
	template<class Seq, class DelimitRng> inline typename disable_if_iterator<Seq,
	void>::type lrtrim(Seq& inout, const DelimitRng& delimit);
	template<class OutRange, class InputRange, class DelimitRng> inline typename disable_if_iterator<OutRange,
	void>::type lrtrim_copy(OutRange& out, const InputRange& input, const DelimitRng& delimit);
	template<class OutLit, class InputRange, class DelimitRng> inline typename enable_if_iterator<OutLit,
	void>::type lrtrim_copy(OutLit out, size_t buff_size, const InputRange& input, const DelimitRng& delimit);

	template<class Literal> inline typename enable_if_iterator<Literal,
	void>::type lrtrim(Literal inout);
	template<class Seq> inline typename disable_if_iterator<Seq,
	void>::type lrtrim(Seq& inout);
	template<class OutRange, class InputRange> inline typename disable_if_iterator<OutRange,
	void>::type lrtrim_copy(OutRange& out, const InputRange& input);
	template<class OutLit, class InputRange> inline typename enable_if_iterator<OutLit,
	void>::type lrtrim_copy(OutLit out, size_t buff_size, const InputRange& input);

	template<class InRng1, class InRng2, class OutRng> inline typename i3::disable_if_iterator<OutRng>::type
	make_relative_path( const InRng1& base_path, const InRng2& abs_path, OutRng& out_rel_path);
	template<class InRng1, class InRng2, class OutIt> inline typename i3::enable_if_iterator<OutIt>::type
	make_relative_path( const InRng1& base_path, const InRng2& abs_path, OutIt out_rel_path);

	template<class Range> inline typename i3::disable_if_iterator<Range>::type
	make_unix_path(Range& path);
	template<class Lit> inline typename i3::enable_if_iterator<Lit>::type
	make_unix_path(Lit path);

	__forceinline unsigned int	curr_cp();

	template<class MBRange, class WideSeq> inline typename i3::disable_if_iterator<WideSeq>::type
	mb_to_utf16( const MBRange& src, WideSeq& dest, unsigned int codepage = curr_cp() );
	template<class MBRange, class WideLit> inline typename i3::enable_if_iterator<WideLit>::type
	mb_to_utf16( const MBRange& src, WideLit dest, std::size_t dest_buff_size, unsigned int codepage = curr_cp() );
	template<class MBRange, class U8Seq> inline typename i3::disable_if_iterator<U8Seq>::type
	mb_to_utf8( const MBRange& src, U8Seq& dest, unsigned int codepage = curr_cp() );
	template<class MBRange, class U8Lit> inline typename i3::enable_if_iterator<U8Lit>::type
	mb_to_utf8( const MBRange& src, U8Lit dest, std::size_t dest_buff_size, unsigned int codepage = curr_cp() );

	template<class WideRange, class MBSeq> inline typename i3::disable_if_iterator<MBSeq>::type
	utf16_to_mb( const WideRange& src, MBSeq& dest, unsigned int codepage = curr_cp() );
	template<class WideRange, class MBLit> inline typename i3::enable_if_iterator<MBLit>::type
	utf16_to_mb( const WideRange& src, MBLit dest, std::size_t dest_buff_size, unsigned int codepage = curr_cp() );
	template<class WideRange, class U8Seq> inline typename i3::disable_if_iterator<U8Seq>::type
	utf16_to_utf8( const WideRange& src, U8Seq& dest);
	template<class WideRange, class U8Lit> inline typename i3::enable_if_iterator<U8Lit>::type
	utf16_to_utf8( const WideRange& src, U8Lit dest, std::size_t dest_buff_size);

	template<class U8Range, class MBSeq> inline typename i3::disable_if_iterator<MBSeq>::type
	utf8_to_mb( const U8Range& src, MBSeq& dest, unsigned int codepage = curr_cp() );
	template<class U8Range, class MBLit> inline typename i3::enable_if_iterator<MBLit>::type
	utf8_to_mb( const U8Range& src, MBLit dest, std::size_t dest_buff_size, unsigned int codepage = curr_cp() );
	template<class U8Range, class WideSeq> inline typename i3::disable_if_iterator<WideSeq>::type
	utf8_to_utf16( const U8Range& src, WideSeq& dest);
	template<class U8Range, class WideLit> inline typename i3::enable_if_iterator<WideLit>::type
	utf8_to_utf16( const U8Range& src, WideLit dest, std::size_t dest_buff_size);

	template<class Lit, class OutSeq> typename enable_if_iterator<Lit>::type
	parse_multiselect_filename( Lit lit, i3::vector<OutSeq>& inout);
	template<class Seq, class OutSeq> typename disable_if_iterator<Seq>::type
	parse_multiselect_filename( const Seq& seq, i3::vector<OutSeq>& inout);

	template<class Seq> inline 
	typename i3::disable_if_iterator<Seq>::type remove_all_blank( Seq& s);
	template<class It> inline
	typename i3::enable_if_iterator<It>::type remove_all_blank(It it);

	template<class Literal> inline typename i3::enable_if_iterator<Literal>::type 
	remove_all_char(Literal ctn, typename i3::iterator_value<Literal>::type c);
	template<class Seq> inline typename i3::disable_if_iterator<Seq>::type
	remove_all_char(Seq& s, typename i3::range_value<Seq>::type c);

	template<class Literal> inline typename i3::enable_if_iterator<Literal>::type 
	remove_all_space(Literal ctn);
	template<class Seq> inline typename i3::disable_if_iterator<Seq>::type
	remove_all_space(Seq& s);

	template< class Lit> inline typename i3::enable_if_iterator<Lit>::type
	remove_drivename(Lit inout);
	template< class Seq> inline typename i3::disable_if_iterator<Seq>::type
	remove_drivename(Seq& inout);

	template<class Lit> 
	typename enable_if_iterator<Lit>::type
	remove_end_slash( Lit inout );
	template<class Seq> 
	typename disable_if_iterator<Seq>::type
	remove_end_slash( Seq& inout );

	template<class Lit> inline 
	typename i3::enable_if_iterator<Lit>::type
	remove_ext( Lit lit);
	template<class Seq> inline 
	typename i3::disable_if_iterator<Seq>::type
	remove_ext( Seq& s);

	template< class Lit> inline typename i3::enable_if_iterator<Lit, std::size_t>::type
	remove_filename(Lit inout);
	template< class Seq> inline typename i3::disable_if_iterator<Seq, std::size_t>::type
	remove_filename(Seq& inout);

	template< class Lit> inline typename i3::enable_if_iterator<Lit, std::size_t>::type
	remove_slash_filename(Lit inout);
	template< class Seq> inline typename i3::disable_if_iterator<Seq, std::size_t>::type
	remove_slash_filename(Seq& inout);

	template<class Rng, class OutSeq>  inline
	typename disable_if_iterator<OutSeq>::type 
	resolve_path_from_environment_variable(const Rng& input, OutSeq& output);

	template<class InRng, class DestRng> inline
	void	resolve_path_token(const InRng& path, i3::vector<DestRng>& out_list);

	template<class Seq> inline
	typename i3::disable_if_iterator<Seq, std::size_t>::type 
	safe_string_copy(Seq& out, const Seq& src_rng, size_t out_buff_size);
	template<class Seq, class FwdRange> inline
	typename i3::disable_if_iterator<Seq, std::size_t>::type 
	safe_string_copy(Seq& out, const FwdRange& src_rng, size_t out_buff_size);
	template<class It, class FwdRange> inline
	typename i3::enable_if_iterator<It, std::size_t>::type 
	safe_string_copy(It out, const FwdRange& src_rng, size_t out_buff_size);
	template<class Seq, class FwdIt> inline
	typename i3::disable_if_iterator<Seq, std::size_t>::type 
	safe_string_copy(Seq& out, FwdIt src_beg, FwdIt src_end, size_t out_buff_size);
	template<class It, class FwdIt> inline
	typename i3::enable_if_iterator<It, std::size_t>::type 
	safe_string_copy(It out, FwdIt src_beg, FwdIt src_end, size_t out_buff_size);

	template<class Seq, class FwdRange> inline typename i3::disable_if_iterator<Seq, 
	std::size_t>::type safe_string_cat(Seq& out, const FwdRange& src_rng, size_t out_buff_size);
	template<class It, class FwdRange> inline typename i3::enable_if_iterator<It, 
	std::size_t>::type safe_string_cat(It out, const FwdRange& src_rng, size_t out_buff_size);



	template<class SeqOrCh> class sprintf_formatter;

	typedef sprintf_formatter<const char*>			literal_formatter;
	typedef sprintf_formatter<const wchar_t*>		wliteral_formatter;

	typedef sprintf_formatter<i3::string>			string_formatter;
	typedef sprintf_formatter<i3::wstring>			wstring_formatter;
	typedef sprintf_formatter<i3::rc_string>		rc_string_formatter;
	typedef sprintf_formatter<i3::rc_wstring>		rc_wstring_formatter;
	typedef sprintf_formatter<i3::fixed_string>		fixed_string_formatter;
	typedef sprintf_formatter<i3::fixed_wstring>	fixed_wstring_formatter;
	typedef sprintf_formatter<i3::stack_string>		stack_string_formatter;
	typedef sprintf_formatter<i3::stack_wstring>	stack_wstring_formatter;

	template<class OutIt, class FmtRange> inline 
	typename i3::enable_if_iterator<OutIt,	
	size_t>::type snprintf( OutIt out, std::size_t out_num, const FmtRange& rng);
	template<class OutSeq, class FmtRange> inline 
	typename i3::disable_if_iterator<OutSeq,	
	size_t>::type snprintf( OutSeq& out, std::size_t out_num, const FmtRange& rng);
	template<class OutIt, class FSeq> inline 
	typename i3::enable_if_iterator<OutIt,	
	size_t>::type snprintf( OutIt out, std::size_t out_num, const sprintf_formatter<FSeq>& fmter);
	template<class OutSeq, class FSeq> inline 
	typename i3::disable_if_iterator<OutSeq,	
	size_t>::type snprintf( OutSeq& out, std::size_t out_num, const sprintf_formatter<FSeq>& fmter);

	template<class OutIt, class FmtRange> inline 
	typename i3::enable_if_iterator<OutIt,	
	size_t>::type sprintf( OutIt out, const FmtRange& rng);
	template<class OutSeq, class FmtRange> inline 
	typename i3::disable_if_iterator<OutSeq,	
	size_t>::type sprintf( OutSeq& out, const FmtRange& rng);
	template<class OutIt, class FSeq> inline 
	typename i3::enable_if_iterator<OutIt,	
	size_t>::type sprintf( OutIt out, const sprintf_formatter<FSeq>& fmter);
	template<class OutSeq, class FSeq> inline 
	typename i3::disable_if_iterator<OutSeq,	
	size_t>::type sprintf( OutSeq& out, const sprintf_formatter<FSeq>& fmter);

	template<class SeqSeq, class RangeSrc, class RangeDelimit> inline
	SeqSeq& strtok( SeqSeq& Result, const RangeSrc& Input, const RangeDelimit& delimit, bool token_compress = false);
	template<class SeqSeq, class RangeSrc> inline 
	SeqSeq& strtok( SeqSeq& Result, const RangeSrc& Input, 
	typename i3::range_value<RangeSrc>::type delimit_c, bool token_compress = false);

	template<class DestRange, class SrcRange, class DelimitRange> inline
	i3::vector<DestRange>& strtok_fast( i3::vector<DestRange>& Result, const SrcRange& Input, const DelimitRange& delimit, bool token_compress = false);
	template<class DestRange, class SrcRange> inline 
	i3::vector<DestRange>& strtok_fast( i3::vector<DestRange>& Result, const SrcRange& Input, 
	typename i3::range_value<SrcRange>::type delimit_c, bool token_compress = false);

	template<class Range> inline 
	long strtol(const Range& rng, typename i3::range_const_iterator<Range>::type& out_it, int base);
	template<class Range> inline 
	long strtol(const Range& rng, int base);
	template<class Range> inline 
	long strtol(const Range& rng);

	template<class DestIt, class SrcRng> inline typename i3::enable_if_iterator<DestIt>::type 
	substr(DestIt dest, const SrcRng& src, size_t pos, size_t count = 0);
	template<class DestSeq, class SrcRng> inline typename i3::disable_if_iterator<DestSeq>::type 
	substr(DestSeq& dest, const SrcRng& src, size_t pos, size_t count = 0);



	
}

#include "../itl/undef_minmax_end.h"
