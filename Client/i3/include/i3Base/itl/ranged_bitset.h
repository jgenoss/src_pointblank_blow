#if !defined(pp_is_iterating)

#ifndef ranged_bitset_h
#define ranged_bitset_h

#define ranged_bit_pack_limit		40


#include "preprocessor/ext/pp_enum_params_1_with_a_partial_default.h"


namespace i3
{
	struct ranged_digit_size_tag;
	struct ranged_digit_pos_size_tag;
	struct ranged_digit_start_last_tag;
	
	template<int EnumVal, size_t DigitSize>
	struct ranged_digit_size
	{
		typedef ranged_digit_size_tag		argument_tag;
		static const int enum_value = EnumVal;
		static const size_t digit_size = DigitSize;
	};

	template<int EnumVal, size_t DigitPos, size_t DigitSize>
	struct ranged_digit_pos_size
	{
		typedef ranged_digit_pos_size_tag argument_tag;
		static const int enum_value = EnumVal;
		static const size_t digit_start_pos = DigitPos;
		static const size_t digit_last_pos = DigitPos + DigitSize - 1;
		static const size_t digit_size = DigitSize;
	};

	template<int EnumVal, size_t StartDigit, size_t LastDigit>
	struct ranged_digit_start_last
	{
		typedef ranged_digit_start_last_tag argument_tag;
		static const int enum_value = EnumVal;
		static const size_t digit_start_pos = StartDigit;
		static const size_t digit_last_pos = LastDigit;
		static const size_t digit_size = 1 + LastDigit - StartDigit;
	};

	struct ranged_digitpack_bit_tag;
	struct ranged_digitpack_dec_tag;
}

#include "detail/ranged_bitset_impl.h"

namespace i3
{
	template<pp_enum_params_1_with_a_partial_default(ranged_bit_pack_limit, 1, class RangedDigit, mpl::na) >
	struct ranged_bitpack
	{
		typedef ranged_digitpack_bit_tag													tag;
		typedef typename mpl::vector<pp_enum_params_1(ranged_bit_pack_limit, RangedDigit)>::type		input_vector_type;
		typedef typename ranged_bitset_imp::transform_pack_into_pos_size<input_vector_type>::type		pack_vector_type;
	};
	
	template<pp_enum_params_1_with_a_partial_default(ranged_bit_pack_limit, 1, class RangedDigit, mpl::na) >
	struct ranged_bitpack_rev : ranged_bitpack<pp_enum_params_1(ranged_bit_pack_limit, RangedDigit)>
	{
		typedef ranged_digitpack_bit_tag													tag;
		typedef ranged_bitpack<pp_enum_params_1(ranged_bit_pack_limit, RangedDigit)>		base_type;
		typedef typename ranged_bitset_imp::transform_pack_into_rev_order<typename base_type::pack_vector_type>::type pack_vector_type;
	};

	// 10진수 형태 ..
	template<pp_enum_params_1_with_a_partial_default(ranged_bit_pack_limit, 1, class RangedDigit, mpl::na) >
	struct ranged_decpack : ranged_bitpack<pp_enum_params_1(ranged_bit_pack_limit, RangedDigit)>
	{
		typedef ranged_digitpack_dec_tag													tag;
	};

	template<pp_enum_params_1_with_a_partial_default(ranged_bit_pack_limit, 1, class RangedDigit, mpl::na) >
	struct ranged_decpack_rev : ranged_decpack<pp_enum_params_1(ranged_bit_pack_limit, RangedDigit)>
	{
		typedef ranged_digitpack_dec_tag													tag;
		typedef ranged_decpack<pp_enum_params_1(ranged_bit_pack_limit, RangedDigit)>	base_type;
		typedef typename ranged_bitset_imp::transform_pack_into_rev_order<typename base_type::pack_vector_type>::type pack_vector_type;
	};
		
	template<int EnumVal, class IntType> __forceinline typename i3::enable_if< is_integral<IntType>,
	ranged_digitvalue_static<EnumVal, IntType> >::type	rbv(IntType& val)
	{
		return ranged_digitvalue_static<EnumVal, IntType>(val);
	}

	template<int EnumVal, class IntType> __forceinline typename i3::enable_if< is_integral<IntType>,
	const ranged_digitvalue_static<EnumVal, const IntType> >::type	rbv(const IntType& val)
	{
		return ranged_digitvalue_static<EnumVal, const IntType>(val);
	}
		
