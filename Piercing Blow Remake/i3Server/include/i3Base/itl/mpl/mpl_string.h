#pragma once

#include "mpl_limit.h"
#include "mpl_if.h"
#include "mpl_char.h"
#include "mpl_copy.h"
#include "mpl_size.h"
#include "mpl_empty.h"
#include "mpl_size_t.h"
#include "mpl_begin_end.h"
#include "mpl_joint_view.h"
#include "mpl_insert_range.h"
#include "mpl_back_inserter.h"
#include "mpl_front_inserter.h"
#include "mpl_iterator_range.h"
#include <iterator>

namespace i3
{
	namespace mpl
	{
		#define mpl_string_max_params      pp_div(pp_add( mpl_string_limit, 3), 4)
		
		#define mpl_multichar_length(c)    (std::size_t)((c<CHAR_MIN) ? 4 : ((c>0xffffff)+(c>0xffff)+(c>0xff)+1))
		#define mpl_multichar_at(c,i)      (char)(0xff&((unsigned)(c)>>(8*(mpl_multichar_length(c)-(std::size_t)(i)-1))))
        #define mpl_multichar_push_back(c,i)   (((unsigned)(c)<<8)|(unsigned char)(i))
        #define mpl_multichar_push_front(c,i)  ((((unsigned char)(i))<<(mpl_multichar_length(c)*8))|(unsigned)(c))
        #define mpl_multichar_pop_back(c)    ((unsigned)(c)>>8)
        #define mpl_multichar_pop_front(c)   (((1<<((mpl_multichar_length(c)-1)*8))-1)&(unsigned)(c))

		struct string_tag;
		struct string_iterator_tag;

		template<pp_enum_params_with_a_default(mpl_string_max_params, int C, 0)>
		struct string;

		template<class Seq, int I, int J> struct string_iterator;
		template<class Seq> struct sequence_tag;
		template<class Tag> struct size_impl;

		template<>
		struct size_impl<mpl::string_tag>
		{
			template<class Seq>
			struct apply;

			#define M0(z, n, data) + mpl_multichar_length(pp_cat(C,n))

			#define M1(z, n, data)	template<pp_enum_params_z(z, n, int C)>                             \
									struct apply<mpl::string<pp_enum_params_z(z, n, C)> >               \
									: mpl::size_t<(0 pp_repeat_ ## z(n, M0, ~))> {};

			pp_repeat_from_to(1, pp_inc(mpl_string_max_params), M1, ~)
			#undef M0
			#undef M1
		};

		template<>
		struct size_impl<mpl::string_tag>::apply<mpl::string<> > : mpl::size_t<0> {};

		template<class Tag>
		struct begin_impl;

		template<>
		struct begin_impl<mpl::string_tag>
		{
			template<class Seq> struct apply {	typedef mpl::string_iterator<Seq, 0, 0> type;	};
		};
		
		template<class Tag>
		struct end_impl;

		template<>
		struct end_impl<mpl::string_tag>
		{
			template<class Seq>	struct apply;

			#define M0(z,n,data)	template<pp_enum_params_z(z, n, int C)>                          \
									struct apply<mpl::string<pp_enum_params_z(z, n, C)> >            \
									{  typedef mpl::string_iterator<mpl::string<pp_enum_params_z(z, n, C)>, n, 0> type;  };

			pp_repeat_from_to(1, pp_inc(mpl_string_max_params), M0, ~)
			#undef M0
		};

		template<>
		struct end_impl<mpl::string_tag>::apply<mpl::string<> >
		{
			typedef mpl::string_iterator<mpl::string<>, 0, 0> type;
		};

		template<class Tag>
		struct push_back_impl;

		template<>
		struct push_back_impl<mpl::string_tag>
		{
			template<class Seq, class Value, bool B = (4==mpl_multichar_length(Seq::back_))>
			struct apply
			{
				compile_assert_msg_type( (mpl_string_limit != mpl::size<Seq>::type::value), PUSH_BACK_FAILED_MPL_STRING_IS_FULL, (Seq) );
				// If the above assertion didn't fire, then the string is sparse.
				// Repack the string and retry the push_back
				typedef typename mpl::push_back< typename mpl::copy< Seq, mpl::back_inserter<mpl::string<> > >::type, 
												Value >::type	type;
			};

			template<class Value>
			struct apply<mpl::string<>, Value, false>
			{
				typedef mpl::string<(char)Value::value> type;
			};

			#define M0(z,n,data)                                                                        \
			template<pp_enum_params_z(z, n, int C), class Value>										\
			struct apply<mpl::string<pp_enum_params_z(z, n, C)>, Value, false>							\
			{                                                                                           \
				typedef                                                                                 \
					mpl::string<                                                                        \
						pp_enum_params_z(z, pp_dec(n), C)												\
						pp_comma_if(pp_dec(n))															\
						((unsigned)pp_cat(C,pp_dec(n))>0xffffff)										\
						?pp_cat(C,pp_dec(n))															\
						:mpl_multichar_push_back(pp_cat(C,pp_dec(n)), Value::value)						\
					  , ((unsigned)pp_cat(C,pp_dec(n))>0xffffff)										\
						?(char)Value::value                                                             \
						:0                                                                              \
					>                                                                                   \
				type;                                                                                   \
			};

