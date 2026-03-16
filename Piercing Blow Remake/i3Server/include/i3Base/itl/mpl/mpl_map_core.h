#pragma once

#include "mpl_contains_fwd.h"
#include "mpl_if.h"

#include "mpl_at_fwd.h"
#include "mpl_long.h"
#include "mpl_order.h"
#include "mpl_if.h"
#include "mpl_pair.h"
#include "mpl_void.h"

#include "mpl_insert_fwd.h"
#include "mpl_next_prior.h"
#include "mpl_na.h"

#include "mpl_erase_fwd.h"
#include "mpl_erase_key_fwd.h"
#include "mpl_identity.h"
#include "mpl_base.h"

#include "mpl_has_key_fwd.h"
#include "mpl_key_type_fwd.h"
#include "mpl_value_type_fwd.h"
#include "mpl_clear_fwd.h"
#include "mpl_size_fwd.h"

#include "mpl_not.h"
#include "mpl_empty_fwd.h"
#include "mpl_begin_end_fwd.h"
#include "mpl_next_prior.h"

#include "mpl_iterator_tags.h"
#include "mpl_deref.h"

#include "aux_/mpl_ptr_to_ref.h"

namespace i3
{
	namespace mpl
	{
		namespace aux { struct map_tag; }

		template< typename Dummy = na > 
		struct map0
		{
			typedef map0            type;
			typedef aux::map_tag    tag;
			typedef void_           key_;
			typedef long_<1>        order;
			typedef long_<0>        size;
			
			static no_type order_by_key_(const map0<>&, void const volatile*);
			static no_type is_masked_(const map0<>&, void const volatile*);
		};

		template< class Map, long n > struct m_at {	typedef void_ type;	};
		
		template<>
		struct at_impl< aux::map_tag >
		{
			template< class Map, class Key > 
			struct apply
			{
				typedef typename m_at< Map, (x_order_impl<Map,Key>::value - 2) >::type item_;       
				typedef typename eval_if< is_void_<item_>, void_, second<item_>	>::type type;
			};
		};

		template< class Map, long order > 
		struct is_item_masked
		{
			static const bool value = sizeof( Map::is_masked_( aux::ptr_to_ref(static_cast<Map*>(0)), static_cast<long_<order>*>(0)) ) == sizeof(yes_type);
		};

		template< class Map, long order > 
		struct item_by_order
		{    
			typedef typename eval_if_c< is_item_masked<Map,order>::value, void_, m_at<Map,(order - 2)> >::type type;
		};

		template<>
		struct contains_impl<aux::map_tag>
		{
			template< class Map, class Pair > 
			struct apply : is_same< typename at_impl<aux::map_tag>::apply<Map, typename Pair::first>::type, typename Pair::second> {};
		};

		template< long n, class Key, class T, class Base >
		struct m_item;
		

		template< class Key, class T, class Base >
		struct m_item_ : Base
		{
			typedef Key     key_;
			typedef Base    base;
			typedef m_item_ type;
		    
			typedef typename next< typename Base::size >::type  size;
			typedef typename next< typename Base::order >::type order;

			typedef char (&order_tag_)[order::value];
			
			static order_tag_ order_by_key_(const m_item_&, identity<Key>* );
			using Base::order_by_key_;
		};

		template< class Key, class Base >
		struct m_mask : Base
		{
			typedef void_   key_;
			typedef Base    base;

			typedef typename prior< typename Base::size >::type  size;
			typedef typename x_order_impl<Base,Key>::type key_order_;
		    
			static no_type order_by_key_(const m_mask&, identity<Key>* );
			using Base::order_by_key_;

			static yes_type is_masked_(const m_mask&, key_order_*);
			using Base::is_masked_;
		};
		
		namespace aux
		{
			template <class Map, class Pair>
			struct map_insert_impl
				: if_< contains_impl<aux::map_tag>::apply<Map,Pair>, Map
						, m_item<Map::order::value, typename Pair::first, typename Pair::second, Map> > {};
		}

		template<>
		struct insert_impl< aux::map_tag >
		{
			template< class Map, class PosOrKey, class KeyOrNA> 
			struct apply : aux::map_insert_impl<Map, typename if_na<KeyOrNA,PosOrKey>::type>{};
		};

		template<>
		struct has_key_impl< aux::map_tag >
		{
			template< class Map, class Key > 
			struct apply : bool_< ( x_order_impl<Map,Key>::value > 1 ) >{};
		};
		
		template<>
		struct erase_key_impl< aux::map_tag >
		{
			template< class Map, class Key> 
			struct apply : eval_if< has_key_impl<aux::map_tag>::apply<Map,Key>, 
									eval_if< is_same< Key,typename Map::key_ >, base<Map>, identity< m_mask<Key,Map> > >, 
									identity<Map> >	{};
		};

		template<>
		struct erase_impl< aux::map_tag >
		{
			template< class Map, class Pos, class unused_> 
			struct apply : erase_key_impl<aux::map_tag>::apply<Map,typename Pos::type::first>{};
		};
		
		template<>
		struct key_type_impl< aux::map_tag >
		{
			template< class Map, class T > 
			struct apply : first<T> {};
		};

		template<>
		struct value_type_impl< aux::map_tag >
		{
			template< class Map, class T > 
			struct apply : second<T> {};
		};

		template<>
		struct clear_impl< aux::map_tag >
		{
			template< class Map > struct apply {	typedef map0<> type; };
		};
		
		template<>
		struct size_impl< aux::map_tag >
		{
			template< class Map > struct apply : Map::size {};
		};

		template<>
		struct empty_impl< aux::map_tag >
		{
			template< class Map > struct apply : not_< typename Map::size > {};
		};
		

		template< class Map, long order, long max_order>
		struct next_order : if_< is_void_< typename item_by_order<Map,order>::type >, 
								 next_order<Map,(order+1),max_order>, 
								 long_<order> >::type {};

		template< class Map, long max_order>
		struct next_order<Map,max_order,max_order> : long_<max_order> {};

		template< class Map, long order, long max_order >
		struct m_iter
		{
			typedef forward_iterator_tag category;
			typedef typename item_by_order<Map,order>::type type;
		};

		template< class Map, long max_order >
		struct m_iter<Map,max_order,max_order>
		{
			typedef forward_iterator_tag category;
		};

		template< class Map, long order, long max_order > 
		struct next< m_iter<Map,order,max_order> >
		{
			typedef m_iter< Map, next_order<Map,order+1,max_order>::value, max_order > type;
		};

		template< class Map, long max_order > 
		struct next< m_iter<Map,max_order,max_order> > {};
		
		template<>
		struct begin_impl< aux::map_tag >
		{
			template< typename Map > 
			struct apply
			{
				typedef typename next< typename Map::order >::type max_order_;
				typedef m_iter<Map, next_order<Map,1,max_order_::value>::value, max_order_::value> type;
			};
		};

		template<>
		struct end_impl< aux::map_tag >
		{
			template< typename Map > 
			struct apply
			{
				typedef typename next< typename Map::order >::type max_order_;
				typedef m_iter< Map,max_order_::value,max_order_::value > type;
			};
		};
		


	}
}
