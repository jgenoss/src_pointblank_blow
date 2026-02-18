#pragma once

//
//   range가 모든 iterator 지원 가능 컨테이너를 대신할것으로 보임..
//
#include "../begin.h"
#include "../end.h"
#include "../detail/enable_if_iterator.h"
#include "../../utility/copy_or_swap.h"
#include "../../algorithm/transform.h"

namespace i3
{

namespace range
{
	// 이 2개는 기존 알고리즘 함수에서 크게 변형된건 없음..

	template<class SinglePassRange, class OutIt, class Fn1> inline 
	typename i3::iterator_return<OutIt>::type transform(const SinglePassRange& r, OutIt dest, Fn1 fn)
	{
		return i3::transform(i3::begin(r), i3::end(r), dest, fn);
	}
	
	template<class SinglePassRange1, class SinglePassRange2, class OutIt, class Fn2> inline
	typename i3::iterator_return<OutIt>::type transform(const SinglePassRange1& r1, 
		const SinglePassRange2& r2, OutIt dest, Fn2 fn)
	{
		return i3::transform(i3::begin(r1), i3::end(r1), i3::begin(r2), dest, fn);
	}

	//
	// 둘째 인수가 이터레이터가 아닌 경우, 컨테이너로 가정하는 함수를 추가..(2012.12.05.수빈)
	// 초기에는 assign함수가 존재하면 그것을 사용하고, 그렇지 않으면 컨테이너 생성후 대입을 썼음..
	// 대부분의 컨테이너 assign함수 구현이 그다지 좋아보이지 않아서, 객체 생성후 스왑하는 방식으로 변경..
	//
		
	template<class SinglePassRange, class OutSeq, class Fn1> inline 
	typename i3::non_iterator_return<OutSeq>::type transform(const SinglePassRange& r, OutSeq& dest, Fn1 fn)	
	{
		OutSeq temp(make_transform_iterator(i3::begin(r), fn), make_transform_iterator(i3::end(r), fn) );
		i3::copy_or_swap(dest, temp);
		return dest;
	}

	template<class SinglePassRange1, class SinglePassRange2, class OutSeq, class Fn2> inline
	typename i3::non_iterator_return<OutSeq>::type transform(const SinglePassRange1& r1, 
		const SinglePassRange2& r2, OutSeq& dest, Fn2 fn)
	{
		OutSeq temp(make_binary_transform_iterator(i3::begin(r1), i3::begin(r2), fn), 
					make_binary_transform_iterator(i3::end(r1), i3::end(r2), fn) );	
		i3::copy_or_swap(dest, temp);
		return dest;
	}

	// 컨테이너 입출력인수 하나로 통일..
	template<class SinglePassRange1, class Fn1> inline 
	void inplace_transform(const SinglePassRange1& inout, Fn1 fn)
	{
		i3::transform(i3::begin(inout), i3::end(inout), i3::begin(inout), fn);
	}
		
	// 첫번째 컨테이너에 누적시키는 버전..
	template<class SinglePassRange1, class SinglePassRange2, class Fn2> inline 
	void inplace_transform(const SinglePassRange1& inout, const SinglePassRange2& r2, Fn2 fn)
	{
		i3::transform(i3::begin(inout), i3::end(inout),	i3::begin(r2), i3::begin(inout), fn);
	}
	
	
}


}

namespace i3range = i3::range;
