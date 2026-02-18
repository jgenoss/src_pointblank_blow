#pragma once

#include "slist_fwd.h"
#include "../pool/pool_allocator.h"
#include "memory.h"
#include "compressed_pair.h"
#include "algorithm/equal.h"
#include "algorithm/lexicographical_compare.h"

#pragma push_macro("new")
#undef new

// 컨테이너 사이즈 최소화를 위해 사이즈 멤버변수를 제거..

namespace i3
{
	
	template<class T, class Alloc>
	class slist 
	{
	public:
		typedef slist<T, Alloc>									my_type;
		typedef typename Alloc::template rebind<T>::other		allocator_type;
		typedef typename allocator_type::size_type				size_type;
		typedef typename allocator_type::difference_type		difference_type;
		typedef typename allocator_type::pointer				pointer;
		typedef typename allocator_type::const_pointer			const_pointer;
		typedef typename allocator_type::reference				reference;
		typedef typename allocator_type::const_reference		const_reference;
		typedef typename allocator_type::value_type				value_type;
		
		#pragma warning(push)
		#pragma warning(disable:4512)

		struct node;

		struct node_head
		{
			node_head() : next(0) {}
			node_head(node* next) : next(next) {}
			node* next;
		};

		struct node : node_head
		{
			node( node* next, const T& val) : node_head(next), val(val) {}
			T val;
		};

		#pragma warning(pop)
		
		typedef typename Alloc::template rebind<node>::other	node_allocator_type;	

		class const_iterator : public i3::iterator< forward_iterator_tag, value_type, difference_type, 
			const_pointer, const_reference>
		{
		public:
			typedef const_pointer	pointer;
			typedef const_reference	reference;
			const_iterator() : ptr(0){}
			explicit const_iterator(node* pnode) : ptr(pnode) {}
			const_reference operator*() const  { return ptr->val; }
			const_pointer	operator->() const { return (&**this); }
			const_iterator& operator++() {	ptr = ptr->next;	return *this; }
			const_iterator	operator++(int)	{	const_iterator _Tmp = *this;	++*this;	return _Tmp;  }
			bool operator==(const const_iterator& Right) const 	{	return (ptr == Right.ptr);	}
			bool operator!=(const const_iterator& Right) const	{	return (!(*this == Right));	}
			node* get_node() const { return ptr; }
		private:
			node* ptr;
		};
		
		class iterator : public const_iterator
		{	
		public:
			typedef typename allocator_type::pointer			pointer;
			typedef typename allocator_type::reference			reference;

			iterator(){}
			explicit iterator(node* pnode)	: const_iterator(pnode)	{}
			reference operator*() const {	return ((reference)**(const_iterator *)this);	}
			pointer operator->() const	{	return (&**this);	}
			iterator& operator++()	{	++(*(const_iterator *)this);	return (*this);	}
			iterator operator++(int){	iterator _Tmp = *this;	++*this;	return _Tmp;	}
		};
		
		slist() {}
		explicit slist(const Alloc& Al) : m_alloc_and_head(Al) {}
		explicit slist(size_type count) { construct_n(count, T() );	}
		slist(size_type count, const T& val) { construct_n(count, val );	}
		slist(size_type count, const T& val, const Alloc& Al) : m_alloc_and_head(Al) { construct_n(count, val );	}
		
		slist(const my_type& rhs) : m_alloc_and_head(rhs.node_alloc()) { insert_after_impl(head(), rhs.begin(), rhs.end() ); }

		template<class It>
		slist(It f, It l) { insert_after_impl(head(), f, l); }
		template<class It>
		slist(It f, It l, const Alloc& Al) : m_alloc_and_head(Al) { insert_after_impl(head(), f, l); }
		
		~slist()
		{
			node* next = 0;
			for (node* pnode = head()->next ; pnode != 0 ; pnode = next )
			{
				next = pnode->next;
				node_alloc().destroy(pnode);	node_alloc().deallocate(pnode, 1);
			}
			// 헤드 쪽은 할당자를 사용하지 않았으므로 이대로 마무리한다.
		}
		
		slist&		operator=(const my_type& rhs) 
		{
			if (this != &rhs) assign(rhs.begin(), rhs.end()); 
			return *this;
		}
		template<class It>
		void assign(It First, It Last) 
		{  
			assign_impl(First, Last, typename i3::iterator_traits<It>::iterator_category() );  
		}
		
