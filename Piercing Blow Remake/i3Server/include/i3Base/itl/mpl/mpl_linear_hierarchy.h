#pragma once


namespace i3
{
	namespace mpl
	{
		// 높은 인덱스 쪽이 base가 됨..(시퀀스의 첫요소가 최외각)
		template<class Seq, template<class Elem, class Base> class Unit, class Root = empty_base>
		struct linear_hierarchy : reverse_fold<Seq, Root, Unit<_2, _1> > {};
		
		// 낮은 인덱스 쪽이 base가 됨..(시퀀스의 첫요소가 Root다음에 위치)
		template<class Seq, template<class Elem, class Base> class Unit, class Root = empty_base>
		struct reverse_linear_hierarchy : fold<Seq, Root, Unit<_2, _1> > {};
	}
}

