#pragma once

namespace i3
{
	namespace intrusive
	{
		struct member_tag;
		struct default_smart_list_base_tag;
		struct default_smart_slist_base_tag;

		template<class Node, class Tag>
		struct node_holder : Node {};
		
		template<class Derived, class Base, class Tag>
		struct CRTP_default_smart_hook : Base {};

		template<class Derived, class Base>
		struct CRTP_default_smart_hook<Derived, Base, default_smart_list_base_tag> : Base 
		{
			typedef Derived		default_smart_list_hook;
		};
		
		template<class Derived, class Base>
		struct CRTP_default_smart_hook<Derived, Base, default_smart_slist_base_tag> : Base 
		{
			typedef Derived		default_smart_slist_hook;
		};

		template<class NodeAlgorithm, class Tag>
		struct smart_generic_hook : mpl::if_<	is_same<Tag, member_tag>,
												typename NodeAlgorithm::node,
												CRTP_default_smart_hook<	smart_generic_hook<NodeAlgorithm, Tag>,
																				node_holder<typename NodeAlgorithm::node, Tag>,
																				Tag> >::type
		{
			typedef NodeAlgorithm								node_algorithm;
			typedef typename node_algorithm::node				node;
			typedef typename node_algorithm::node_traits		node_traits;
			typedef Tag											tag;
			
			node*			this_ptr() { return static_cast<node*>(this); }
			const node*		this_ptr() const { return static_cast<const node*>(this); }

			smart_generic_hook() { node_algorithm::init(this_ptr()); }
			smart_generic_hook(const smart_generic_hook& ) { node_algorithm::init(this_ptr()); }
			smart_generic_hook& operator=(const smart_generic_hook&) { return *this; }
			~smart_generic_hook() { assert( !is_linked() ); }

			bool is_linked() const { return !node_algorithm::unique(this_ptr()); }
			void swap_nodes(smart_generic_hook& rhs)
			{
				node_algorithm::swap_nodes(this_ptr(), rhs.this_ptr());
			}
		};
		
	}


}

namespace intrusive = i3::intrusive;