		void assign(size_type count, const T& val)
		{
			assign_impl_n(count, val);
		}
		
		iterator begin() { return iterator( head()->next ); }
		const_iterator begin() const { return const_iterator(head()->next); }
		iterator end() { return iterator(); }
		const_iterator end() const { return const_iterator(); }
		
		iterator		before_begin() { return iterator(head()); }
		const_iterator	before_begin() const { return const_iterator(head()); }
		
		size_type		size() const		// 이 리스트에서는 느리게 동작한다.
		{    
			size_type count = 0;	for ( node* n = head()->next; n != 0 ; n = n->next, ++count) {}
			return count;
		}
		
		size_type		max_size() const { return node_alloc().max_size(); }
		bool			empty() const  { return head()->next == 0; }
		allocator_type	get_allocator() const {	return node_alloc(); }	

		void			swap( slist& rhs)
		{
			if (this == &rhs) return;
			if (this->node_alloc() == rhs.node_alloc() )
			{
				i3::swap( head()->next, rhs.head()->next);
			}
			else
			{
				iterator where = begin();
				splice_after_impl( head(), rhs);
				rhs.splice(rhs.begin(), *this, where, end());
			}
		}
		
		reference			front() { return *begin(); }
		const_reference		front() const { return *begin(); }
		void				push_front(const T& val) { insert_after_impl(head(), val);  }
		void				pop_front() { erase_after_impl(head()); }
		
		iterator			previous(const_iterator pos) {	return iterator(previous_impl(head(), pos.get_node())); }
		const_iterator		previous(const_iterator pos) const { return const_iterator(previous_impl(head(), pos.get_node())); }
		
		iterator			insert_after(const_iterator before, const T& val)
		{
			return iterator( insert_after_impl(before.get_node(), val) );
		}
		void				insert_after(const_iterator before, size_type count, const T& val)
		{
			insert_after_impl_n(before.get_node(), count, val);
		}
		template<class It>
		void				insert_after(const_iterator before, It f, It l) 
		{
			insert_after_impl(before.get_node(), f, l);
		}
		
		iterator			insert(const_iterator pos, const T& val) {	return insert_after( previous(pos), val); }
		void				insert(const_iterator pos, size_type count, const T& val) { insert_after(previous(pos), count, val); }
		template<class It>
		void				insert(const_iterator pos, It f, It l) { insert_after(previous(pos), f, l); }

		iterator			erase_after(const_iterator before) {	return iterator( erase_after_impl(before.get_node()) );	}
		iterator			erase_after(const_iterator before_first, const_iterator last)
		{
			return iterator( erase_after_impl(before_first.get_node(), last.get_node()) );
		}
		
		iterator			erase(const_iterator pos) { return erase_after(previous(pos));  }
		iterator			erase(const_iterator f, const_iterator l) { return erase_after( previous(f), l); }

		void				resize(size_type new_size) { resize(new_size, T()); }
		void				resize(size_type new_size, T Val) 
		{
			node* n = head();
			for ( ; n->next != 0 && new_size > 0 ; n = n->next, --new_size) {}

			if ( n->next)
				erase_after_impl(n, 0);
			else
				insert_after_impl_n(n, new_size, Val);
		}
		
		void				clear() 
		{	
			node* next = 0;
			for (node* pnode = head()->next ; pnode != 0 ; pnode = next )
			{
				next = pnode->next;
				node_alloc().destroy(pnode);	node_alloc().deallocate(pnode, 1);
			}
			head()->next = 0;
		}
		
		void				splice_after(const_iterator before, slist& rhs)
		{
			if ( this != &rhs && !rhs.empty() )
			{
				splice_after_impl(before.get_node(), rhs);
			}
		}

		void				splice(const_iterator pos, slist& rhs)
		{
			if ( this != &rhs && !rhs.empty() )
			{
				splice_after_impl( previous_impl(head(), pos.get_node()), rhs);
			}
		}

		void				splice_after(const_iterator before_pos, slist& rhs, const_iterator before_first, const_iterator before_last)
		{
			if (before_first != before_last)
			{
				if (this->node_alloc() == rhs.node_alloc() )
					splice_after_impl(before_pos.get_node(), before_first.get_node(), before_last.get_node());
				else
				{
					if ( this != &rhs || before_pos != before_last)
					{
						const_iterator first = before_first;	++first;
						const_iterator last  = before_last;		++last;
						insert_after_impl(before_pos.get_node(), first, last);
						rhs.erase_after_impl(before_first.get_node(), last.get_node());
					}
				}
			}
		}
		
