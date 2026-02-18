#pragma once

#include "type_accessor.h"
#include "internal_visitor.h"

namespace i3
{
	namespace detail {	namespace variant
	{

		struct	exist_type_processor_tag;
		struct	equivalent_type_processor_tag;
		struct	convertible_type_processor_tag;
		struct	error_type_processor_tag;

		template<class Seq, class T>
		struct select_tag
		{
			typedef typename mpl::multi_if< 
				mpl::contains<Seq, T>, exist_type_processor_tag,
				mpl::contains_if<Seq, is_equivalent<T, mpl::_> >, equivalent_type_processor_tag,
				mpl::contains_if<Seq, is_convertible<T, mpl::_> >, convertible_type_processor_tag,
				error_type_processor_tag>::type			type;
		};


		template<class TargetType> __forceinline
		void construct_type(int* dest_index, void* dest_data, int src_index)
		{
			detail::type_accessor<TargetType>::construct(dest_data);
			*dest_index = src_index;
		}

		template<class TargetType, class T> __forceinline
		void construct_type(int* dest_index, void* dest_data, int src_index, const T& val)
		{
			detail::type_accessor<TargetType>::construct(dest_data, val);
			*dest_index = src_index;		//mpl::index_of<sequence_type, TargetType>::type
		}
		
		template<class TargetType, class T> __forceinline
		void construct_type(int* dest_index, void* dest_data, int src_index, T& val) 
		{	
			detail::type_accessor<TargetType>::construct(dest_data, val);	 
			*dest_index = src_index;
		}

		template<class Seq, class Visitor> __forceinline
		typename Visitor::result_type	apply_internal_identity_visitor(const Visitor& vis, int type_index) 
		{
			typedef detail::type_identity_acceptor<Seq, const Visitor>	acceptor;
			return acceptor()( type_index, vis );
		}

		template<class Seq, class TargetType, class T> __forceinline
		void copy_type( int* dest_index, void* dest_data, int src_index, const T& val) 
		{ 
			if ( *dest_index == src_index )
			{
				detail::type_accessor<TargetType>::copy(dest_data, val);	
			}
			else																// 소멸시키고 재생성해야함..
			{
				apply_internal_identity_visitor<Seq>(detail::internal_destroy_visitor(dest_data), *dest_index);
				detail::type_accessor<TargetType>::construct(dest_data, val);	// new (m_storage.address()) tgtType(val);
				*dest_index = src_index;
			}
		}
		
		template<class Seq, class TargetType, class T> __forceinline
		void copy_type( int* dest_index, void* dest_data, int src_index, T& val) 
		{ 
			if ( *dest_index == src_index )
			{
				detail::type_accessor<TargetType>::copy(dest_data, val);		
			}
			else																// 소멸시키고 재생성해야함..
			{
				apply_internal_identity_visitor<Seq>(detail::internal_destroy_visitor(dest_data), *dest_index );
				detail::type_accessor<TargetType>::construct(dest_data, val);	
				*dest_index = src_index;
			}
		}
		
		template<class Seq, class TargetType> __forceinline
		bool convert_type(int* dest_index, void* dest_data, int src_index)
		{
			if ( *dest_index == src_index )
				return true;		// 타입이 동일한 경우 성공으로 간주하고 아무일도 하지 않음..

			if ( apply_internal_identity_visitor<Seq>( detail::internal_change_type_visitor<TargetType>(dest_data), *dest_index) )
			{
				*dest_index = src_index;
				return true;
			}
			return false;
		}
		
		template<class Seq, class InternalSeq, class T, class processor_tag> 
		struct processor;

		template<class Seq, class InternalSeq, class T>
		struct get_processor
		{
			typedef processor<Seq, InternalSeq, T, typename select_tag<Seq, T>::type>	type;
		};
	

		template<class Seq, class InternalSeq, class T> 
		struct processor<Seq, InternalSeq, T, detail::variant::exist_type_processor_tag>		// 동일타입..
		{ 
			typedef typename mpl::index_of<Seq, T>::type	index_type;
			typedef typename mpl::at<InternalSeq, index_type>::type	target_type;

			__forceinline static void construct(int* dest_index, void* dest_data, T& val)
			{	
				construct_type<target_type>(dest_index, dest_data, index_type::value, val);	
			}
			__forceinline static void copy(int* dest_index, void* dest_data, T& val) 
			{  
				copy_type<InternalSeq, target_type>(dest_index, dest_data, index_type::value, val);   
			}
			__forceinline static bool change_type(int* dest_index, void* dest_data) 
			{  
				return convert_type<InternalSeq, target_type>(dest_index, dest_data, index_type::value);  
			}
		};

		template<class Seq, class InternalSeq, class T> 
		struct processor<Seq, InternalSeq, T, detail::variant::equivalent_type_processor_tag>
		{
			typedef typename mpl::find_if<Seq, is_equivalent<T,mpl::_> >::type iter;
			typedef typename mpl::index_of<Seq, typename mpl::deref<iter>::type >::type	index_type;
			typedef typename mpl::at<InternalSeq, index_type>::type	target_type;

