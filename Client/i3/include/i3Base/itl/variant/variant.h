#pragma once

#include "variant_fwd.h"

#include "static_visitor.h"

#include "../preprocessor/ext/pp_enum_params_from_to.h"

#include "../mpl/mpl_is_sequence.h"
#include "../mpl/mpl_vector.h"
#include "../mpl/mpl_back_inserter.h"
#include "../mpl/mpl_copy.h"
#include "../mpl/mpl_contains.h"
#include "../mpl/mpl_contains_if.h"
#include "../mpl/mpl_index_of.h"
#include "../mpl/mpl_index_of_x.h"
#include "../mpl/mpl_deref_x.h"
#include "../mpl/mpl_size.h"
#include "../mpl/mpl_max_element.h"
#include "../mpl/mpl_transform_view.h"
#include "../mpl/mpl_filter_view.h"
#include "../mpl/mpl_min_element.h"
#include "../mpl/mpl_linear_hierarchy.h"
#include "../mpl/mpl_less_equal.h"
#include "../mpl/mpl_transform.h"

#include "../type_traits/is_equivalent.h"
#include "../type_traits/aligned_storage.h"

#include "detail/internal_visitor.h"
#include "detail/type_acceptor.h"
#include "detail/type_converter.h"

#include "detail/enable_recursive_fwd.h"
#include "recursive_wrapper_fwd.h"



#pragma warning(push)
#pragma warning( disable: 4521 4522 4244)

namespace i3
{
	
	template<class T0, pp_enum_params_from_to(1, variant_limit, class T) >
	class variant 
	{
	public:
		typedef variant									this_type;

		typedef detail::variant::is_recursive_flag<T0>	is_recursive_type;
		static const bool								is_recursive = is_recursive_type::value;

		typedef typename mpl::eval_if<is_recursive_type, T0, mpl::identity<T0> >::type	unwrapped_T0;
		
		typedef typename mpl::eval_if<	
			mpl::and_<mpl::is_sequence<unwrapped_T0>, mpl::is_na<T1> >,
			mpl::copy<unwrapped_T0, mpl::back_inserter< mpl::vector<> > >,
			mpl::vector<unwrapped_T0, pp_enum_shifted_params(variant_limit, T)> >::type		specified_types;
		
		typedef typename mpl::eval_if<
			is_recursive_type,
			mpl::transform< specified_types, mpl::protect< detail::variant::quoted_enable_recursive<this_type> > >,
			mpl::identity<specified_types> >::type		internal_sequence_type;
		
		typedef typename mpl::transform<internal_sequence_type, unwrap_recursive<mpl::_1> >::type sequence_type;

		static const size_t sequence_size = mpl::size<sequence_type>::value;

		variant() 
		{
			// non_ref이면서 사이즈가 가장 작은 것을 디폴트타입으로 지정..
			typedef typename mpl::begin<internal_sequence_type>::type	internal_sequence_begin;

			typedef mpl::filter_view< internal_sequence_type, mpl::not_< mpl::or_<is_reference<mpl::_>, is_recursive_wrapper<mpl::_> > > > 
				nonref_sequence_view;

			typedef typename mpl::end<nonref_sequence_view>::type								filter_end;

			typedef typename mpl::min_element< nonref_sequence_view, mpl::less_equal< mpl::sizeof_<mpl::_1>, mpl::sizeof_<mpl::_2> > >::type candidate_iter;
			typedef typename mpl::eval_if< is_same<candidate_iter, filter_end>, 
				mpl::deref<internal_sequence_begin>, mpl::deref<candidate_iter> >::type default_type;
			
			typedef typename mpl::index_of<internal_sequence_type, default_type>::type	index_type;

			detail::variant::construct_type<default_type>(&m_type_index, m_storage.address(), index_type::value);
		}
		
		variant(const variant& rhs) 
		{
			detail::variant::apply_internal_identity_visitor<internal_sequence_type>( 
				detail::internal_clone_visitor(rhs.m_storage.address(), m_storage.address()), rhs.m_type_index );

			m_type_index = rhs.m_type_index;
		}