			pp_repeat_from_to(1, mpl_string_max_params, M0, ~)
			#undef M0

			template<pp_enum_params(mpl_string_max_params, int C), class Value>
			struct apply<mpl::string<pp_enum_params(mpl_string_max_params, C)>, Value, false>
			{
				typedef
					mpl::string<
						pp_enum_params(pp_dec(mpl_string_max_params), C)
					  , mpl_multichar_push_back(pp_cat(C,pp_dec(mpl_string_max_params)), Value::value)
					>
				type;
			};
		};

		template<class Tag>
		struct has_push_back_impl;

		template<>
		struct has_push_back_impl<mpl::string_tag>
		{
			template<class Seq> struct apply : mpl::true_ {};
		};

		template<class Tag>
		struct pop_back_impl;

		template<>
		struct pop_back_impl<mpl::string_tag>
		{
			template<class Seq>
			struct apply;

			#define M0(z,n,data)																			\
			template<pp_enum_params_z(z, n, int C)>															\
			struct apply<mpl::string<pp_enum_params_z(z, n, C)> >											\
			{																								\
				compile_assert_msg_type((C0 != 0), POP_BACK_FAILED_MPL_STRING_IS_EMPTY, (mpl::string<>));	\
				typedef																						\
					mpl::string<																			\
						pp_enum_params_z(z, pp_dec(n), C)													\
						pp_comma_if(pp_dec(n))																\
						mpl_multichar_pop_back(pp_cat(C,pp_dec(n)))											\
					>																						\
				type;																						\
			};

			pp_repeat_from_to(1, pp_inc(mpl_string_max_params), M0, ~)
			#undef M0
		};
		
		template<class Tag>
		struct has_pop_back_impl;

		template<>
		struct has_pop_back_impl<mpl::string_tag>
		{
			template<class Seq> struct apply : mpl::true_ {};
		};
		
		template<class Tag>
		struct push_front_impl;

		template<>
		struct push_front_impl<mpl::string_tag>
		{
			template<class Seq, class Value, bool B = (4==mpl_multichar_length(Seq::front_))>
			struct apply
			{
				compile_assert_msg_type( (mpl_string_limit != mpl::size<Seq>::type::value), PUSH_FRONT_FAILED_MPL_STRING_IS_FULL, (Seq) );
				// If the above assertion didn't fire, then the string is sparse.
				// Repack the string and retry the push_front.
				typedef typename mpl::push_front<	typename mpl::reverse_copy< Seq, mpl::front_inserter<string<> > >::type, 
													Value>::type	type;
			};

			template<class Value>
			struct apply<mpl::string<>, Value, false>
			{
				typedef mpl::string<(char)Value::value> type;
			};
			
			#define M0(z,n,data)                                                                        \
			template<pp_enum_params_z(z, n, int C), class Value>										\
			struct apply<mpl::string<pp_enum_params_z(z, n, C)>, Value, true>							\
			{                                                                                           \
				typedef                                                                                 \
					mpl::string<                                                                        \
						(char)Value::value                                                              \
						pp_enum_trailing_params_z(z, n, C)												\
					>                                                                                   \
				type;                                                                                   \
			};

			pp_repeat_from_to(1, mpl_string_max_params, M0, ~)
			#undef M0

			template<pp_enum_params(mpl_string_max_params, int C), class Value>
			struct apply<mpl::string<pp_enum_params(mpl_string_max_params, C)>, Value, false>
			{
				typedef mpl::string<mpl_multichar_push_front(C0, Value::value), 
									pp_enum_shifted_params(mpl_string_max_params, C) >	type0;
				typedef type0 type;
			};
		};

		template<class Tag>
		struct has_push_front_impl;

		template<>
		struct has_push_front_impl<mpl::string_tag>
		{
			template<class Seq>	struct apply : mpl::true_ {};
		};

		template<class Tag>
		struct pop_front_impl;

		template<>
		struct pop_front_impl<mpl::string_tag>
		{
			template<class Seq, bool B = (1==mpl_multichar_length(Seq::front_))>
			struct apply;

			#define M0(z,n,data)																			\
			template<pp_enum_params_z(z, n, int C)>															\
			struct apply<mpl::string<pp_enum_params_z(z, n, C)>, true>										\
			{																								\
				compile_assert_msg_type((C0 != 0), POP_FRONT_FAILED_MPL_STRING_IS_EMPTY, (mpl::string<>));	\
				typedef																						\
					mpl::string<pp_enum_shifted_params_z(z, n, C)>											\
				type;																						\
			};

			pp_repeat_from_to(1, mpl_string_max_params, M0, ~)
			#undef M0

			template<pp_enum_params(mpl_string_max_params, int C)>
			struct apply<mpl::string<pp_enum_params(mpl_string_max_params, C)>, false>
			{
				typedef mpl::string<mpl_multichar_pop_front(C0), pp_enum_shifted_params(mpl_string_max_params, C) > type;
			};
		};