	template<class IntType> __forceinline typename i3::enable_if< is_integral<IntType>,
	ranged_digitvalue_dynamic<IntType> >::type	rbv(int enumVal, IntType& val)
	{
		return ranged_digitvalue_dynamic<IntType>(enumVal, val);
	}

	template<class IntType> __forceinline typename i3::enable_if< is_integral<IntType>,
	const ranged_digitvalue_dynamic<const IntType> >::type	rbv(int enumVal, const IntType& val)
	{
		return ranged_digitvalue_dynamic<const IntType>(enumVal, val);
	}
	
#define		ranged_bitset_create_cons			0x01
#define		ranged_bitset_create_paren			0x02
#define		ranged_bitset_create_extract		0x03

	template<class RangedDigitPack, class BlockType = unsigned long>
	class ranged_bitset : private find_ranged_bitset_base<RangedDigitPack, BlockType>::type
	{
	public:
		typedef ranged_bitset<RangedDigitPack, BlockType>							this_type;
		typedef typename find_ranged_bitset_base<RangedDigitPack, BlockType>::type	base_type;
		typedef typename RangedDigitPack::pack_vector_type							pack_vector_type;
		typedef BlockType															block_type;
		typedef typename base_type::maximum_element_type							maximum_element_type;
		typedef typename base_type::tag												tag;

		typedef safe_bool<this_type>					safe_bool_t;

		static const size_t number_element = mpl::size<pack_vector_type>::type::value;
		static const size_t maximum_digit_index = base_type::maximum_digit_index;
		static const size_t block_digit_size = base_type::block_digit_size;
		static const size_t number_block = base_type::number_block;
		static const size_t binary_size_value = base_type::binary_size_value;
		
		ranged_bitset() {}
		ranged_bitset(void* binData, size_t binSize)
		{
			I3ASSERT(binSize == binary_size_value);
			base_type::set_bin(binData);
		}

		ranged_bitset(const ranged_bitset& rhs) : base_type(rhs) {}

		// ..가변 생성자 지원...
		ranged_bitset(typename rb_imp::mk_arg<this_type, 0>::type p0) 
		{ 
			typedef typename rb_imp::mk_arg<this_type, 0>::type  P0;
			static const int EnumVal0 = rb_imp::get_enum<this_type, 0>::value;	
			rb_imp::setter<P0>.call<EnumVal0, this_type>(*this, p0);
		}

		ranged_bitset(typename rb_imp::mk_arg<this_type, 0>::type p0, 
					  typename rb_imp::mk_arg<this_type, 1>::type p1 )
		{
			typedef typename rb_imp::mk_arg<this_type, 0>::type  P0;
			static const int EnumVal0 = rb_imp::get_enum<this_type, 0>::value;	
			rb_imp::setter<P0>.call<EnumVal0, this_type>(*this, p0);

			typedef typename rb_imp::mk_arg<this_type, 1>::type  P1;
			static const int EnumVal1 = rb_imp::get_enum<this_type, 1>::value;	
			rb_imp::setter<P1>.call<EnumVal1, this_type>(*this, p1);
		}

#define pp_iteration_params_1	( 4, (3, ranged_bit_pack_limit, "ranged_bitset.h", ranged_bitset_create_cons ))	// 1단계 파일 반복..
#include pp_iterate()

	

		// 배정 연산자 대신에 () operator를 사용하여 통대입..
		ranged_bitset&	operator()(typename rb_imp::mk_arg<this_type, 0>::type p0)
		{
			typedef typename rb_imp::mk_arg<this_type, 0>::type  P0;
			static const int EnumVal0 = rb_imp::get_enum<this_type, 0>::value;
			rb_imp::setter<P0>.call<EnumVal0, this_type>(*this, p0);

			return *this;
		}

		ranged_bitset&	operator()(typename rb_imp::mk_arg<this_type, 0>::type p0,
			typename rb_imp::mk_arg<this_type, 1>::type p1)
		{
			typedef typename rb_imp::mk_arg<this_type, 0>::type  P0;
			static const int EnumVal0 = rb_imp::get_enum<this_type, 0>::value;
			rb_imp::setter<P0>.call<EnumVal0, this_type>(*this, p0);

			typedef typename rb_imp::mk_arg<this_type, 1>::type  P1;
			static const int EnumVal1 = rb_imp::get_enum<this_type, 1>::value;
			rb_imp::setter<P1>.call<EnumVal1, this_type>(*this, p1);

			return *this;
		}

#define pp_iteration_params_1	( 4, (3, ranged_bit_pack_limit, "ranged_bitset.h", ranged_bitset_create_paren ))	// 1단계 파일 반복..
#include pp_iterate()

