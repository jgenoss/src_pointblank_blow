#pragma once

//  이름선택의 아이디어 제한때문에, 향후 템플릿 타입이름은 대문자로 시작하게 한다. ( 앞으로 이 원칙으로 전환할것)
#include "mpl_long.h"
#include "mpl_na.h"
#include "mpl_limit.h"
#include "mpl_push_front_fwd.h"
#include "mpl_pop_front_fwd.h"
#include "mpl_next_prior.h"
#include "mpl_push_back_fwd.h"
#include "mpl_bool.h"
#include "mpl_front_fwd.h"
#include "mpl_clear_fwd.h"
#include "mpl_O1_size_fwd.h"
#include "mpl_size_fwd.h"
#include "mpl_empty_fwd.h"
#include "mpl_not.h"
#include "mpl_begin_end_fwd.h"

#include "mpl_iterator_tags.h"
#include "mpl_deref.h"


namespace i3
{
	namespace mpl
	{
		namespace aux {	struct list_tag;	struct l_iter_tag;	}

		template<class Size, class T, class Next>
		struct l_item
		{
			typedef aux::list_tag	tag;	typedef l_item			type;	typedef Size		size;
			typedef T				item;	typedef Next			next;
		};
		struct l_end
		{
			typedef aux::list_tag	tag;	typedef l_end			type;	typedef long_<0>	size;
		};

		template<>
		struct push_front_impl< aux::list_tag >
		{
			template< class List, class T > 
			struct apply {	typedef l_item<typename next<typename List::size>::type, T, typename List::type> type;	};
		};
		
		template<>
		struct pop_front_impl< aux::list_tag >
		{
			template<class List> struct apply { typedef typename mpl::next<List>::type type; };
		};
		
		template<class Tag> struct has_push_back_impl;
		template<> struct has_push_back_impl< aux::list_tag> {	template<class Seq> struct apply : false_ {}; };
		
		template<>
		struct front_impl<aux::list_tag> {	template<class List> struct apply { typedef typename List::item type; }; };

		template<> struct clear_impl<aux::list_tag>	{	template<class List> struct apply { typedef l_end type; };	};	

		template<>	struct size_impl<aux::list_tag> {	template<class List> struct apply : List::size {};	};	//상속대상 최외각에는 typename이 붙지 않음..
		template<>	struct O1_size_impl<aux::list_tag> {	template<class List> struct apply : List::size {};	};	//상속대상 최외각에는 typename이 붙지 않음..
	
		template<>	struct empty_impl< aux::list_tag >{ template< class List > struct apply : not_<typename List::size> {}; };

		template< class Node > struct l_iter {	typedef aux::l_iter_tag tag;	typedef forward_iterator_tag category;	};
		template< class Node > struct deref< l_iter<Node> >	{	typedef typename Node::item type;	};
		template< class Node > struct next< l_iter<Node> > {	typedef l_iter< typename Node::next > type;	};
		template<> struct l_iter<l_end> {	typedef aux::l_iter_tag tag;	typedef forward_iterator_tag category;	};

		template<> struct begin_impl< aux::list_tag > {	template< class List > struct apply { typedef l_iter<typename List::type> type;	};	};
		template<> struct end_impl< aux::list_tag > {	template< class > struct apply {	typedef l_iter<l_end> type;	};	};
	
	}
}

#define lambda_spec (1, l_iter)
#include "aux_/mpl_lambda_spec.h"