		template<class Tag>
		struct has_pop_front_impl;

		template<>
		struct has_pop_front_impl<mpl::string_tag>
		{
			template<class Seq> struct apply : mpl::true_ {};
		};
		
		template<class Tag>
		struct insert_range_impl;

		template<>
		struct insert_range_impl<mpl::string_tag>
		{
			template<class Seq, class Pos, class Range>
			struct apply : mpl::copy< mpl::joint_view<	mpl::iterator_range<mpl::string_iterator<Seq, 0, 0>, Pos>, 
														mpl::joint_view<Range, mpl::iterator_range<Pos, typename mpl::end<Seq>::type> >	>, 
									mpl::back_inserter<mpl::string<> > >
			{};
		};

		template<class Tag>
		struct insert_impl;

		template<>
		struct insert_impl<mpl::string_tag>
		{
			template<class Seq, class Pos, class Value>
			struct apply : mpl::insert_range<Seq, Pos, mpl::string<(char)Value::value> > {};
		};

		template<class Tag>
		struct erase_impl;

		template<>
		struct erase_impl<mpl::string_tag>
		{
			template<class Seq, class First, class Last>
			struct apply
			  : mpl::copy<
					mpl::joint_view<
						mpl::iterator_range<
							mpl::string_iterator<Seq, 0, 0>
						  , First
						>
					  , mpl::iterator_range<
							typename mpl::if_na<Last, typename mpl::next<First>::type>::type
						  , typename mpl::end<Seq>::type
						>
					>
				  , mpl::back_inserter<mpl::string<> >
				>
			{};
		};

		template<class Tag>
		struct clear_impl;

		template<>
		struct clear_impl<mpl::string_tag>
		{
			template<class> struct apply {	typedef mpl::string<> type;	};
		};

		#define M0(z, n, data)																				\
		template<pp_enum_params( mpl_string_max_params, int C), int J>										\
		struct string_iterator<mpl::string<pp_enum_params(mpl_string_max_params, C)>, n, J>					\
		{																									\
			enum { eomc_ = (mpl_multichar_length(pp_cat(C, n)) == J + 1) };									\
			typedef mpl::string<pp_enum_params(mpl_string_max_params, C)> string;							\
			typedef std::bidirectional_iterator_tag category;												\
			typedef																							\
				mpl::string_iterator<string, n + eomc_, eomc_ ? 0 : J + 1>									\
			next;																							\
			typedef																							\
				mpl::string_iterator<string, n, J - 1>														\
			prior;																							\
			typedef mpl::char_<mpl_multichar_at(pp_cat(C, n), J)> type;										\
		};																									\
		template<pp_enum_params(mpl_string_max_params, int C)>												\
		struct string_iterator<mpl::string<pp_enum_params(mpl_string_max_params, C)>, n, 0>					\
		{																									\
			enum { eomc_ = (mpl_multichar_length(pp_cat(C, n)) == 1) };										\
			typedef mpl::string<pp_enum_params(mpl_string_max_params, C)> string;							\
			typedef std::bidirectional_iterator_tag category;												\
			typedef																							\
				mpl::string_iterator<string, n + eomc_, !eomc_>												\
			next;																							\
			typedef																							\
				mpl::string_iterator<																		\
					string																					\
				  , n - 1																					\
				  , mpl_multichar_length(pp_cat(C, pp_dec(n))) - 1											\
				>																							\
			prior;																							\
			typedef mpl::char_<mpl_multichar_at(pp_cat(C, n), 0)> type;										\
		};

		pp_repeat(mpl_string_max_params, M0, ~)
		#undef M0
		

		template<pp_enum_params(mpl_string_max_params, int C)>
		struct string
		{
			/// INTERNAL ONLY
			enum
			{
				front_  = C0
			  , back_   = pp_cat(C, pp_dec(mpl_string_max_params))
			};

			typedef char        value_type;
			typedef string      type;
			typedef string_tag  tag;
		};

		namespace aux_
		{
			template<class It, class End> struct next_unless : mpl::next<It> {};
			template<class End> struct next_unless<End, End> {	typedef End type;	};
			template<class It, class End> struct deref_unless : mpl::deref<It> {};
			template<class End> struct deref_unless<End, End> {	typedef mpl::char_<'\0'> type;	};
		}
		
		template<class Seq>
		struct c_str
		{
			typedef typename mpl::end<Seq>::type iend;
			typedef typename mpl::begin<Seq>::type i0;
			#define M0(z, n, data)  typedef typename mpl::aux_::next_unless<pp_cat(i, n), iend>::type   pp_cat(i, pp_inc(n));
			pp_repeat(mpl_string_limit, M0, ~)
			#undef M0
			typedef c_str type;
			static typename Seq::value_type const value[mpl_string_limit+1];
		};

		template<class Seq>
		typename Seq::value_type const c_str<Seq>::value[mpl_string_limit+1] =
		{
			#define M0(z, n, data) mpl::aux_::deref_unless<pp_cat(i, n), iend>::type::value,
			pp_repeat(mpl_string_limit, M0, ~)
			#undef M0
			'\0'
		};

	}
}
