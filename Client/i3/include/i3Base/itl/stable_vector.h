#pragma once

// 원 구현은 http://bannalia.blogspot.kr/2008/09/introducing-stablevector.html 
// Joaquin M Lopez Munoz라는 사람의 것으로 boost 1.49버전에 변경버전도 존재함..

// iterator 무효화와 참조데이터 변경이 없는 노드기반 배열임...
// vector보다는 확실이 느리기 때문에, vector 대용으로 쓰면 안된다..

// deque와의 구현내용 비교 : deque는 노드기반과 유사한 2중배열인데, 처음과 끝부분을 제외하면 iterator무효화가 존재함..
// list와의 구현 내용 비교 : list쪽은 노드기반이지만, 인덱스를 통한 억세스는 되지 않는다..

//
// 빈번한 삽입,삭제 이후 순회할때 list와 퍼포먼스 비교를 하는게 가장 좋을것임.. 
// 

//
// 퍼포먼스 테스트가 요구됨... (vector/deque/list)
//

#include "stable_vector_fwd.h"
#include "vector.h"

namespace i3
{
	namespace detail_stable_vector
	{
	
		template<class T>
		struct node 
		{
			node(node** up, const T& val) : up(up), data(val) {}
			node**								up;
			T									data;		
		};
		
		template<class T>		// T 내부 const 유무를 사용..
		class iterator
		{
		
		public:
			typedef typename i3::remove_const<T>::type			value_type;
			typedef node<value_type>							node_type;

			typedef i3::random_access_iterator_tag				iterator_category;
		
			typedef std::ptrdiff_t								differnece_type;

			static const bool is_const_iterator = i3::is_const<T>::value;
		
			typedef typename mpl::eval_if_c<is_const_iterator, 
											i3::add_pointer<const value_type>,
											i3::add_pointer<value_type> >::type		pointer;

			typedef typename i3::add_pointer<const value_type>::type				const_pointer;
			typedef typename mpl::eval_if_c<is_const_iterator,
											i3::add_reference<const value_type>,
											i3::add_reference<value_type> >::type	reference;
			typedef typename i3::add_reference<const value_type>::type				const_reference;

			iterator() {}
			explicit iterator(node_type* p) : m_node(p) {}
			iterator(const iterator<value_type>& rhs) :	m_node(rhs.get_node()) {}

			node_type*		get_node() const {  return m_node; }

			reference operator*() const { return m_node->data; }
			pointer	operator->() const { return &m_node->data; }
			iterator& operator++() {  m_node = *(m_node->up+1); return *this; }
			iterator operator++(int) { iterator tmp(*this); ++*this;  return tmp; }
			iterator& operator--() { m_node = *(m_node->up-1); return *this; }
			iterator operator--(int) { iterator tmp(*this); --*this; return tmp; }
			
			reference operator[](differnece_type diff) const { return m_node->up[diff]->data; }
			
			iterator& operator+=(differnece_type diff) {  m_node = m_node->up[diff]; return *this; }

			friend iterator operator+(const iterator& lhs, differnece_type diff) { iterator tmp(lhs); return tmp += diff; }
			friend iterator operator+(differnece_type diff, const iterator& rhs) { iterator tmp(rhs); return tmp += diff; }
			
			iterator& operator-=(differnece_type diff) { return *this += -diff;  }
			friend iterator operator-(const iterator& lhs, differnece_type diff) { iterator tmp(lhs); return tmp -= diff; }
			friend iterator operator-(const iterator& lhs, const iterator& rhs) { return lhs.m_node->up - rhs.m_node->up; }

			friend bool operator==(const iterator& l, const iterator& r) { return l.m_node == r.m_node; }
			friend bool operator!=(const iterator& l, const iterator& r) { return l.m_node != r.m_node; }
			
			friend bool operator<( const iterator& l, const iterator& r) { return l.m_node->up < r.m_node->up }
			friend bool operator<=( const iterator& l, const iterator& r) { return l.m_node->up <= r.m_node->up }
			friend bool operator>( const iterator& l, const iterator& r) { return l.m_node->up > r.m_node->up }
			friend bool operator>=( const iterator& l, const iterator& r) { return l.m_node->up >= r.m_node->up }
			