		void				splice(const_iterator pos, slist& rhs, const_iterator f, const_iterator l)
		{
			if ( f != l )
			{
				if (this->node_alloc() == rhs.node_alloc() )
					splice_after_impl( previous_impl(head(), pos.get_node() ), 
					                   previous_impl(rhs.head(), f.get_node()), 
									   previous_impl(rhs.head(), l.get_node()));
				else
				{
					if (this != &rhs || pos != l )
					{
						insert_after_impl(previous_impl(head(), pos.get_node() ), f, l);
						rhs.erase_after_impl(previous_impl(rhs.head(), f.get_node()), l.get_node());
					}
				}
			}
		}
		
		void			splice_after(const_iterator before_pos, slist& rhs, const_iterator before_target)
		{
			if ( before_target.get_node() == 0 ||
				 before_target.get_node()->next == 0) 
				 return;

			if (this->node_alloc() == rhs.node_alloc() )
				splice_after_impl(before_pos.get_node(), before_target.get_node(), before_target.get_node()->next);
			else
			{
				const_iterator first = before_target;		++first;
				const_iterator last  = first;				++last;

				if ( this != &rhs || ( before_pos != before_target && before_pos != first ) )
				{
					insert_after_impl(before_pos.get_node(), first, last);
					rhs.erase_after_impl(before_target.get_node());
				}
			}
		}

		void		splice(const_iterator pos, slist& rhs, const_iterator target)
		{
			if (target.get_node() == 0) 
				return;

			if (this->node_alloc() == rhs.node_alloc() )
				splice_after_impl(previous_impl(head(), pos.get_node()), 
								  previous_impl(rhs.head(), target.get_node()),
								  target.get_node() );
			else
			{
				const_iterator last = target;		++last;

				if (this != &rhs || ( pos != target && pos != last) )
				{
					insert_after_impl(previous_impl(head(), pos.get_node()), target, last);
					rhs.erase_after_impl(previous_impl(rhs.head(), target.get_node() ) );
				}
			}
		}
		
		void reverse()
		{	
			if (head()->next == 0)
				return;
			
			node* n = head()->next;
			node* res = n;
			n = n->next;
			res->next = 0;
			
			while (n)
			{
				node* next = n->next;
				n->next = res;
				res = n;
				n = next;
			}
			head()->next = res;
		}

		void remove(const T& val)
		{
			node* n = head();
			while (n->next)
			{
				if ( n->next->val == val)
					erase_after_impl(n);
				else
					n = n->next;
			}
		}
		
		template<class Pred1>
		void remove_if(Pred1 pr)
		{
			node* n = head();
			while (n->next)
			{
				if ( pr(n->next->val) )
					erase_after_impl(n);
				else
					n = n->next;
			}
		}

		void unique()
		{
			node* n = head()->next; 
			if (n)
			{
				while (n->next)
				{
					if ( n->val == n->next->val )
						erase_after_impl(n); 
					else
						n = n->next;
				}
			}
		}

		template<class Pred2>
		void unique(Pred2 pr)
		{
			node* n = head()->next; 
			if (n)
			{
				while (n->next)
				{
					if ( pr(n->val, n->next->val) )
						erase_after_impl(n); 
					else
						n = n->next;
				}
			}
		}

		void merge(slist& rhs)
		{
			node* n = head();

			while (n->next && rhs.head()->next)
			{
				if (rhs.head()->next->val <  n->next->val )
					splice_after_impl(n, rhs.head(), rhs.head()->next);
				n = n->next;
			}

			if (rhs.head()->next)
			{
				n->next = rhs.head()->next;
				rhs.head()->next = 0;
			}
		}

		template<class Pred2>
		void merge(slist& rhs, Pred2 pr)
		{
			node* n = head();

			while (n->next && rhs.head()->next)
			{
				if ( pr(rhs.head()->next->val, n->next->val ) )
					splice_after_impl(n, rhs.head(), rhs.head()->next);
				n = n->next;
			}

			if (rhs.head()->next)
			{
				n->next = rhs.head()->next;
				rhs.head()->next = 0;
			}
		}

