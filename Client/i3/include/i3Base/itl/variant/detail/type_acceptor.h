#if !defined(pp_is_iterating)

#ifndef type_acceptor_h
#define type_acceptor_h

#include "../../mpl/mpl_at.h"

#include "../../type_traits/functor_can_be_called.h"

#pragma warning(push)
#pragma warning(disable : 4244)				// 알고리즘 함수객체의 경우, 형변환 경고를 막음..(MS기준)

namespace i3
{

	namespace detail
	{
	
		template<class Visitor, class void_ptr, class TargetType>
		struct type_acceptor_function
		{
			typedef typename Visitor::result_type	result_type;
			static result_type visit(Visitor& f, void_ptr data) { return f(type_accessor<TargetType>::get(data));  }
		};
		
		template<class Visitor, class void_ptr>
		struct type_acceptor_null_function
		{
			typedef typename Visitor::result_type	result_type;
			static result_type visit(Visitor& , void_ptr ) { return result_type();  }
		};
		
		template<class Visitor, class IdentityT>
		struct type_acceptor_identity_function
		{
			typedef typename Visitor::result_type	result_type;
			static result_type visit(Visitor& f) { return f( IdentityT() );  }
		};

		template<class Visitor>
		struct type_acceptor_null_identity_function
		{
			typedef typename Visitor::result_type	result_type;
			static result_type visit(Visitor& ) { return result_type();  }
		};


		template<class Seq, class InternalSeq, class Visitor, bool is_const, std::size_t>
		struct type_acceptor_impl;

		template<class Seq, class Visitor, std::size_t>
		struct type_identity_acceptor_impl;

		template<class Seq, class InternalSeq, class Visitor, bool is_const>
		struct type_acceptor
		{
			static const size_t sequence_size = mpl::size<Seq>::value;

			typedef typename Visitor::result_type			result_type;
			typedef typename mpl::if_c<is_const, const void*, void*>::type	void_ptr;

			__forceinline result_type operator()( int index, Visitor& f, void_ptr data) const
			{
				return type_acceptor_impl<Seq, InternalSeq, Visitor, is_const, sequence_size>::call(index, f, data);
			}
		};

		template<class Seq, class Visitor>
		struct type_identity_acceptor
		{
			static const size_t sequence_size = mpl::size<Seq>::value;

			typedef typename Visitor::result_type			result_type;
		
			__forceinline result_type operator()( int index, Visitor& f) const
			{
				return type_identity_acceptor_impl<Seq, Visitor, sequence_size>::call(index, f);
			}
		};
		
	}
		
	
}

#define pp_iteration_params_1 (3, (1, variant_limit, "type_acceptor.h") )
#include pp_iterate()

#pragma warning(pop)

#endif // type_acceptor_h

#else // pp_is_iterating

#if				pp_iteration_depth() == 1

#define i_		pp_frame_iteration(1)

#define type_acceptor_impl_call					0x00
#define type_acceptor_impl_call_identity		0x01

namespace i3
{
	namespace detail
	{

		template<class Seq, class InternalSeq, class Visitor, bool is_const>
		struct type_acceptor_impl<Seq, InternalSeq, Visitor, is_const, i_>
		{
			typedef		Seq					sequence_type;
			typedef		InternalSeq			internal_sequence_type;

//			typedef typename mpl::if_c<is_const, 
//				mpl::transform_view<Seq, add_const_to_acceptor_type<mpl::_> >, Seq>::type	sequence_type;
//			typedef typename mpl::if_c<is_const,
//				mpl::transform_view<InternalSeq, add_const_to_acceptor_type<mpl::_> >, Seq>::type  internal_sequence_type;
						
			typedef Visitor									visitor_type;

			typedef typename Visitor::result_type			result_type;
			typedef typename mpl::if_c<is_const, const void*, void*>::type	void_ptr;
			
					
			__forceinline static result_type call( int index, Visitor& f, void_ptr data)
			{
				switch(index)
				{
				
				#define pp_iteration_params_2 (4, (0, pp_dec(i_), "type_acceptor.h", type_acceptor_impl_call) )
				#include pp_iterate()
									
				default:
				break;	
				}
				return result_type();
			}		
		};

		template<class Seq, class Visitor>
		struct type_identity_acceptor_impl<Seq, Visitor, i_>
		{
//			typedef typename mpl::if_c<is_const, 
//				mpl::transform_view<Seq, add_const_to_acceptor_type<mpl::_> >, Seq>::type	sequence_type;

			typedef Seq										sequence_type;
			typedef Visitor									visitor_type;
			typedef typename Visitor::result_type			result_type;
				
			__forceinline static result_type call( int index, Visitor& f )
			{
				switch(index)
				{

				#define pp_iteration_params_2 (4, (0, pp_dec(i_), "type_acceptor.h", type_acceptor_impl_call_identity) )
				#include pp_iterate()

				default:
				break;	
				}
				return result_type();
			}	
			
		};
		
	}
}


#undef type_acceptor_impl_call_identity
#undef type_acceptor_impl_call

#undef i_

#elif		pp_iteration_depth() == 2

#define j_		pp_frame_iteration(2)			// 1부터 반복되어야함.. 

#if			pp_iteration_flags() == type_acceptor_impl_call

					case j_:
						{
							typedef typename mpl::at_c<sequence_type, j_>::type				check_type;
							typedef typename mpl::at_c<internal_sequence_type, j_>::type	target_type;
						
							typedef typename mpl::if_<i3::functor_can_be_called<visitor_type, check_type>, 
									type_acceptor_function<visitor_type, void_ptr, target_type>,
									type_acceptor_null_function<visitor_type, void_ptr> >::type	fun_type;
							
							return fun_type::visit( f, data);
						}
					break;

#elif			pp_iteration_flags() == type_acceptor_impl_call_identity					
		
					case j_:
						{
							typedef typename mpl::at_c<sequence_type, j_>::type		target_type;
											
							typedef i3::identity<target_type>						identity_type;


							typedef typename mpl::if_<i3::functor_can_be_called<visitor_type, identity_type >, 
								type_acceptor_identity_function<visitor_type, identity_type >,
								type_acceptor_null_identity_function<visitor_type> >::type	fun_type;

							return fun_type::visit( f );
						}
					break;		
						

#endif		// 	pp_iteration_flags() 

#undef  j_

#endif	// pp_iteration_depth() == 1 or 2

#endif // pp_is_iterating