		// extract 함수 가변 인수 지원...
		template<class T0>
		void	extract(T0& p0)
		{
			static const bool isConvert0 = rb_imp::is_convertible_to_T<this_type, 0, T0>::value;
			static const int EnumVal0 = rb_imp::get_enum<this_type, 0>::value;	
			rb_imp::get_extract<isConvert0>::call<T0, EnumVal0, this_type>(p0, *this);
				
		}

		template<class T0, class T1>
		void	extract(T0& p0, T1& p1)
		{
			static const bool isConvert0 = rb_imp::is_convertible_to_T<this_type, 0, T0>::value;
			static const int EnumVal0 = rb_imp::get_enum<this_type, 0>::value;
			rb_imp::get_extract<isConvert0>::call<T0, EnumVal0, this_type>(p0, *this);

			static const bool isConvert1 = rb_imp::is_convertible_to_T<this_type, 1, T1>::value;
			static const int EnumVal1 = rb_imp::get_enum<this_type, 1>::value;
			rb_imp::get_extract<isConvert1>::call<T1, EnumVal1, this_type>(p1, *this);
		}

#define pp_iteration_params_1	( 4, (3, ranged_bit_pack_limit, "ranged_bitset.h", ranged_bitset_create_extract ))	// 1단계 파일 반복..
#include pp_iterate()


		ranged_bitset& operator=(const ranged_bitset& rhs) { base_type::assign(rhs); return *this; }

		bool operator==(const ranged_bitset& rhs) const { return base_type::is_equal(rhs); }
		bool operator!=(const ranged_bitset& rhs) const { return !base_type::is_equal(rhs); }

		template<int EnumVal, class IntType> __forceinline
		ranged_bitset& operator<<(const ranged_digitvalue_static<EnumVal, IntType>& rhs)
		{
			return set<EnumVal>(rhs.get());
		}

		template<int EnumVal, class IntType> __forceinline
		ranged_bitset& operator>>(ranged_digitvalue_static<EnumVal, IntType>& rhs)
		{
			rhs.get() = get<EnumVal>();
			return *this;
		}
		
		template<class IntType> __forceinline
		ranged_bitset& operator<<(const ranged_digitvalue_dynamic<IntType>& rhs)
		{
			return set(	rhs.get_enum_value(), rhs.get_value());
		}

		template<class IntType> __forceinline
		ranged_bitset& operator>>(ranged_digitvalue_dynamic<IntType>& rhs)
		{
			rhs.get_value() = get(rhs.get_enum_value());
			return *this;
		}


		template<int EnumVal>
		typename ranged_bitset_imp::get_fitted_data_type<base_type, EnumVal>::type get() const
		{
			return base_type::get<EnumVal>();
		}

		template<int EnumVal, class IntType> typename i3::enable_if<is_integral<IntType>,
		this_type&>::type	set(IntType val)						// 정수타입이므로 상수참조 처리 하지 않습니다.
		{
			typedef typename ranged_bitset_imp::get_fitted_data_promoted_type<base_type, EnumVal>::type	fitted_promoted_type;
			static const fitted_promoted_type max_value = ranged_bitset_imp::get_data_type_maximum_value<base_type, EnumVal>::value;
			I3ASSERT(val <= max_value);
			base_type::set<EnumVal>(val);
			return *this;
		}
		

		template<int EnumVal, class IntType> 
		this_type& set(const ranged_digitvalue_static<EnumVal, IntType>& rhs)
		{
			return set<EnumVal>(rhs.get());
		}

		template<class IntType>
		this_type& set(const ranged_digitvalue_dynamic<IntType>& rhs)
		{
			return set(rhs.get_enum_value(), rhs.get_value());
		}
		
		template<class IntType> typename i3::enable_if<is_integral<IntType>,
		this_type&>::type set(int enumVal, IntType data_val)
		{
			ranged_bitset_imp::dynamic_setter<base_type, number_element>::call(*this, enumVal, data_val);
			return *this;
		}
		
