#pragma once

#include "../../../pool/create_by_common_pool.h"
#include "../../../itl/svector.h"
#include "../../../itl/mpl/mpl_size_t.h"

namespace i3
{
	// 문자열 보관 용도로 작성..
	
	
	template<class Ch, class mplSize>
	class stack_string_storage : private i3::noncopyable
	{
	public:
		typedef Ch							value_type;

		static const std::size_t min_size = mplSize::value;
		stack_string_storage() : m_curr_pos(m_stack_data), m_curr_end(m_stack_data + min_size) {}
		
		~stack_string_storage()
		{
			const std::size_t num_vec = m_vec_data.size();
			for (std::size_t i = 0; i < num_vec ; ++i) 
				i3::delete_array_by_common_pool(m_vec_data[num_vec - 1 - i]); 
		}

		void	append(const Ch* str, std::size_t size)
		{
				if (m_curr_pos + size > m_curr_end)
					change_char_array(size);
				m_curr_pos = i3::copy( str, str+size, m_curr_pos);
		}
		
		void	append(std::size_t num, Ch c)
		{
			if (m_curr_pos + num > m_curr_end)
				change_char_array(num);

			i3::fill_n(m_curr_pos, num, c);
			m_curr_pos += num;
		}
		
		template<class SizeT, class Ch2> typename i3::enable_if< is_convertible<SizeT, std::size_t>,
		void>::type	append(SizeT num, Ch2 c)
		{
			append(std::size_t(num), Ch(c));
		}
		
		template<class It> typename i3::disable_if< is_convertible<It, std::size_t>,
		void>::type	append(It beg, It end)
		{
			const std::ptrdiff_t dist = i3::distance(beg, end);
			if (m_curr_pos + dist > m_curr_end)
				change_char_array(dist);

			m_curr_pos = i3::copy(beg, end, m_curr_pos);
		}
		
		void append(Ch c)
		{
			if ( m_curr_pos + 1 > m_curr_end)
				change_char_array(1);
			*m_curr_pos = c;
			++m_curr_pos;
		}
		
		const Ch*			prev_pos(std::size_t prev_size) const { return m_curr_pos - prev_size; }
		const Ch*			curr_pos() const { return m_curr_pos; }

	private:

		void				change_char_array(std::size_t req_size)
		{
			if (req_size < (min_size/2) ) req_size = min_size;
			
			Ch* data = i3::create_array_by_common_pool(i3::identity<Ch>(), req_size);
			m_vec_data.push_back(data);
			m_curr_pos = data;
			m_curr_end = data + req_size;
		}

		Ch					m_stack_data[min_size];
		i3::svector<Ch*>	m_vec_data;
		Ch*					m_curr_pos;
		Ch*					m_curr_end;
	};
	
	
}