		private:
			node_type*		m_node;
		};
	
	}

	template<class T, class Alloc>
	class stable_vector : 
		private enable_ebo< typename rebind_wrap<Alloc, detail_stable_vector::node<T> >::type >
	{
		typedef detail_stable_vector::node<T>				node_type;
		typedef typename rebind_wrap<Alloc, node_type>::type		node_allocator_type;

		typedef i3::vector<node_type*, node_allocator_type > node_vector;
		typedef stable_vector<T, Alloc>						this_type;
	
		typedef enable_ebo< node_allocator_type >					allocator_base;
		
		typedef typename node_vector::iterator						node_iterator;
		typedef typename node_vector::const_iterator				const_node_iterator;

	public:

		typedef detail_stable_vector::iterator<T>			iterator;
		typedef detail_stable_vector::iterator<const T>		const_iterator;
		
		typedef typename iterator::reference				reference;
		typedef typename iterator::const_reference			const_reference;
		typedef typename Alloc::size_type					size_type;
		typedef typename iterator::differnece_type			difference_type;
		typedef T											value_type;
		typedef Alloc										allocator_type;
		typedef typename iterator::pointer					pointer;
		typedef typename iterator::const_pointer			const_pointer;
		typedef i3::reverse_iterator<iterator>				reverse_iterator;
		typedef i3::reverse_iterator<const_iterator>		const_reverse_iterator;

		this_type() : m_node_vector(size_type(1), nullptr) { create_end_node(); }
		explicit this_type(const allocator_type& al) : allocator_base(al), 
			m_node_vector(size_type(1), nullptr, al) {	create_end_node();	}
		explicit this_type(size_type n) { construct_n( n, T());  }
		this_type(size_type n, const T& val) { construct_n(n, val); }
		this_type(size_type n, const T& val, const allocator_type& al) : 
			allocator_base(al), m_node_vector(al) { construct_n(n, val); }

		this_type(const this_type& rhs) : allocator_base(rhs), 
			m_node_vector(rhs.m_node_vector.size(), nullptr, allocator_base::get())
		{
			const size_type n = m_node_vector.size()-1;
			for (size_type i = 0 ; i < n ; ++i )
				m_node_vector[i] = create_node(&m_node_vector[i], rhs[i]);
			create_end_node();
		}
		
		template<class It>
		this_type(It f, It l) {  construct(f, l, typename iterator_traits<It>::iterator_category() ); }
		template<class It>
		this_type(It f, It l, const allocator_type& al) : allocator_base(al), m_node_vector(al)
		{
			construct(f, l, typename iterator_traits<It>::iterator_category() );
		}
		
		~stable_vector()
		{
			const size_type n = m_node_vector.size()-1;
			for (size_type i = 0 ; i < n ; ++i )
				destroy_node( m_node_vector[i] );
			destroy_end_node();
		}

		this_type& operator=(const this_type& rhs)
		{
			if (this == &rhs) return *this;
			this_type(rhs).swap(*this);
			return *this;
		}
		
		template<class It>
		void assign(It f, It l) { assign_impl(f, l, typename iterator_traits<It>::iterator_category()); }
		
		void assign(size_type count, const T& val) { assign_n(count, val); }

		allocator_type	get_allocator() const { return allocator_base::get(); }
		
		iterator begin() { return iterator(m_node_vector.front()); }
		const_iterator begin() const { return const_iterator(m_node_vector.front()); }
		iterator end() { return iterator(m_node_vector.back()); }
		const_iterator end() const { return const_iterator(m_node_vector.back()); }

		reverse_iterator		rbegin() {	return reverse_iterator(end()); }
		const_reverse_iterator	rbegin() const {	return const_reverse_iterator(end()); }
		reverse_iterator		rend()	{	return reverse_iterator(begin());	}
		const_reverse_iterator	rend() const {	return const_reverse_iterator(begin());	}		

		const_iterator cbegin() const { return begin(); }
		const_iterator cend() const { return end(); }
		const_reverse_iterator crbegin() const { return rbegin(); }
		const_reverse_iterator crend() const { return rend(); }

		size_type size() const { return m_node_vector.size()-1; }
		size_type max_size() const { return m_node_vector.max_size()-1; }
		size_type capacity() const { return m_node_vector.capacity()-1; }
		bool empty() const { return m_node_vector.size() == 1; }

		void resize(size_type newSize) { resize(newSize, T()); }
		void resize(size_type newSize, const T& val)
		{
			if ( size() < newSize)
				insert_n(end(), newSize - size(), val);
			else if ( newSize < size() )
				erase(begin() + newSize, end());
		}
		
		void reserve(size_type count)
		{
			if ( count <= capacity() ) return;
			m_node_vector.reserve(count+1);
			align_nodes(m_node_vector.begin(), m_node_vector.end());
		}
		
		const_reference at(size_type pos) const 
		{
#ifdef _DEBUG
			if (size() <= pos)
				throw std::out_of_range("i3::stable_vector subscript out of range");
#endif
			return m_node_vector[pos]->data;
		}
		
		reference at(size_type pos)
		{
#ifdef _DEBUG
			if (size() <= pos)
				throw std::out_of_range("i3::stable_vector subscript out of range");
#endif
			return m_node_vector[pos]->data;
		}
		
		const_reference operator[](size_type pos) const { return m_node_vector[pos]->data; }
		reference operator[](size_type pos) { return m_node_vector[pos]->data; }

		reference front() { return m_node_vector.front()->data; }
		const_reference front() const { return m_node_vector.front()->data; }
		reference back() { return  (*(m_node_vector.end()-2))->data;  }
		const_reference back() const { return  (*(m_node_vector.end()-2))->data; }

		void push_back(const T& val) {	insert_1(size(), val);	}
		void pop_back() { if (!empty() )	erase(end()-1);	}
		
		iterator insert(const_iterator pos, const T& val)
		{
			size_type off = pos - begin();
			insert_1(off, val);
			return begin() + off;
		}
		
		void insert(const_iterator where, size_type count, const T& val) { insert_n(where, count, val); }
		
		template<class It>
		void insert(const_iterator where, It f, It l)
		{
			insert_impl(where, f, l, typename iterator_traits<It>::iterator_category() );
		}
		
		iterator erase(const_iterator pos)
		{
			difference_type diff = pos - begin();
			node_iterator it_node = m_node_vector.begin() + diff;
			destroy_node(*it_node);
			m_node_vector.erase(it_node);
			align_nodes(m_node_vector.begin() + diff, m_node_vector.end());
			return begin()+diff;
		}
		
		iterator erase(const_iterator f, const_iterator l)
		{
			difference_type diff1 = f - begin();
			difference_type diff2 = l - begin();
			node_iterator it_node_f = m_node_vector.begin() + diff1;
			node_iterator it_node_l = m_node_vector.begin() + diff2;
			
			for (node_iterator it_node = it_node_f ; it_node != it_node_l ; ++it_node )
				destroy_node(*it_node);
			m_node_vector.erase(it_node_f, it_node_l);

			align_nodes(m_node_vector.begin() + diff1, m_node_vector.end());
			return begin() + diff1;
		}

		void clear() { erase_begin_end(); }

		void swap(this_type& rhs)
		{
			if (this->allocator_base::get() == rhs.allocator_base::get())
			{
				i3::swap(m_node_vector, rhs.m_node_vector);
			}
			else
			{
				this_type tmp = *this;
				*this = rhs;
				rhs = tmp;
			}
		}

	private:
		
		node_type*	create_node(node_type** up, const T& val)
		{
			return new ( allocator_base::get().allocate(1) ) node_type(up, val);
		}
		
		void		destroy_node(node_type* n)
		{
			allocator_base::get().destroy(n);
			allocator_base::get().deallocate(n, 1);
		}

		void create_end_node() 
		{ 
			node_type* p = allocator_base::get().allocate(1);  
			m_node_vector.back() = p;
			p->up = &m_node_vector.back();
		}
		
		void destroy_end_node()
		{
			allocator_base::get().deallocate( m_node_vector.back(), 1);
		}
		
		void construct_n(size_type n, const T& val)
		{
			m_node_vector.assign(n+1, nullptr);
			for (size_type i = 0 ; i < n ; ++i )
				m_node_vector[i] = create_node(&m_node_vector[i], val);
			create_end_node();
		}
		
		template<class It>
		void construct(It count, It val, integral_iterator_tag) { construct_n(size_type(count), T(val)); }

		template<class It>
		void construct(It f, It l, input_iterator_tag)
		{
			for ( ; f != l ; ++f)
			{
				m_node_vector.push_back(nullptr);
				m_node_vector.back() = create_node(0, *f);
			}
			m_node_vector.push_back(nullptr);
			create_end_node();
			align_nodes( m_node_vector.begin(), m_node_vector.end());
		}

		template<class It>
		void construct(It f, It l, forward_iterator_tag)
		{
			const size_type n = i3::distance(f, l);
			m_node_vector.assign(n+1, nullptr);
			for (size_type i = 0 ; i < n ; ++i, ++f)
				m_node_vector[i] = create_node(&m_node_vector[i], *f);

			create_end_node();
		}
		
		static void align_nodes( node_iterator f, node_iterator l)
		{
			for ( ; f != l ; ++f ) (*f)->up = &*f;
		}
		
		void erase_begin_end()
		{
			
			const size_type n = m_node_vector.size() - 1;
			for (size_type i = 0 ; i < n ; ++i )
				destroy_node(m_node_vector[i]);

			m_node_vector.erase( m_node_vector.begin(), m_node_vector.begin() + n);
			
			node_type*& e = m_node_vector.front();
			e->up = &e;
		}

		template<class It>
		void assign_impl(It count, It val, integral_iterator_tag)
		{
			assign_n((size_type)count, (T)val);
		}
		
		template<class It>
		void assign_impl(It f, It l, input_iterator_tag)
		{
			erase_begin_end();
			insert(begin(), f, l);
		}
		
		void assign_n(size_type count, const T& val)
		{
			erase_begin_end();
			insert(begin(), count, val);
		}
		
		void insert_n(const_iterator where, size_type count, const T& val)
		{
			difference_type pos = where - begin();

			if (m_node_vector.capacity() >= m_node_vector.size() + count)
			{
				m_node_vector.insert(m_node_vector.begin()+pos, count, nullptr);
				node_iterator it_node = m_node_vector.begin() + pos;
				
				for (size_type i = 0; i < count ; ++i, ++it_node)
				{
					*it_node = create_node(&*it_node, val);
				}

				align_nodes(it_node, m_node_vector.end());		// 뒷부분만..
			}
			else
			{
				m_node_vector.insert(m_node_vector.begin()+pos, count, nullptr);
				node_iterator it_node = m_node_vector.begin() + pos;
				for (size_type i = 0; i < count ; ++i, ++it_node)
				{
					*it_node = create_node(&*it_node, val);
				}
				
				align_nodes(m_node_vector.begin(), m_node_vector.begin() + pos);	// 앞부분..
				align_nodes(it_node, m_node_vector.end());							// 뒷부분..
			}
		}

		void insert_1(size_type pos, const T& val)
		{
			node_iterator it_node;
			if ( m_node_vector.size() < m_node_vector.capacity() )
			{
				it_node = m_node_vector.insert(m_node_vector.begin()+pos, nullptr);
				*it_node = create_node(&*it_node, val);
				align_nodes(it_node+1, m_node_vector.end());
			}
			else
			{
				it_node = m_node_vector.insert(m_node_vector.begin()+pos, nullptr);
				*it_node = create_node(&*it_node, val);
				align_nodes(m_node_vector.begin(), m_node_vector.end());
			}
		}

		template<class It>
		void insert_impl(const_iterator where, It f, It l, integral_iterator_tag)
		{
			insert_n(where, (size_type) f, (T) l);
		}
		template<class It>
		void insert_impl(const_iterator where, It f, It l, input_iterator_tag)
		{
			difference_type pos = where - begin();
			size_type       capa = m_node_vector.capacity();

			size_type i = 0;

			for (; f != l ; ++f, ++i)
			{
				node_iterator it_node = m_node_vector.insert(m_node_vector.begin()+pos+i, nullptr);
				*it_node = create_node(&*it_node, *f);
			}
			
			if (capa == m_node_vector.capacity())
				align_nodes(m_node_vector.begin()+pos+i, m_node_vector.end());
			else		// capa변경은 루프중 재할당을 의미 --> 재할당 이전에 들어간 up값들은 모두 깨진 상태.
				align_nodes(m_node_vector.begin(), m_node_vector.end());
		}
		
		template<class It>
		void insert_impl(const_iterator where, It f, It l, forward_iterator_tag)
		{
			size_type dist = (size_type)i3::distance(f, l);
			difference_type pos = where - begin();
			
			if (m_node_vector.capacity() >= m_node_vector.size() + dist )
			{
				m_node_vector.insert(m_node_vector.begin()+pos, dist, nullptr);
				node_iterator it_node = m_node_vector.begin() + pos;
				for ( ; f != l ; ++f, ++it_node)
				{
					*it_node = create_node(&*it_node, *f);
				}
				align_nodes( it_node, m_node_vector.end());							// 삽입 뒷부분만..
			}
			else	// 내부 재할당..
			{
				m_node_vector.insert(m_node_vector.begin()+pos, dist, nullptr);
				node_iterator it_node = m_node_vector.begin() + pos;
				for ( ; f != l ; ++f, ++it_node)
				{
					*it_node = create_node(&*it_node, *f);
				}
				
				align_nodes(m_node_vector.begin(), m_node_vector.begin() + pos);	// 삽입 앞부분.
				align_nodes(it_node, m_node_vector.end());							// 삽입 뒷부분.
			}
		}


		node_vector			m_node_vector;		// 12바이트..
	};

