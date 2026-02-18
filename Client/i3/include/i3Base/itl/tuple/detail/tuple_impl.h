#if !defined(pp_is_iterating)

#ifndef tuple_impl_h
#define tuple_impl_h

#include "../../type_traits/is_function.h"

namespace i3
{

#define tuple_impl0_na_constructor_implementation	0x00
#define tuple_impl_implementation					0x01
#define tuple_impl_constructor_implementation		0x02
#define tuple_impl_na_constructor_implementation    0x03	


	namespace detail
	{	
		// 0 이외의 나머지는 디폴트함수인자적용을 위해서 인덱스 특화 + pp를 요구함..
		template<class T> 
		class non_storable_type {	non_storable_type();	};
		
		template<class T>
		struct wrap_non_storable_type { typedef typename mpl::if_<is_function<T>, non_storable_type<T>, T>::type type; };
		template<> 
		struct wrap_non_storable_type<void> { typedef non_storable_type<void> type; };


		template<class Seq>
		class tuple_impl<Seq, 0>		// 말단..0
		{
		public:
			typedef typename mpl::at_c<Seq, 0>::type							value_type;
			typedef typename wrap_non_storable_type<value_type>::type			stored_value_type;

			typedef typename make_reference_result<stored_value_type>::type			reference_type;
			typedef typename make_const_reference_result<stored_value_type>::type	const_reference_type;

			tuple_impl() {}
			template<class P0>
			tuple_impl(P0& p0) : m_value(p0) {}
			
			#define		pp_iteration_params_1 (4, (1, pp_dec(tuple_limit), "tuple_impl.h", tuple_impl0_na_constructor_implementation))
			#include	pp_iterate()

			tuple_impl(const tuple_impl& rhs) : m_value(rhs.m_value) {}

			template<class Seq2>
			tuple_impl(const tuple_impl<Seq2, 0>& rhs) : m_value(rhs.m_value) {}

		protected:
			tuple_impl&	get_this() { return *this; }
			const tuple_impl& get_this() const { return *this; }
				
			void	assign_impl(const tuple_impl& rhs) { m_value = rhs.m_value; }  

			// 이 루틴 역시 진행이 안된다면, i_를 그냥 Index2템플릿으로 바꾼다...
			template<class Seq2>
			void	assign_impl(const tuple_impl<Seq2, 0>& rhs) { m_value = rhs.m_value; }
			
			template<class U1, class U2>
			void	assign_impl(const i3::pair<U1, U2>& rhs) { m_value = rhs.first; }

			reference_type get_data() { return m_value; }
			const_reference_type get_data() const { return m_value; }

		private:
			
			stored_value_type m_value;

			template<class, int> friend class tuple_impl;
			template< pp_enum_datas(tuple_limit, class)> friend class tuple;
			tuple_impl& operator=(const tuple_impl&);		// 사용하지 않는다.

		};
		
		#define		pp_iteration_params_1	(4, (1, pp_dec(tuple_limit), "tuple_impl.h", tuple_impl_implementation))				// 1단계 파일반복..
		#include	pp_iterate()

	}


#undef tuple_impl_na_constructor_implementation
#undef tuple_impl_constructor_implementation
#undef tuple_impl_implementation
#undef tuple_impl0_na_constructor_implementation

}


#endif // tuple_impl_h


#else // pp_is_iterating

#if			pp_iteration_depth() == 1

#define		i_		pp_frame_iteration(1)

#if			pp_iteration_flags() == tuple_impl0_na_constructor_implementation

			template<class P0>
			tuple_impl(P0& p0, pp_enum_datas(i_, const swallow_copy&) ) : m_value(p0) {}

#elif		pp_iteration_flags() == tuple_impl_implementation
			