		template<class T> variant(const T& val) 
		{  // const인자에는 const T를 조건파라미터로..

			typedef typename detail::variant::get_processor<sequence_type, internal_sequence_type, const T>::type	processor_type;
			processor_type::construct(&m_type_index, m_storage.address(), val);
		}

		template<class T> variant(T& val)	
		{	// nonconst인자에는 only T를 조건파라미터로	
			typedef typename detail::variant::get_processor<sequence_type, internal_sequence_type, T>::type	processor_type;
			processor_type::construct(&m_type_index, m_storage.address(), val);
		}
			
		struct copy_type_only_tag {};
		variant(const variant& rhs, copy_type_only_tag) 
		{ 
			detail::variant::apply_internal_identity_visitor<internal_sequence_type>(detail::internal_clone_type_visitor(m_storage.address()), 
				rhs.m_type_index);
			m_type_index = rhs.m_type_index;
		}
		
		// 타 배리언트...생성자..
		template< pp_enum_params(variant_limit, class U)>
		variant(const variant<pp_enum_params(variant_limit, U)>& rhs) {	rhs.apply_visitor(detail::construct_variant_visitor<variant>(this)); }

		template< pp_enum_params(variant_limit, class U)>
		variant(variant<pp_enum_params(variant_limit, U)>& rhs) { rhs.apply_visitor(detail::construct_variant_visitor<variant>(this)); }
		
		// 타 배리언트...대입연산...
		template< pp_enum_params(variant_limit, class U)>
		variant&	operator=( const variant<pp_enum_params(variant_limit, U)>& rhs)
		{
			rhs.apply_visitor(detail::copy_variant_visitor<variant>(this));		return *this;
		}
		template< pp_enum_params(variant_limit, class U)>
		variant&	operator=( variant<pp_enum_params(variant_limit, U)>& rhs)
		{
			rhs.apply_visitor(detail::copy_variant_visitor<variant>(this));		return *this;
		}

		variant&	operator=(const variant& rhs) 
		{	
			if (this == &rhs) return *this;

			if ( m_type_index == rhs.m_type_index)
			{
				detail::variant::apply_internal_identity_visitor<internal_sequence_type>(
					detail::internal_copy_visitor( rhs.m_storage.address(), m_storage.address() ), m_type_index);
			}
			else
			{
				detail::variant::apply_internal_identity_visitor<internal_sequence_type>(
					detail::internal_destroy_visitor(m_storage.address()), m_type_index);

				detail::variant::apply_internal_identity_visitor<internal_sequence_type>(
					detail::internal_clone_visitor( rhs.m_storage.address(), m_storage.address()), rhs.m_type_index);
				m_type_index = rhs.m_type_index;
			}
			
			return *this;
		}
		
		template<class T> variant&	operator=(const T& val)
		{
			typedef typename detail::variant::get_processor<sequence_type, internal_sequence_type, const T>::type	processor_type;
			processor_type::copy(&m_type_index, m_storage.address(), val);		return *this;
		}

		template<class T> variant&	operator=(T& val)
		{
			typedef typename detail::variant::get_processor<sequence_type, internal_sequence_type, T>::type	processor_type;
			processor_type::copy(&m_type_index, m_storage.address(), val);		return *this;
		}

		~variant() 
		{  
			detail::variant::apply_internal_identity_visitor<internal_sequence_type>(
				detail::internal_destroy_visitor(m_storage.address()), m_type_index );
		}		// 쉽다..
		
		template<class T>
		bool	change_type()
		{
			typedef typename detail::variant::get_processor<sequence_type, internal_sequence_type, T>::type	processor_type;
			return processor_type::change_type(&m_type_index, m_storage.address());
		}


#pragma push_macro("new")
#undef new

