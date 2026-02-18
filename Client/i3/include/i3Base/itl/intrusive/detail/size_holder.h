#pragma once

namespace i3
{
	namespace intrusive
	{
		template<bool ConstantSize, class SizeType, class Tag = void>
		struct size_holder
		{
			static const bool		constant_time_size = ConstantSize;
			typedef SizeType		size_type;

			size_type	get_size() const { return m_size; }
			void		set_size(size_type s) { m_size = s; }

			void		decrement() { --m_size; }
			void		increment() { ++m_size; }
			void		increase(size_type n) { m_size += n; }
			void		decrease(size_type n) { m_size -= n; }

			size_type m_size;
		};

		template<class SizeType, class Tag>
		struct size_holder<false, SizeType, Tag>
		{
			static const bool		constant_time_size = false;
			typedef SizeType		size_type;

			size_type	get_size() const { return 0; }
			void		set_size(size_type s) {}

			void		decrement() {}
			void		increment() {}
			void		increase(size_type n) {}
			void		decrease(size_type n) {}
		};

	}
}