#define		num_p_	pp_inc(i_)
#define		num_na_	pp_sub(tuple_limit, num_p_)

			template<class Seq>
			class tuple_impl<Seq, i_> : public tuple_impl<Seq, pp_dec(i_)>
			{
			public:
				typedef tuple_impl<Seq, pp_dec(i_)>						base_type;
				typedef typename mpl::at_c<Seq, i_>::type				value_type;
				typedef typename wrap_non_storable_type<value_type>::type			stored_value_type;

				typedef typename make_reference_result<stored_value_type>::type			reference_type;
				typedef typename make_const_reference_result<stored_value_type>::type	const_reference_type;

				tuple_impl() {}

// 이 사이에 많은 디폴트 구현들이 존재할수 있음..모두 베이스로 올릴 것..
				#define		pp_iteration_params_2	(4, (1, i_, "tuple_impl.h", tuple_impl_constructor_implementation))				// 2단계 파일반복..
				#include	pp_iterate()
//
				template<pp_enum_params(num_p_, class P)>
				tuple_impl(pp_enum_params_cat_params(num_p_, P, &, p) ) : base_type( pp_enum_params(i_, p) ), m_value( pp_cat(p, i_) ) {}		// fitted..
				
				#if			num_na_ > 0
				#define		pp_iteration_params_2	(4, (1, num_na_ , "tuple_impl.h", tuple_impl_na_constructor_implementation))
				#include	pp_iterate()
				#endif

				tuple_impl(const tuple_impl& rhs) : base_type(rhs.base_type::get_this()), m_value(rhs.m_value) {}

				
				template<class Seq2, int Index2>
				tuple_impl(const tuple_impl<Seq2, Index2>& rhs) : base_type(rhs) {}		// 그대로 넘김..(맞는게 나올때까지..)
				template<class Seq2>
				tuple_impl(const tuple_impl<Seq2, i_>& rhs) : base_type(rhs.tuple_impl<Seq2, i_>::base_type::get_this()), m_value(rhs.m_value) {}



			protected:

				tuple_impl&	get_this() { return *this; }
				const tuple_impl& get_this() const { return *this; }
				
				void	assign_impl(const tuple_impl& rhs) { base_type::assign_impl(rhs.base_type::get_this()); m_value = rhs.m_value; }  

				template<class Seq2>
				void	assign_impl(const tuple_impl<Seq2, i_>& rhs) 
				{ base_type::assign_impl(rhs.tuple_impl<Seq2, i_>::base_type::get_this()); m_value = rhs.m_value; }
				
				template<class U1, class U2>
				void	assign_impl(const i3::pair<U1, U2>& rhs) {	base_type::get_data() = rhs.first;	m_value = rhs.second; }


				reference_type get_data() { return m_value; }
				const_reference_type get_data() const { return m_value; }

			private:
				stored_value_type m_value;

				template<class, int> friend class tuple_impl;
				template< pp_enum_datas(tuple_limit, class)> friend class tuple;
				tuple_impl& operator=(const tuple_impl&);		// 사용하지 않는다.
			};

#undef		num_na_
#undef		num_p_

#endif		// pp_iteration_flags()

#undef		i_

#elif	pp_iteration_depth() == 2

#define		j_		pp_frame_iteration(2)			

#if			pp_iteration_flags() == tuple_impl_constructor_implementation

				template<pp_enum_params(j_, class P)>
				tuple_impl(pp_enum_params_cat_params(j_, P, &, p) ) : base_type( pp_enum_params(j_, p) ) {}		// to base class..

#elif		pp_iteration_flags() == tuple_impl_na_constructor_implementation
				
				template<pp_enum_params(num_p_, class P)>
				tuple_impl(pp_enum_params_cat_params(num_p_, P, &, p), pp_enum_datas(j_, const swallow_copy&) ) : base_type( pp_enum_params(i_, p) ), m_value( pp_cat(p, i_) ) {}		// fitted..
				

#endif		// pp_iteration_flags()

#undef		j_

#endif // pp_iteration_depth() == 1 / 2

#endif // pp_is_iterating