		void sort()
		{
			if (head()->next && head()->next->next)
			{
				slist carry_list;
				slist bin_list[64];
				int  fill_index = 0;
				while (!empty())
				{
					splice_after_impl(carry_list.head(), head(), head()->next);

					int i = 0;
					while (i < fill_index && !bin_list[i].empty())
					{
						bin_list[i].merge(carry_list);
						carry_list.swap(bin_list[i]);
						++i;
					}
					carry_list.swap(bin_list[i]);
					if (i == fill_index)
						++fill_index;
				}

				for (int i = 1; i < fill_index; ++i)
					bin_list[i].merge(bin_list[i-1]);
				this->swap(bin_list[fill_index-1]);
			}
		}
		
		template<class Pred2>
		void sort(Pred2 pr)
		{
			if (head()->next && head()->next->next)
			{
				slist carry_list;
				slist bin_list[64];
				int  fill_index = 0;
				while (!empty())
				{
					splice_after_impl(carry_list.head(), head(), head()->next);

					int i = 0;
					while (i < fill_index && !bin_list[i].empty())
					{
						bin_list[i].merge(carry_list, pr);
						carry_list.swap(bin_list[i]);
						++i;
					}
					carry_list.swap(bin_list[i]);
					if (i == fill_index)
						++fill_index;
				}

				for (int i = 1; i < fill_index; ++i)
					bin_list[i].merge(bin_list[i-1], pr);
				this->swap(bin_list[fill_index-1]);
			}
		}


	private:
		iterator	cast_to_iterator(const_iterator where) { return iterator( where.get_node() );  }

		node*		buy_node(node* Next, const T& Val)
		{	
			return new ( node_alloc().allocate(1) ) node( Next, Val);
		}

		void	construct_n(size_type count, const T& Val) {	insert_after_impl_n(head(), count, Val);	}
		
		void	insert_after_impl_n(node* n, size_type count, const T& val)
		{
			for ( ; 0 < count ; --count)
				n = insert_after_impl(n, val);
		}
		
		node*	insert_after_impl(node* n, const T& val)
		{
			return n->next = buy_node(n->next, val);
		}
		
		template<class It>
		void	insert_after_impl(node* n, It f, It l)
		{
			insert_after_impl(n, f, l, typename i3::iterator_traits<It>::iterator_category());
		}

		template<class It>
		void	insert_after_impl(node* n, It count, It val, integral_iterator_tag)
		{
			insert_after_impl_n(n, size_type(count), T(val));
		}
		
		template<class It>
		void	insert_after_impl(node* n, It f, It l, input_iterator_tag)
		{
			for (; f != l; ++f ) 
				n = insert_after_impl(n, *f); 
		}
		
		template<class It>
		void    assign_impl(It count, It val, integral_iterator_tag)
		{
			assign_impl_n(size_type(count), T(val));
		}

		template<class It>
		void	assign_impl(It f, It l, input_iterator_tag)
		{
			node* prev = head();
			node* n    = prev->next;
			for ( ; n != 0 && f != l ; ++f, n = n->next )
			{
				n->val = *f;
				prev = n;
			}

			if ( f != l )
				insert_after_impl( prev, f, l );
			else
				erase_after_impl( prev, 0);
		}

		void assign_impl_n(size_type count, const T& Val)
		{	
			node* prev = head();
			node* n = prev->next;
			
			for ( ; n != 0 && count > 0 ; --count, n = n->next )
			{
				n->val = Val;
				prev = n;
			}

			if ( count > 0)
				insert_after_impl_n(prev, count, Val);
			else
				erase_after_impl(prev, 0);
		}
		
		node* erase_after_impl(node* before)
		{
			node* n = before->next;
			node* n_next = n->next;
			before->next = n_next;
			node_alloc().destroy(n);
			node_alloc().deallocate(n, 1);
			return n_next;
		}

		node* erase_after_impl(node* before_first, node* last)
		{
			node* n = before_first->next;
			
			node* next = 0;

			for ( ; n != last ; n = next )
			{
				next = n->next;
				node_alloc().destroy(n);
				node_alloc().deallocate(n, 1);
			}

			before_first->next = last;
			return last;
		}
		
		// previous함수는 근본적으로 선형 검색 부하를 갖는다.
		static node* previous_impl(node* h, const node* pos)
		{
			for ( ; h->next != pos ; h = h->next ) {}
			return h;
		}