			__forceinline static void construct(int* dest_index, void* dest_data, T& val)
			{	
				construct_type<target_type>(dest_index, dest_data, index_type::value, val); 
			}

			__forceinline static void copy(int* dest_index, void* dest_data, T& val) 
			{	
				copy_type<InternalSeq, target_type>(dest_index, dest_data, index_type::value, val);  
			}

			__forceinline static bool change_type(int* dest_index, void* dest_data)
			{ 	
				return convert_type<InternalSeq, target_type>(dest_index, dest_data, index_type::value); 
			}
		};

#pragma warning(push)
#pragma warning( disable: 4244)

		template<class T, class Base>			// 여기서 T는 각 리스트의 타입들..(문제가 있다면 add_reference를 쓴다..)
		struct construct_type_visitor : public Base
		{
			using Base::operator();			// 가림처리해제.
			typedef typename make_parameter<T>::type			param_type;


			template<class Seq, class InternalSeq>	__forceinline 
			static void		call(int* dest_index, void* dest_data, param_type val)
			{  
				typedef typename mpl::index_of<Seq, T>::type	index_type;
				typedef typename mpl::at<InternalSeq, index_type>::type	target_type;

				construct_type<target_type>(dest_index, dest_data, index_type::value, val); 
			}
		};

		template<class T, class Base>			// 여기서 T는 각 리스트의 타입들..
		struct copy_type_visitor : public Base
		{
			using Base::call;			// 가림처리해제.
			typedef typename make_parameter<T>::type			param_type;

			template<class Seq, class InternalSeq> __forceinline
			static void		call(int* dest_index, void* dest_data, param_type val)
			{ 
				typedef typename mpl::index_of<Seq, T>::type	index_type;
				typedef typename mpl::at<InternalSeq, index_type>::type	target_type;

				copy_type<InternalSeq, target_type>(dest_index, dest_data, index_type::value, val);  
			}
		};

		template<class T, class Base>
		struct change_type_visitor : public Base
		{
			using Base::test;				// 가림처리 해제..
			template<class Seq>
			static char	(&test(T))[ mpl::index_of<Seq, T>::type::value + 1];
		};


		struct variant_unit_base
		{
			static void	call();
			static void	test();
		};


		template<class Seq, class InternalSeq, class T> 
		struct processor<Seq, InternalSeq, T, detail::variant::convertible_type_processor_tag>
		{

			__forceinline static void	construct(int* dest_index, void* dest_data, T& val)
			{	
				typedef mpl::filter_view<Seq, is_convertible<T,mpl::_> > view;
				typedef typename mpl::linear_hierarchy<view, construct_type_visitor, variant_unit_base>::type visitor_type;
				visitor_type::call<Seq, InternalSeq>(dest_index, dest_data, val);
			}

			__forceinline static void	copy(int* dest_index, void* dest_data, T& val)
			{	
				typedef mpl::filter_view<Seq, is_convertible<T,mpl::_> > view;
				typedef typename mpl::linear_hierarchy<view, copy_type_visitor, variant_unit_base>::type visitor_type;
				visitor_type::call<Seq, InternalSeq>(dest_index, dest_data, val);
			}

			__forceinline static bool change_type(int* dest_index, void* dest_data)
			{
				typedef mpl::filter_view<Seq, is_convertible<T,mpl::_> > view;
				typedef typename mpl::linear_hierarchy<view, change_type_visitor, variant_unit_base>::type visitor_type;
				typedef typename add_reference<T>::type test_type;
				struct Test {  static test_type call();  };

				static const size_t found_index = sizeof( visitor_type::test<Seq>( Test::call() ) ) - 1;

				typedef mpl::int_<found_index>									index_type;
				typedef mpl::at_c<InternalSeq, found_index>::type				target_type;

				return convert_type< InternalSeq, target_type>(dest_index, dest_data, index_type::value);
			}
		};

#pragma warning(pop)

		template<class Seq, class InternalSeq, class T> 
		struct processor<Seq, InternalSeq, T, detail::variant::error_type_processor_tag>
		{
			__forceinline static void construct(int* dest_index, void* dest_data, T& )
			{
				typedef typename mpl::begin<InternalSeq>::type	internal_sequence_begin;

				typedef mpl::filter_view< InternalSeq, mpl::not_< mpl::or_<is_reference<mpl::_>, is_recursive_wrapper<mpl::_> > > > 
					nonref_sequence_view;

				typedef typename mpl::end<nonref_sequence_view>::type								filter_end;

				typedef typename mpl::min_element< nonref_sequence_view, mpl::less_equal< mpl::sizeof_<mpl::_1>, mpl::sizeof_<mpl::_2> > >::type candidate_iter;
				typedef typename mpl::eval_if< is_same<candidate_iter, filter_end>, 
					mpl::deref<internal_sequence_begin>, mpl::deref<candidate_iter> >::type default_type;

				typedef typename mpl::index_of<InternalSeq, default_type>::type	index_type;

				construct_type<default_type>(dest_index, dest_data, index_type::value);
			}

			__forceinline static void copy(int* , void* , T& ) {}	
			__forceinline static bool change_type(int* , void* ) { return false; }	// 해당타입이 없으면 실패를 리턴시킨다...
		};




	}}
}