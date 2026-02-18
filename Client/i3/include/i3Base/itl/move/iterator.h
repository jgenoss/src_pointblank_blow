#pragma once

#include "../iterator/iterator_traits.h"
#include "move.h"

namespace i3
{
	template<class It>
	class move_iterator
	{
	public:
		typedef It											iterator_type;
		typedef typename i3::iterator_value<It>::type		value_type;
		
		typedef typename mpl::if_<	i3::has_move_emulation_enabled<value_type>, 
			i3::rv<value_type>&, value_type&>::type			reference;

		typedef It											pointer;			// ?
		typedef typename i3::iterator_difference<It>::type	difference_type;
		typedef typename i3::iterator_category<It>::type	iterator_category;
		
		move_iterator() {}
		explicit move_iterator(It it) : m_it(it) {}
		template<class U>
		move_iterator(const move_iterator<U>& u) : m_it(u.base()) {}
		iterator_type base() const { return m_it; }
		reference operator*() const { return *m_it;	}
		pointer operator->() const { return m_it; }
		
		move_iterator& operator++() { ++m_it; return *this; }
		move_iterator operator++(int) { move_iterator<iterator_type> tmp(*this); ++(*this); return tmp; }
		move_iterator& operator--() { --m_it; return *this; }
		move_iterator operator--(int) { move_iterator<iterator_type> tmp(*this); --(*this); return tmp; }
		
		move_iterator operator+(difference_type n) const { return move_iterator<iterator_type>(m_it + n); }
		move_iterator& operator+=(difference_type n) { m_it += n; return *this; }
		move_iterator operator-(difference_type n) const { return move_iterator<iterator_type>(m_it - n); }
		move_iterator& operator-=(difference_type n) { m_it -= n; return *this; }

		reference operator[](difference_type n) const { return m_it[n]; }

		friend bool operator==(const move_iterator& x, const move_iterator& y)
		{  return x.base() == y.base();  }

		friend bool operator!=(const move_iterator& x, const move_iterator& y)
		{  return x.base() != y.base();  }

		friend bool operator< (const move_iterator& x, const move_iterator& y)
		{  return x.base() < y.base();   }

		friend bool operator<=(const move_iterator& x, const move_iterator& y)
		{  return x.base() <= y.base();  }

		friend bool operator> (const move_iterator& x, const move_iterator& y)
		{  return x.base() > y.base();  }

		friend bool operator>=(const move_iterator& x, const move_iterator& y)
		{  return x.base() >= y.base();  }

		friend difference_type operator-(const move_iterator& x, const move_iterator& y)
		{  return x.base() - y.base();   }

		friend move_iterator operator+(difference_type n, const move_iterator& x)
		{  return move_iterator(x.base() + n);   }
	private:
		It	m_it;
	};

	template<class T> struct is_move_iterator : false_type {};
	template<class It> struct is_move_iterator< i3::move_iterator<It> > : true_type {};

	template<class It> inline
	move_iterator<It> make_move_iterator(const It& it) { return move_iterator<It>(it); }

	template<class Ctn>
	class back_move_insert_iterator : public i3::iterator<i3::output_iterator_tag, void, void, void, void>
	{
	public:
		typedef Ctn								container_type;
		typedef typename Ctn::value_type		value_type;
		typedef typename Ctn::reference			reference;

		explicit back_move_insert_iterator(Ctn& x) : m_ctn(&x) { }

		back_move_insert_iterator& operator=(reference x) { m_ctn->push_back(i3::move(x)); return *this; }
		back_move_insert_iterator& operator=(rv_ref(value_type) x) {  reference rx = x; return this->operator=(rx);  }

		back_move_insert_iterator& operator*()     { return *this; }
		back_move_insert_iterator& operator++()    { return *this; }
		back_move_insert_iterator& operator++(int) { return *this; }
	private:
		Ctn*		m_ctn;
	};

	template<class Ctn> inline
	back_move_insert_iterator<Ctn> back_move_inserter(Ctn& c) { return back_move_insert_iterator<Ctn>(c); }

	template<class Ctn>
	class front_move_insert_iterator : public i3::iterator<i3::output_iterator_tag, void, void, void, void>
	{
	public:
		typedef Ctn								container_type;
		typedef typename Ctn::value_type		value_type;
		typedef typename Ctn::reference			reference;

		explicit front_move_insert_iterator(Ctn& x) : m_ctn(&x) { }

		front_move_insert_iterator& operator=(reference x) { m_ctn->push_front(i3::move(x)); return *this; }
		front_move_insert_iterator& operator=(rv_ref(value_type) x) {  reference rx = x; return this->operator=(rx);  }

		front_move_insert_iterator& operator*()     { return *this; }
		front_move_insert_iterator& operator++()    { return *this; }
		front_move_insert_iterator& operator++(int) { return *this; }
	private:
		Ctn*		m_ctn;
	};

	template<class Ctn> inline
	front_move_insert_iterator<Ctn> front_move_inserter(Ctn& c) { return front_move_insert_iterator<Ctn>(c); }

	template<class Ctn>
	class move_insert_iterator : public i3::iterator<i3::output_iterator_tag, void, void, void, void>
	{
	public:
		typedef Ctn								container_type;
		typedef typename Ctn::value_type		value_type;
		typedef typename Ctn::reference			reference;

		explicit move_insert_iterator(Ctn& x, typename Ctn::iterator pos) : m_ctn(&x), m_pos(pos) { }

		move_insert_iterator& operator=(reference x) { m_pos = m_ctn->insert(m_pos, i3::move(x)); ++m_pos;	return *this; }
		move_insert_iterator& operator=(rv_ref(value_type) x) {  reference rx = x; return this->operator=(rx);  }

		move_insert_iterator& operator*()     { return *this; }
		move_insert_iterator& operator++()    { return *this; }
		move_insert_iterator& operator++(int) { return *this; }
	private:
		Ctn*						m_ctn;
		typename Ctn::iterator		m_pos;
	};

	template<class Ctn> inline
	move_insert_iterator<Ctn> move_inserter(Ctn& c, typename Ctn::iterator pos) { return move_insert_iterator<Ctn>(c, pos); }


}