		void	swap(variant& rhs)
		{
			if (m_type_index == rhs.m_type_index)			// 동종 타입....값만 스웝하면 됨..
			{
				detail::variant::apply_internal_identity_visitor<internal_sequence_type>(
					detail::internal_swap_visitor(m_storage.address(), rhs.m_storage.address()), m_type_index);
			}
			else								// 다른 타입....복잡...
			{
				// Create a temporary storage
				variant temp(rhs, copy_type_only_tag() );
				// Move rhs into temp
				detail::variant::apply_internal_identity_visitor<internal_sequence_type>(
					detail::internal_swap_visitor(rhs.m_storage.address(), temp.m_storage.address()), rhs.m_type_index);

				rhs.~variant();
				// Move *this into rhs
				new (&rhs) variant(*this, copy_type_only_tag());
				detail::variant::apply_internal_identity_visitor<internal_sequence_type>(
					detail::internal_swap_visitor(m_storage.address(), rhs.m_storage.address()), m_type_index);

				this->~variant();
				// Move temp into *this
				new (this) variant(temp, copy_type_only_tag());
				detail::variant::apply_internal_identity_visitor<internal_sequence_type>(
					detail::internal_swap_visitor(m_storage.address(), temp.m_storage.address()), m_type_index);

			}
		}
#pragma pop_macro("new")

		void						reset()
		{
			detail::variant::apply_internal_identity_visitor<internal_sequence_type>(detail::internal_reset_visitor(m_storage.address()), m_type_index);
		}
		
		bool						empty() const { return false; } //항상 존재한다..부스트도 이러한구조..
		int							which() const	{ return m_type_index; }			
		const std::type_info&		type() const { return apply_identity_visitor(detail::internal_typeid_visitor()); }	// internal_visitor가 아님..		
	
		template<class U> bool	operator==(const U&) const { compile_assert_msg( false, specific_type_comparison_with_variant_is_prohibited ); }
		template<class U> bool  operator<(const U&) const  { compile_assert_msg( false, specific_type_comparison_with_variant_is_prohibited ); }

		bool					operator==(const variant& rhs) const
		{
			if ( m_type_index != rhs.m_type_index ) return false;		
			return detail::variant::apply_internal_identity_visitor<internal_sequence_type>(
				detail::internal_equal_visitor(m_storage.address(), rhs.m_storage.address()), m_type_index);
		}

		bool					operator<(const variant& rhs) const
		{
			if (m_type_index != rhs.m_type_index ) return (m_type_index < rhs.m_type_index);
			return detail::variant::apply_internal_identity_visitor<internal_sequence_type>(
				detail::internal_less_visitor(m_storage.address(), rhs.m_storage.address()), m_type_index);
		}

		template<class visitor>
		typename visitor::result_type	apply_visitor(visitor& vis)
		{
			typedef detail::type_acceptor<sequence_type, internal_sequence_type, visitor, false>	acceptor;
			return acceptor()( m_type_index, vis, m_storage.address() );
		}

		template<class visitor>
		typename visitor::result_type	apply_visitor(visitor& vis) const
		{
			typedef detail::type_acceptor<sequence_type, internal_sequence_type, visitor, true>	acceptor;
			return acceptor()( m_type_index, vis, m_storage.address() );
		}

		template<class visitor>
		typename visitor::result_type	apply_visitor(const visitor& vis)
		{
			typedef detail::type_acceptor<sequence_type, internal_sequence_type, const visitor, false>	acceptor;
			return acceptor()( m_type_index, vis, m_storage.address() );
		}

		template<class visitor>
		typename visitor::result_type	apply_visitor(const visitor& vis) const
		{
			typedef detail::type_acceptor<sequence_type, internal_sequence_type, const visitor, true>	acceptor;
			return acceptor()( m_type_index, vis, m_storage.address() );
		}
/////////////////////

		template<class visitor>
		typename visitor::result_type	apply_identity_visitor(visitor& vis) const
		{
			typedef detail::type_identity_acceptor<sequence_type, visitor>	acceptor;
			return acceptor()( m_type_index, vis );
		}

		template<class visitor>
		typename visitor::result_type	apply_identity_visitor(const visitor& vis) const
		{
			typedef detail::type_identity_acceptor<sequence_type, const visitor>	acceptor;
			return acceptor()( m_type_index, vis );
		}

		template<class visitor>
		typename visitor::result_type	apply_identity_visitor(visitor& vis, int index) const
		{
			typedef detail::type_identity_acceptor<sequence_type, visitor>	acceptor;
			return acceptor()( index, vis );
		}

