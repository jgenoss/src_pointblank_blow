#pragma once

#include "mpl_at_fwd.h"
#include "mpl_has_key_fwd.h"
#include "mpl_bool.h"
#include "mpl_if.h"
#include "mpl_void.h"
#include "aux_/mpl_ptr_to_ref.h"

#include "mpl_clear_fwd.h"
#include "mpl_long.h"
#include "mpl_na.h"
#include "mpl_size_fwd.h"
#include "mpl_empty_fwd.h"
#include "mpl_not.h"
#include "mpl_insert_fwd.h"

#include "mpl_next_prior.h"
#include "mpl_identity.h"
#include "mpl_base.h"

#include "mpl_erase_fwd.h"
#include "mpl_erase_key_fwd.h"
#include "mpl_key_type_fwd.h"
#include "mpl_value_type_fwd.h"
#include "mpl_begin_end_fwd.h"

#include "mpl_has_key.h"
#include "mpl_iterator_tags.h"

#include "mpl_integral_c.h"

namespace i3
{
	namespace mpl
	{
		namespace aux { struct set_tag; }

		template<class Dummy = na> 
		struct set0
		{
			typedef set0<>				item_;
			typedef item_				type;
			typedef aux::set_tag		tag;
			typedef void_				last_masked_;
			typedef void_				item_type_;
			typedef long_<0>			size;
			typedef long_<1>			order;
			
			static no_type	order_by_key_(const set0<>&, void const volatile*);
			static yes_type	is_masked_(const set0<>&, void const volatile*);
		};

		template< class T > 
		struct set0_c : set0<>
		{
			typedef set0_c type;	typedef T value_type;
		};


		template<>
		struct has_key_impl< aux::set_tag >
		{
			template< class Set, class T > 
			struct apply 
			{
				static const bool value = (	sizeof( Set::is_masked_( aux::ptr_to_ref(static_cast<Set*>(0)), static_cast<identity<T>*>(0)) ) == sizeof(no_type) );
				typedef bool_<value> type;
			};
		};

		template<>
		struct at_impl< aux::set_tag >
		{
			template< class Set, class T > 
			struct apply
			{
				typedef typename if_< has_key_impl<aux::set_tag>::apply<Set,T>, T, void_ >::type type;            
			};
		};

		template<>
		struct clear_impl<aux::set_tag>
		{
			template<class Set> struct apply { typedef set0<> type; };
		};

		template<>
		struct size_impl< aux::set_tag >
		{
			template< typename Set > struct apply : Set::size{};
		};

		template<>
		struct empty_impl< aux::set_tag >
		{
			template< typename Set > struct apply : not_< typename Set::size > {};
		};
		

		template< class T, class Base >
		struct s_item : Base
		{
			typedef s_item<T,Base>	item_;
			typedef void_			last_masked_;
			typedef T				item_type_;
			typedef Base			base;
		    typedef typename next< typename Base::size >::type  size;
			typedef typename next< typename Base::order >::type order;

			typedef char (&order_tag_)[order::value];
		
			static order_tag_	order_by_key_(const s_item&, identity<T>*);
			using Base::order_by_key_;
			static no_type		is_masked_(const s_item&, identity<T>*);
			using Base::is_masked_;
		};
		

		template< class T, class Base >
		struct s_mask : Base
		{
			typedef s_mask<T,Base> item_;
			typedef T       last_masked_;
			typedef void_   item_type_;
			typedef Base    base;
			typedef typename prior< typename Base::size >::type  size;
			
			static yes_type is_masked_(const s_mask&, identity<T>* );
			using Base::is_masked_;
		};

		template< class T, class Base >
		struct s_unmask : Base
		{
			typedef s_unmask<T,Base> item_;
			typedef void_   last_masked_;
			typedef T       item_type_;
			typedef Base    base;
			typedef typename next< typename Base::size >::type  size;
			
			static no_type is_masked_(const s_unmask&, identity<T>* );
			using Base::is_masked_;
		};
////
		
		namespace aux 
		{
			template<  class Set, class T > 
			struct set_insert_impl : eval_if<	has_key_impl<aux::set_tag>::apply<Set,T>, identity<Set>, 
												eval_if<is_same< T,typename Set::last_masked_ >, base<Set>, 
														identity< s_item<T,typename Set::item_> > > > {};
		}

		template<>
		struct insert_impl< aux::set_tag >
		{
			template< class Set, class PosOrKey, class KeyOrNA> 
			struct apply : aux::set_insert_impl<Set, typename if_na<KeyOrNA,PosOrKey>::type	> {};
		};

		template<>
		struct erase_key_impl< aux::set_tag >
		{
			template< class Set, class T> 
			struct apply : eval_if< has_key_impl<aux::set_tag>::apply<Set,T>, 
									eval_if< is_same< T,typename Set::item_type_ >, base<Set>, 
											identity< s_mask<T,typename Set::item_> > >, 
									identity<Set> >
			{
			};
		};
		
		template<>
		struct erase_impl< aux::set_tag >
		{
			template< class Set, class Pos, class unused_> 
			struct apply : erase_key_impl<aux::set_tag>::apply<Set,typename Pos::type> {};
		};
		
		template<>
		struct key_type_impl< aux::set_tag >
		{
			template< class Set, class T > struct apply { typedef T type; };
		};
		
		template<>
		struct value_type_impl< aux::set_tag >
		{
			template< class Set, class T > struct apply { typedef T type; };
		};
		
		template< class Set, class Tail > struct s_iter;

		template< class Set, class Tail > 
		struct s_iter_get : eval_if< has_key< Set,typename Tail::item_type_ >, identity< s_iter<Set,Tail> >, next< s_iter<Set,Tail> > >
		{
		};

		template< class Set, class Tail > 
		struct s_iter_impl
		{
			typedef Tail                        tail_;
			typedef forward_iterator_tag        category;
			typedef typename Tail::item_type_   type;
		};
		
		template< class Set, class Tail > 
		struct next< s_iter<Set,Tail> > : s_iter_get< Set,typename Tail::base >	{};

		template< class Set > 
		struct next< s_iter<Set,set0<> > > { typedef s_iter<Set,set0<> > type;	};

		template< class Set, class Tail > 
		struct s_iter : s_iter_impl<Set,Tail> {};

		template< class Set > struct s_iter<Set, set0<> > {	typedef forward_iterator_tag category; };
		
		template<>
		struct begin_impl< aux::set_tag >
		{
			template< class Set > struct apply : s_iter_get<Set,typename Set::item_> {};
		};

		template<>
		struct end_impl< aux::set_tag >
		{
			template< class Set > struct apply { typedef s_iter< Set,set0<> > type; };
		};


	}
}