		static const node* previous_impl(const node* h, const node* pos)
		{
			for ( ; h->next != pos ; h = h->next ) {}
			return h;
		}

		void	splice_after_impl(node* before_pos, node* before_first, node* before_last)
		{
			if (before_pos == before_first || before_pos == before_last) return;
			
			node* first = before_first->next;
			node* after_pos = before_pos->next;
			before_first->next = before_last->next;
			before_pos->next = first;
			before_last->next = after_pos;
		}
		
		// rhs list를 통째로 잘라서 this의 pos에 붙이는 함수이다..
		void	splice_after_impl(node* before_pos, slist& rhs) // other_head)
		{
			if (this->node_alloc() == rhs.node_alloc() )
			{
				node* rhs_head = rhs.head();
				node* before_last = previous_impl(rhs_head, 0);
					
				if ( before_last == rhs_head )
					return;

				node* after_pos = before_pos->next;
				before_pos->next = rhs_head->next;
				rhs_head->next = 0;
				before_last->next = after_pos;
			}
			else	// allocator의 상태값이 다르면 개별처리해준다..
			{
				insert_after_impl( before_pos, rhs.begin(), rhs.end() );
				rhs.clear();
			}
		}

		compressed_pair<node_allocator_type, node_head>		m_alloc_and_head;
		
		__forceinline const node_allocator_type&		node_alloc() const { return	m_alloc_and_head.first(); }
		__forceinline node_allocator_type&				node_alloc()  { return	m_alloc_and_head.first(); }
		__forceinline const node* 						head() const { return static_cast<const node*>(&m_alloc_and_head.second()); }
		__forceinline node*								head()  { return static_cast<node*>(&m_alloc_and_head.second()); }

	};

	template<class T, class A> inline
	void swap(slist<T,A>& lhs, slist<T,A>& rhs) { lhs.swap(rhs); }

	template <class T, class A>
	inline bool operator==(const slist<T, A>& lhs, const slist<T, A>& rhs)
	{
		typedef typename slist<T,A>::const_iterator const_iterator;
		const_iterator end1 = lhs.end();		const_iterator end2 = rhs.end();
		const_iterator i1 = lhs.begin();		const_iterator i2 = rhs.begin();

		while (i1 != end1 && i2 != end2 && *i1 == *i2)
		{
			++i1;	++i2;
		}
		return i1 == end1 && i2 == end2;
	}

	template <class T, class A>
	inline bool operator!=(const slist<T, A>& lhs, const slist<T, A>& rhs) {	return !(lhs == rhs); }
	
	template<class T, class A> inline
	bool operator<(const slist<T, A>& Left,	const slist<T, A>& Right) {	return lexicographical_compare(Left.begin(), Left.end(), Right.begin(), Right.end()); }

	template<class T, class A> inline
	bool operator>(const slist<T, A>& Left,	const slist<T, A>& Right) {	return Right < Left; }

	template<class T, class A> inline
	bool operator<=(const slist<T, A>& Left, const slist<T, A>& Right) {	return !(Right < Left); }

	template<class T, class A> inline
	bool operator>=(const slist<T, A>& Left, const slist<T, A>& Right) {	return !(Left < Right);	}

	
	// inserter()함수용 insert_iterator 전체 특화

	template <class T, class Alloc>
	class insert_iterator<i3::slist<T, Alloc> >
	{
	public:

		typedef i3::slist<T, Alloc> container_type;
		typedef output_iterator_tag iterator_category;
		typedef void                value_type;
		typedef void                difference_type;
		typedef void                pointer;
		typedef void                reference;

		insert_iterator(container_type& cont, typename container_type::iterator it) : container(&cont)
		{
			if (it == cont.begin())
				iter = cont.before_begin();
			else
				iter = cont.previous(it);
		}

		insert_iterator<container_type>& operator=(const typename container_type::value_type& val)
		{
			iter = container->insert_after(iter, val);	return *this;
		}

		insert_iterator<container_type>& operator*() { return *this; }
		insert_iterator<container_type>& operator++() { return *this; }
		insert_iterator<container_type>& operator++(int) { return *this; }

	protected:
		container_type*						container;
		typename container_type::iterator	iter;
	};




}


#pragma pop_macro("new")
#pragma pack(pop)
