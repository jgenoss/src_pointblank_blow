

// 아래는 구현..
#include "../algorithm/find.h"
#include "../algorithm/transform.h"
#include "../algorithm/remove.h"

namespace i3
{
	namespace vector_util 
	{

		template<class T, class A, class U> inline
			void	set_value_force(i3::vector<T, A>& inoutCtn, size_t idx, const U& value)
		{
			const size_t num = inoutCtn.size();
			if ( idx >= num )
			{
				// 아래 함수가 resize()보다 우월하다...
				inoutCtn.insert(inoutCtn.end(), idx - num , U() );
				inoutCtn.push_back( value );
			}
			else
			{
				inoutCtn[idx] = value;
			}
		}

		template<class T, class A, class U, class V> inline
			void	set_value_force(i3::vector<T, A>& inoutCtn, size_t idx, const U& value, const V& null_value)
		{
			const size_t num = inoutCtn.size();
			if ( idx >= num )
			{
				// 아래 함수가 resize()보다 우월하다...
				inoutCtn.insert(inoutCtn.end(), idx - num , null_value );
				inoutCtn.push_back( value );
			}
			else
			{
				inoutCtn[idx] = value;
			}
		}

		template<class T, class A> inline
			bool	is_value_valid(const i3::vector<T, A>& Ctn, size_t idx )
		{
			if ( idx >= Ctn.size() ) 
				return false;
			return Ctn[idx] != T() ;
		}

		template<class T, class A, class U> inline
			bool	is_value_valid(const i3::vector<T, A>& Ctn, size_t idx , const U& nullValue )
		{
			if ( idx >= Ctn.size() ) 
				return false;
			return Ctn[idx] != nullValue ;
		}

		template<class T, class A> inline
		void	remove_duplicates(i3::vector<T, A>& inout_ctn)
		{
			// 아래 구현은 설명이 필요
			// erase+remove조합에 의해 중복요소를 제거하는데, it 이후의 것들을 대상으로 제거가 끝부분에 이뤄지기 때문에, 그외의 반복자들은 무효화되지 않음..
			// 다만, end()값은 꾸준이 변경되기 때문에 상수화할수 없다...
			for (i3::vector<T, A>::iterator it = inout_ctn.begin(); it != inout_ctn.end() ; ++it  )
			{
				i3::vector<T, A>::iterator in = it;	++in;
				inout_ctn.erase(i3::remove(in, inout_ctn.end(), *it), inout_ctn.end());
			}
		}

		template<class T, class A, class U> inline
		typename i3::vector<T, A>::size_type	index_of( const i3::vector<T, A>& ctn, const U& obj)
		{
			i3::vector<T, A>::const_iterator it =	i3::find(ctn.begin(), ctn.end(), obj);
			return static_cast<typename i3::vector<T, A>::size_type>(i3::distance(ctn.begin(), it));
		}

		template<class T, class A, class U> inline
		int										int_index_of( const i3::vector<T, A>& ctn, const U& obj)		// -1을 사용..
		{
			typename i3::vector<T, A>::size_type sidx = index_of(ctn, obj);
			return (sidx != ctn.size() ) ? static_cast<int>(sidx) : -1;
		}

		template<class T, class A, class U> inline
		typename i3::vector<T, A>::size_type	push_back_once( i3::vector<T, A>& ctn, const U& obj)
		{
			i3::vector<T, A>::iterator it =	i3::find(ctn.begin(), ctn.end(), obj);
			if (it != ctn.end()) return static_cast<typename i3::vector<T, A>::size_type>(i3::distance(ctn.begin(), it));
			ctn.push_back(obj);
			return ctn.size() - 1;
		}

		template<class T, class A, class U> inline
		bool	remove( i3::vector<T, A>& ctn, const U& obj)
		{
			i3::vector<T, A>::const_iterator it =	i3::find(ctn.begin(), ctn.end(), obj);
			if (it == ctn.end() ) return false;
			ctn.erase( it );
			return true;
		}

		template<class T, class A> inline
		void									erase_index( i3::vector<T, A>& ctn, typename i3::vector<T, A>::size_type idx)
		{
			ctn.erase(ctn.begin() + idx );
		}

		template<class T, class A>	inline
		void									clear_capacity( i3::vector<T, A>& ctn)
		{
			i3::vector<T, A>().swap(ctn); 
		}

		template<class T1, class A1, class T2,  class A2>  inline
		void	copy_vector( const i3::vector<T1, A1>& src, i3::vector<T2, A2>& dest)
		{
			i3::vector<T2, A2>(src.size()).swap(dest);				// resize()의 경우 캐퍼가 더 붙기 때문에 이쪽으로 처리..
			i3::copy(src.begin(), src.end(), dest.begin());
		}

		template<class T1, class A1, class T2, class A2> inline
		void	copy_ptr_vector_force( const i3::vector<T1*, A1>& src, i3::vector<T2*, A2>& dest)
		{
			const size_t src_size = src.size();
			i3::vector<T2*, A2>(src_size).swap(dest);				// resize()의 경우 캐퍼가 더 붙기 때문에 이쪽으로 처리..
			if (src_size == 0) return;
			
			intptr_t	offset = intptr_t(reinterpret_cast<T2*>(1)) - intptr_t(static_cast<T1*>( reinterpret_cast<T2*>(1)));
			// 옵셋은 다중상속때의 첫번째 베이스인지 아닌지를 구별해주는 역할...이부분에서는 static_cast가 컴파일타임상수를 내주지 않는다..
			if (offset == 0)
				::memcpy(&dest[0], &src[0], sizeof(T2*) * src_size );	// 내부에서 memcpy로 처리되도록 유도...
			else
			{
				struct fn {		T2*		operator()(T1* p) const { return static_cast<T2*>(p); }	};
				i3::transform(src.begin(), src.end(), dest.begin(), fn() );
			}
		}

	}
}
