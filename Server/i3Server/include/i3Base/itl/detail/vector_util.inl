

// 아래는 구현..
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
				if ( idx > num )
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
				if ( idx > num )
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
		typename i3::vector<T, A>::difference_type	index_of( const i3::vector<T, A>& ctn, const U& obj)
		{
			i3::vector<T, A>::const_iterator it =	i3::find(ctn.begin(), ctn.end(), obj);
			return i3::distance(ctn.begin(), it);
		}

	}
}