		template<class visitor>
		typename visitor::result_type	apply_identity_visitor(const visitor& vis, int index) const
		{
			typedef detail::type_identity_acceptor<sequence_type, const visitor>	acceptor;
			return acceptor()( index, vis );
		}

	private:
	
		static const std::size_t storage_size = mpl::deref_x< 
			typename mpl::max_element< mpl::transform_view< internal_sequence_type, mpl::sizeof_<mpl::_> > >::type >::value;

		static const std::size_t storage_align = mpl::deref_x<
			typename mpl::max_element< mpl::transform_view< internal_sequence_type, i3::alignment_of<mpl::_> > >::type >::value;

		i3::aligned_storage<storage_size, storage_align>	m_storage;
		int													m_type_index;
		
		template<pp_enum_params(variant_limit, class U) > friend class variant;
	};
	
	template<pp_enum_params(variant_limit, class T)> __forceinline	
	void	swap( const variant<pp_enum_params(variant_limit, T)>& lhs, 
		const variant<pp_enum_params(variant_limit, T)>& rhs) {	lhs.swap(rhs); }
	
	namespace detail
	{
		template<class T>
		struct get_value_visitor
		{
			typedef typename add_pointer<T>::type	pointer;
			typedef typename add_reference<T>::type reference;
			typedef pointer							result_type;
			result_type operator()(reference v) const { return &v; }		
			template<class U>
			result_type operator()(const U&) const { return static_cast<pointer>(nullptr); }
		};
	}

	template<class U, pp_enum_params(variant_limit, class T)> __forceinline
	typename add_pointer<U>::type	get( variant<pp_enum_params(variant_limit, T)>* v)
	{
		typedef typename add_pointer<U>::type pointer;
		if (!v) return static_cast<pointer>(nullptr);
		return v->apply_visitor(detail::get_value_visitor<U>());
	}
	
	template<class U, pp_enum_params(variant_limit, class T)> __forceinline
	typename add_pointer<U>::type	get( const variant<pp_enum_params(variant_limit, T)>* v)
	{
		typedef typename add_pointer<const U>::type pointer;
		if (!v) return static_cast<pointer>(nullptr);
		return v->apply_visitor(detail::get_value_visitor<const U>());
	}

	struct bad_get : public std::exception 
	{ 
		virtual const char* what() const throw(){	return "i3::bad_get: failed value get using i3::get"; } 
	};
	
	template<class U, pp_enum_params(variant_limit, class T)> __forceinline	
	typename add_reference<U>::type get(variant<pp_enum_params(variant_limit, T)>& v )
	{
		typedef typename add_pointer<U>::type pointer;
		pointer res = get<U>(&v);
		if (!res) throw bad_get();
		return *res;
	}
	
	template<class U, pp_enum_params(variant_limit, class T)> __forceinline	
	typename add_reference<const U>::type get(const variant<pp_enum_params(variant_limit, T)>& v )
	{
		typedef typename add_pointer<const U>::type pointer;
		pointer res = get<const U>(&v);
		if (!res) throw bad_get();
		return *res;
	}	
	
	template<class Seq>
	struct make_variant_over 
	{
		typedef variant<Seq>	type;
	};
	
	template< pp_enum_params(variant_limit, class T), class Visitor> __forceinline
	typename Visitor::result_type  apply_identity_visitor( i3::identity< variant< pp_enum_params(variant_limit, T)> >, 
		const Visitor& visit, int index)
	{
		typedef variant< pp_enum_params(variant_limit, T)>	variant_t;
		typedef typename variant_t::sequence_type			sequence_type;

		typedef detail::type_identity_acceptor<sequence_type, const Visitor>	acceptor;
		return acceptor()( index, visit );
	}
	
	template< pp_enum_params(variant_limit, class T), class Visitor> __forceinline
	typename Visitor::result_type  apply_identity_visitor( i3::identity< variant< pp_enum_params(variant_limit, T)> >, 
		Visitor& visit, int index)
	{
		typedef variant< pp_enum_params(variant_limit, T)>	variant_t;
		typedef typename variant_t::sequence_type			sequence_type;

		typedef detail::type_identity_acceptor<sequence_type, Visitor>	acceptor;
		return acceptor()( index, visit );
	}
	
}

#pragma warning(pop)