	template<class T, class Alloc> inline
	bool operator==(const stable_vector<T, Alloc>& Left, const stable_vector<T, Alloc>& Right)
	{	// test for vector equality
		return (Left.size() == Right.size() && i3::equal(Left.begin(), Left.end(), Right.begin()));
	}
	template<class T, class Alloc> inline
	bool operator<(const stable_vector<T, Alloc>& Left, const stable_vector<T, Alloc>& Right)
	{	// test if Left < Right for vectors
		return (i3::lexicographical_compare(Left.begin(), Left.end(), Right.begin(), Right.end()));
	}
	template<class T, class Alloc> inline
	bool operator!=(const stable_vector<T, Alloc>& Left, const stable_vector<T, Alloc>& Right)
	{	// test for vector inequality
		return (!(Left == Right));
	}
	template<class T, class Alloc> inline
	bool operator>(const stable_vector<T, Alloc>& Left, const stable_vector<T, Alloc>& Right)
	{	// test if Left > Right for vectors
		return (Right < Left);
	}

	template<class T,	class Alloc> inline
	bool operator<=(const stable_vector<T, Alloc>& Left, const stable_vector<T, Alloc>& Right)
	{	// test if Left <= Right for vectors
		return (!(Right < Left));
	}

	template<class T, class Alloc> inline
	bool operator>=(const stable_vector<T, Alloc>& Left, const stable_vector<T, Alloc>& Right)
	{	// test if Left >= Right for vectors
		return (!(Left < Right));
	}

	template<class T, class Alloc> inline
	void swap(stable_vector<T, Alloc>& Left, stable_vector<T, Alloc>& Right)
	{	// swap Left and Right vectors
		Left.swap(Right);
	}


}