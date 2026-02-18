#pragma once

namespace i3
{
	
	namespace detail
	{
		// 오버로딩이 파생에서 기본클래스방향으로 전달되는 것을 사용함..( n -> 0으로 진행되네..)
		template<bool lhs_idx_valid, bool rhs_idx_valid>
		struct tuple_compare_helper;

		template<>
		struct tuple_compare_helper<true, true>
		{
			template<int Idx, class T1, class T2> inline static
			bool eq(const T1& lhs, const T2& rhs) 
			{
				return lhs.get<Idx>() == rhs.get<Idx>() && 
					tuple_compare_helper< is_tuple_index<Idx+1, T1>::value, is_tuple_index<Idx+1, T2>::value>::eq<Idx+1>(lhs, rhs); 
			}
			template<int Idx, class T1, class T2> inline static
			bool lt(const T1& lhs, const T2& rhs)
			{
				return	lhs.get<Idx>() < rhs.get<Idx>() || 
						( !(rhs.get<Idx>() < lhs.get<Idx>()) &&
						tuple_compare_helper< is_tuple_index<Idx+1, T1>::value, is_tuple_index<Idx+1, T2>::value >::lt<Idx+1>(lhs, rhs) );
			}
		};
		
		template<>
		struct tuple_compare_helper<false, false>
		{
			template<int Idx, class T1, class T2> inline static
			bool eq(const T1& lhs, const T2& rhs) {	return true; }
			template<int Idx, class T1, class T2> inline static
			bool lt(const T1& lhs, const T2& rhs) { return false; }
			
		};
		
		//  인자가 더 긴쪽을 큰 것으로 판단시킨다..
		template<>
		struct tuple_compare_helper<true, false>
		{
			template<int Idx, class T1, class T2> inline static
			bool eq(const T1& lhs, const T2& rhs) { return false; }
			template<int Idx, class T1, class T2> inline static
			bool lt(const T1& lhs, const T2& rhs) { return false; } 
		};

		template<>
		struct tuple_compare_helper<false, true>
		{
			template<int Idx, class T1, class T2> inline static
			bool eq(const T1& lhs, const T2& rhs) { return false; }
			template<int Idx, class T1, class T2> inline static
			bool lt(const T1& lhs, const T2& rhs) { return true; } 
		};

	}
	
	template<pp_enum_params(tuple_limit, class T), pp_enum_params(tuple_limit, class U)>
	inline bool operator==(const tuple<pp_enum_params(tuple_limit, T)>& lhs, const tuple< pp_enum_params(tuple_limit, U)>& rhs)
	{
		typedef tuple<pp_enum_params(tuple_limit, T)> tuple1;
		typedef tuple<pp_enum_params(tuple_limit, U)> tuple2;
		return detail::tuple_compare_helper<is_tuple_index<0, tuple1>::value, is_tuple_index<0,tuple2>::value>::eq<0>(lhs, rhs);
	}
	
	template<pp_enum_params(tuple_limit, class T), pp_enum_params(tuple_limit, class U)>
	inline bool operator!=(const tuple<pp_enum_params(tuple_limit, T)>& lhs, const tuple<pp_enum_params(tuple_limit, U)>& rhs)
	{
		return !(lhs == rhs);
	}
	
	template<pp_enum_params(tuple_limit, class T), pp_enum_params(tuple_limit, class U)>
	inline bool operator<(const tuple<pp_enum_params(tuple_limit, T)>& lhs, const tuple<pp_enum_params(tuple_limit, U)>& rhs)
	{
		typedef tuple<pp_enum_params(tuple_limit, T)> tuple1;
		typedef tuple<pp_enum_params(tuple_limit, U)> tuple2;
		return detail::tuple_compare_helper<is_tuple_index<0, tuple1>::value, is_tuple_index<0,tuple2>::value>::lt<0>(lhs, rhs);
	}
	
	template<pp_enum_params(tuple_limit, class T), pp_enum_params(tuple_limit, class U)>
	inline bool operator>(const tuple<pp_enum_params(tuple_limit, T)>& lhs, const tuple<pp_enum_params(tuple_limit, U)>& rhs)
	{
		return rhs < lhs;
	}
	
	template<pp_enum_params(tuple_limit, class T), pp_enum_params(tuple_limit, class U)>
	inline bool operator<=(const tuple<pp_enum_params(tuple_limit, T)>& lhs, const tuple<pp_enum_params(tuple_limit, U)>& rhs)
	{
		return !(rhs < lhs);
	}
	
	template<pp_enum_params(tuple_limit, class T), pp_enum_params(tuple_limit, class U)>
	inline bool operator>=(const tuple<pp_enum_params(tuple_limit, T)>& lhs, const tuple<pp_enum_params(tuple_limit, U)>& rhs)
	{
		return !(lhs < rhs);
	}
	
}