		maximum_element_type get(int enumVal) const
		{
			return ranged_bitset_imp::dynamic_getter<base_type, number_element>::call(*this, enumVal);
		}
		
	
		
		this_type&	set_bin(const void* binData, size_t binSize)
		{
			I3ASSERT(binSize == binary_size_value);
			base_type::set_bin(binData);
			return *this;
		}

		void	clear() { base_type::clear(); }
		
		BlockType*				data() { return base_type::data(); }
		BlockType*				data() const { return base_type::data(); }

		static size_t			static_size() { return base_type::static_size(); }
		static size_t			static_bin_size() { return base_type::static_bin_size(); }
		
		// 배열 접근 지원..
		maximum_element_type	operator[](int enumVal) const { return get(enumVal); }

		ranged_bitset_imp::reference<this_type> operator[](int enumVal)
		{
			return ranged_bitset_imp::reference<this_type>(*this, enumVal);
		}

		bool			empty() const { return base_type::empty(); }	

		operator typename safe_bool_t::type() const { return safe_bool_t::call( !empty() ); }

		bool			operator!() const { return base_type::empty(); }


	};

	
}

#undef		ranged_bitset_create_extract
#undef		ranged_bitset_create_paren
#undef		ranged_bitset_create_cons


#endif		// ranged_bitset_h

#else		// pp_is_iterating

#define i_				pp_frame_iteration(1)






#if			pp_iteration_flags() == ranged_bitset_create_cons

#define repeat_ranged_bitset_cons_arg(z, n, d) \
		pp_comma_if(n) typename rb_imp::mk_arg<this_type, n>::type pp_cat(p, n) 

		ranged_bitset( pp_repeat( i_, repeat_ranged_bitset_cons_arg, ~) )
		{

#undef repeat_ranged_bitset_cons_arg

#define repeat_ranged_bitset_cons_imp(z, n, d) \
			typedef typename rb_imp::mk_arg<this_type, n>::type  pp_cat(P, n); \
			static const int pp_cat(EnumVal, n) = rb_imp::get_enum<this_type, n>::value; \
			rb_imp::setter<pp_cat(P, n)>::call<pp_cat(EnumVal, n), this_type>(*this, pp_cat(p, n) );

		
		pp_repeat(i_, repeat_ranged_bitset_cons_imp, ~);

#undef repeat_ranged_bitset_cons_imp

		}
		
#elif		pp_iteration_flags() == ranged_bitset_create_paren
		
#define repeat_ranged_bitset_cons_arg(z, n, d) \
		pp_comma_if(n) typename rb_imp::mk_arg<this_type, n>::type pp_cat(p, n) 

		ranged_bitset&	operator()( pp_repeat(i_, repeat_ranged_bitset_cons_arg, ~) )
		{
#undef repeat_ranged_bitset_cons_arg

#define repeat_ranged_bitset_cons_imp(z, n, d) \
			typedef typename rb_imp::mk_arg<this_type, n>::type  pp_cat(P, n); \
			static const int pp_cat(EnumVal, n) = rb_imp::get_enum<this_type, n>::value; \
			rb_imp::setter<pp_cat(P, n)>::call<pp_cat(EnumVal, n), this_type>(*this, pp_cat(p, n) );
			
			pp_repeat(i_, repeat_ranged_bitset_cons_imp, ~);

#undef repeat_ranged_bitset_cons_imp
		
			return *this;
		}

#elif		pp_iteration_flags() == ranged_bitset_create_extract
		

		template< pp_enum_params(i_, class T) >
		void	extract(pp_enum_binary_params(i_, T, &p) )
		{

#define repeat_ranged_bitset_extract_imp(z, n, d) \
			static const bool pp_cat(isConvert, n) = rb_imp::is_convertible_to_T<this_type, n, pp_cat(T, n)>::value;\
			static const int pp_cat(EnumVal, n) = rb_imp::get_enum<this_type, n>::value; \
			rb_imp::get_extract<pp_cat(isConvert, n)>::call<pp_cat(T, n), pp_cat(EnumVal, n), this_type>(pp_cat(p, n), *this);
						
			pp_repeat(i_, repeat_ranged_bitset_extract_imp, ~);

#undef repeat_ranged_bitset_extract_imp
		}
		

#endif		// pp_iteration_flags()

#undef i_	

#endif		// pp_is_iterating
