#if !defined(pp_is_iterating)

#ifndef ranged_bitset_impl_h
#define ranged_bitset_impl_h


#include "../mpl/mpl_max_element.h" 
#include "../mpl/mpl_fold.h"
#include "../mpl/mpl_push_back.h"
#include "../mpl/mpl_back.h"

namespace i3
{

	struct ranged_digit_invalid
	{
		static const int enum_value = -1;
		static const size_t digit_start_pos = 0;
		static const size_t digit_last_pos = 0;
		static const size_t digit_size = 0;
	};

	namespace ranged_bitset_imp
	{
		template<int EnumVal>
		struct pred_find_enum_value
		{
			template<class T>
			struct apply : mpl::bool_< T::enum_value == EnumVal > {};
		};

		template<class BitSet, int EnumVal>
		struct find_ranged_digit
		{
			typedef typename BitSet::pack_vector_type			PackVector;

			typedef typename mpl::find_if<PackVector, pred_find_enum_value<EnumVal> >::type found_iterator;
			typedef typename mpl::end<PackVector>::type										end_iterator;

			typedef typename mpl::eval_if< is_same<found_iterator, end_iterator>,
				mpl::identity<ranged_digit_invalid>,
				mpl::deref<found_iterator> >::type type;
		};

//		
		template<class Tag, class BlockType>
		struct get_block_digit_size
		{
			static const size_t value = sizeof(BlockType) * 8;
		};

		template<class BlockType>
		struct get_block_digit_size<ranged_digitpack_dec_tag, BlockType>
		{
			static const size_t size_of_block = sizeof(BlockType);

			static const size_t value = 
				(size_of_block == 1) ? 2 :				// unsigned char에 비교시...두 자리까지 여유..
				(size_of_block == 2) ? 4 :				// unsigned short에 비교시 ... 네 자리까지 여유..
				(size_of_block == 4) ? 9 :				// unsigned long에 비교시....  9 자리까지..
				(size_of_block == 8) ? 19 : 0;			// unsigned long long 일때 .... 19자리까지..

			compile_assert((value != 0));
		};


		template<class Tag, size_t DigitSize>
		struct get_fitted_data_size
		{
			static const size_t value =
				(DigitSize == 0) ? size_t(0) :
				(DigitSize <= 8) ? 8 :
				(DigitSize <= 16) ? 16 :
				(DigitSize <= 32) ? 32 :
				(DigitSize <= 64) ? 64 :
				size_t(0);
		};

		// 10진수 스타일 처리..
		template<size_t DigitSize>
		struct get_fitted_data_size< ranged_digitpack_dec_tag, DigitSize>
		{
			static const size_t value =
				(DigitSize == 0) ? size_t(0) :
				(DigitSize <= 2) ? 8 :					// unsigned char에 비교시...두 자리까지 여유..
				(DigitSize <= 4) ? 16 :					// unsigned short에 비교시 ... 네 자리까지 여유..
				(DigitSize <= 9) ? 32 :					// unsigned long에 비교시....  9 자리까지..
				(DigitSize <= 19) ? 64 :				// unsigned long long 일때 .... 19자리까지..
				size_t(0);
		};
		
		template<class BitSet, int EnumVal>
		struct get_fitted_data_type
		{
			typedef typename mpl::tag<BitSet>::type							tag;

			typedef typename find_ranged_digit<BitSet, EnumVal>::type	ranged_type;

			typedef get_fitted_data_size<tag, ranged_type::digit_size>  fitted_data_size_type;

			static const size_t fitted_size = fitted_data_size_type::value;

			typedef mpl::size_t<fitted_size>			fitted_size_type;
			typedef typename BitSet::block_type			BlockType;
			typedef typename i3::make_unsigned<BlockType>::type		default_type;

			typedef typename mpl::multi_if< is_same<fitted_size_type, mpl::size_t<8> >, unsigned char,
				is_same<fitted_size_type, mpl::size_t<16> >, unsigned short,
				is_same<fitted_size_type, mpl::size_t<32> >, unsigned long,
				is_same<fitted_size_type, mpl::size_t<64> >, unsigned long long, default_type>::type type;
		};

		struct less_element_for_max_digit_pos
		{
			template<class T1, class T2>
			struct apply : mpl::bool_ < (T1::digit_last_pos < T2::digit_last_pos) > {};
		};

		template<class BitSet>
		struct find_maximum_digit_index
		{
			typedef typename BitSet::pack_vector_type		PackVector;

			typedef typename mpl::max_element<PackVector, less_element_for_max_digit_pos>::type found_iterator;
			typedef typename mpl::deref<found_iterator>::type	result_max_digit_type;
			static const size_t value = result_max_digit_type::digit_last_pos;
		};

		struct less_element_for_max_digit_size
		{
			template<class T1, class T2>
			struct apply : mpl::bool_ < (T1::digit_size < T2::digit_size) > {};
		};

		template<class BitSet>
		struct get_maximum_element_type
		{
			typedef typename BitSet::pack_vector_type		PackVector;
			typedef typename mpl::max_element<PackVector, less_element_for_max_digit_size>::type found_iterator;
			typedef typename mpl::deref<found_iterator>::type	result_max_digit_size_type;
			
			typedef typename mpl::tag<BitSet>::type			tag;
		
			typedef get_fitted_data_size<tag, result_max_digit_size_type::digit_size>  fitted_data_size_type;
			
			static const size_t fitted_size = fitted_data_size_type::value;

			typedef mpl::size_t<fitted_size>	fitted_size_type;

			typedef typename mpl::multi_if< is_same<fitted_size_type, mpl::size_t<8> >, unsigned char,
				is_same<fitted_size_type, mpl::size_t<16> >, unsigned short,
				is_same<fitted_size_type, mpl::size_t<32> >, unsigned long,
				is_same<fitted_size_type, mpl::size_t<64> >, unsigned long long, unsigned long>::type type;
			
		};


		template<class BitSet, int EnumVal>
		struct get_fitted_data_promoted_type
		{
			typedef typename ranged_bitset_imp::get_fitted_data_type<BitSet, EnumVal>::type		fitted_data_type;
			typedef typename BitSet::block_type						BlockType;
			typedef typename i3::make_unsigned<BlockType>::type		default_type;

			typedef typename mpl::multi_if < is_same< fitted_data_type, unsigned char>, unsigned short,
				is_same< fitted_data_type, unsigned short>, unsigned long,
				is_same< fitted_data_type, unsigned long>, unsigned long long,
				is_same< fitted_data_type, unsigned long long>, unsigned long long, default_type>::type type;
		};

		namespace detail
		{
			template<class Tag, class RangedBit, class DataType>
			struct get_data_type_maximum_value
			{
				static const DataType value = DataType(1 << RangedBit::digit_size) - 1;
			};

			template<class RangedBit, class DataType >
			struct get_data_type_maximum_value<ranged_digitpack_dec_tag, RangedBit, DataType>
			{
				static const DataType value = make_dec_mask<DataType, RangedBit::digit_size>::value - 1;
			};

		}

		template<class BitSet, int EnumVal>
		struct get_data_type_maximum_value
		{
			typedef typename mpl::tag<BitSet>::type										tag;
			typedef typename find_ranged_digit<BitSet, EnumVal>::type	ranged_type;
			typedef typename get_fitted_data_promoted_type<BitSet, EnumVal>::type		maximum_value_type;

			static const maximum_value_type value = detail::get_data_type_maximum_value<tag, 
				ranged_type, maximum_value_type>::value;
		};


		template<class BlockType, size_t beg_pos, size_t last_pos>
		struct make_mask_data
		{
			static const BlockType ltrim_val = BlockType((1 << (last_pos + 1)) - 1);
			static const BlockType rrem_val = BlockType((1 << beg_pos) - 1);
			static const BlockType value = ltrim_val ^ rrem_val;		// 왼쪽과 오른쪽의 벗어난 범위를 0으로 만들어둔다..
		};

		template<class BlockType, size_t decPos>						// 0이면 1, 1이면 10, 2이면 100 ... 이런식으로 전개되어야함...
		struct make_dec_mask
		{
			static const  BlockType value = 10 * make_dec_mask<BlockType, decPos - 1>::value;
		};

		template<class BlockType>
		struct make_dec_mask<BlockType, 0>
		{
			static const BlockType value = 1;
		};

		template<class Tag>
		struct get_invalid
		{
			template<class BitSet, int EnumVal, class BlockType> static inline
				typename get_fitted_data_type<BitSet, EnumVal>::type call(const BlockType& m_Data)
			{
				I3PRINTLOG(I3LOG_WARN, "invalid ranged_bit::get() : enumVal = %d", EnumVal);

				typedef typename get_fitted_data_type<BitSet, EnumVal>::type	result_data_type;
				return result_data_type();
			}
		};

		template<class Tag>
		struct get_unique
		{
			template<class BitSet, int EnumVal, class BlockType> static inline
				typename get_fitted_data_type<BitSet, EnumVal>::type call(const BlockType& m_Data)
			{
				typedef typename get_fitted_data_type<BitSet, EnumVal>::type	result_data_type;
				typedef typename find_ranged_digit<BitSet, EnumVal>::type			found_ranged_digit;

				static const size_t digit_start_pos_remain = found_ranged_digit::digit_start_pos;
				static const size_t digit_last_pos_remain = found_ranged_digit::digit_last_pos;

				static const BlockType mask_data = make_mask_data<BlockType, digit_start_pos_remain, digit_last_pos_remain>::value;
				result_data_type result_data = result_data_type((m_Data & mask_data) >> digit_start_pos_remain);
				return result_data;
			}
		};

		template<>
		struct get_unique< ranged_digitpack_dec_tag >
		{
			template<class BitSet, int EnumVal, class BlockType> static inline
				typename get_fitted_data_type<BitSet, EnumVal>::type call(const BlockType& m_Data)
			{
				typedef typename get_fitted_data_type<BitSet, EnumVal>::type	result_data_type;
				typedef typename find_ranged_digit<BitSet, EnumVal>::type			found_ranged_digit;

				static const size_t digit_start_pos_remain = found_ranged_digit::digit_start_pos;
				static const size_t digit_last_pos_remain = found_ranged_digit::digit_last_pos;

				static const BlockType left_mask = make_dec_mask<BlockType, digit_last_pos_remain + 1>::value;
				static const BlockType right_mask = make_dec_mask<BlockType, digit_start_pos_remain>::value;

				result_data_type result_data = result_data_type ( (m_Data % left_mask) / right_mask );
				return result_data;
			}
		};

		template<class Tag>
		struct get_same_index
		{
			template<class BitSet, int EnumVal, class BlockType, size_t NUM_BLOCK> static inline
			typename get_fitted_data_type<BitSet, EnumVal>::type call(const BlockType(&m_arrData)[NUM_BLOCK])
			{
				typedef typename get_fitted_data_type<BitSet, EnumVal>::type	result_data_type;
				typedef typename find_ranged_digit<BitSet, EnumVal>::type			found_ranged_digit;
				static const size_t block_digit_size = sizeof(BlockType) * 8;			// 4바이트라면 32 .. 1바이트라면 8비트..

				static const size_t digit_start_pos_index = found_ranged_digit::digit_start_pos / block_digit_size;
				static const size_t digit_start_pos_remain = found_ranged_digit::digit_start_pos % block_digit_size;
				static const size_t digit_last_pos_remain = found_ranged_digit::digit_last_pos % block_digit_size;

				static const BlockType mask_data = make_mask_data<BlockType, digit_start_pos_remain, digit_last_pos_remain>::value;
				result_data_type result_data = result_data_type((m_arrData[digit_start_pos_index] & mask_data) >> digit_start_pos_remain);
				return result_data;
			}
		};

		template<>
		struct get_same_index< ranged_digitpack_dec_tag >
		{
			template<class BitSet, int EnumVal, class BlockType, size_t NUM_BLOCK> static inline
			typename get_fitted_data_type<BitSet, EnumVal>::type call(const BlockType(&m_arrData)[NUM_BLOCK])
			{
				typedef typename get_fitted_data_type<BitSet, EnumVal>::type	result_data_type;
				typedef typename find_ranged_digit<BitSet, EnumVal>::type			found_ranged_digit;

				static const size_t block_digit_size = BitSet::block_digit_size;
								
				static const size_t digit_start_pos_index = found_ranged_digit::digit_start_pos / block_digit_size;
				static const size_t digit_start_pos_remain = found_ranged_digit::digit_start_pos % block_digit_size;
				static const size_t digit_last_pos_remain = found_ranged_digit::digit_last_pos % block_digit_size;

				static const BlockType left_mask = make_dec_mask<BlockType, digit_last_pos_remain + 1>::value;
				static const BlockType right_mask = make_dec_mask<BlockType, digit_start_pos_remain>::value;

				result_data_type result_data = result_data_type( (m_arrData[digit_start_pos_index] % left_mask) / right_mask );
				return result_data;
			}

		};

		template<class Tag>
		struct get_different_index
		{
			template<class BitSet, int EnumVal, class BlockType, size_t NUM_BLOCK> static inline
				typename get_fitted_data_type<BitSet, EnumVal>::type call(const BlockType(&m_arrData)[NUM_BLOCK])
			{
				typedef typename get_fitted_data_type<BitSet, EnumVal>::type	result_data_type;
				typedef typename find_ranged_digit<BitSet, EnumVal>::type			found_ranged_digit;
				static const size_t block_digit_size = sizeof(BlockType) * 8;			// 4바이트라면 32 .. 1바이트라면 8비트..

				static const size_t digit_start_pos_index = found_ranged_digit::digit_start_pos / block_digit_size;
				static const size_t digit_start_pos_remain = found_ranged_digit::digit_start_pos % block_digit_size;
				static const size_t digit_last_pos_index = found_ranged_digit::digit_last_pos / block_digit_size;
				static const size_t digit_last_pos_remain = found_ranged_digit::digit_last_pos % block_digit_size;

				static const BlockType last_mask_data = make_mask_data<BlockType, 0, digit_last_pos_remain>::value;
				result_data_type result_data = result_data_type(m_arrData[digit_last_pos_index] & last_mask_data);
				// 데이터 타입 한개 분량 비트수만큼 왼쪽으로 밀어넣음...
				compile_assert((digit_last_pos_index > 0));
				for (size_t i = digit_last_pos_index - 1; i > digit_start_pos_index; --i)				// 거꾸로 처리함!
				{
					result_data <<= block_digit_size;
					result_data |= result_data_type(m_arrData[i]);
				}

				result_data <<= block_digit_size - digit_start_pos_remain;
				static const BlockType start_mask_data = make_mask_data<BlockType, digit_start_pos_remain, block_digit_size - 1>::value;
				result_data |= result_data_type((m_arrData[digit_start_pos_index] & start_mask_data) >> digit_start_pos_remain);

				return result_data;
			}
		};

		template<>
		struct get_different_index<ranged_digitpack_dec_tag>
		{

			template<class BitSet, int EnumVal, class BlockType, size_t NUM_BLOCK> static inline
			typename get_fitted_data_type<BitSet, EnumVal>::type call(const BlockType(&m_arrData)[NUM_BLOCK])
			{
				typedef typename get_fitted_data_type<BitSet, EnumVal>::type	result_data_type;
				typedef typename find_ranged_digit<BitSet, EnumVal>::type			found_ranged_digit;

				static const size_t block_digit_size = BitSet::block_digit_size;
				static const BlockType max_digit_mask = make_dec_mask<BlockType, block_digit_size>::value;
				
				static const size_t digit_start_pos_index = found_ranged_digit::digit_start_pos / block_digit_size;
				static const size_t digit_start_pos_remain = found_ranged_digit::digit_start_pos % block_digit_size;
				static const size_t digit_last_pos_index = found_ranged_digit::digit_last_pos / block_digit_size;
				static const size_t digit_last_pos_remain = found_ranged_digit::digit_last_pos % block_digit_size;

			
				static const BlockType last_left_mask = make_dec_mask<BlockType, digit_last_pos_remain + 1>::value;
				result_data_type result_data = result_data_type(m_arrData[digit_last_pos_index] % last_left_mask);
				
				// 데이터 타입 한개 분량 비트수만큼 왼쪽으로 밀어넣음...
				compile_assert((digit_last_pos_index > 0));
				for (size_t i = digit_last_pos_index - 1; i > digit_start_pos_index; --i)				// 거꾸로 처리함!
				{
					result_data *= max_digit_mask;
					result_data += result_data_type(m_arrData[i]);
				}

				static const BlockType end_digit_mask = make_dec_mask<BlockType, block_digit_size - digit_start_pos_remain>::value;
				result_data *= end_digit_mask;
							
				static const BlockType right_mask = make_dec_mask<BlockType, digit_start_pos_remain>::value;
							
				result_data += result_data_type(m_arrData[digit_start_pos_index] / right_mask );

				return result_data;
			}

		};

		template<class Tag>
		struct set_invalid
		{
			template<class BitSet, int EnumVal, class BlockType, class IntType> static inline
				void call(BlockType& m_Data, IntType val)
			{
				I3PRINTLOG(I3LOG_WARN, "invalid ranged_bit::set() : enumVal = %d ,  dataVal = %d", EnumVal, val);
				// do nothing....  (못찾는 경우엔 그냥 암것도 안하도록 조절된다...)
			}
		};

		template<class Tag>
		struct set_unique
		{
			template<class BitSet, int EnumVal, class BlockType, class IntType> static inline
				void call(BlockType& m_Data, IntType val)
			{
				typedef typename find_ranged_digit<BitSet, EnumVal>::type			found_ranged_digit;

				static const size_t digit_start_pos_remain = found_ranged_digit::digit_start_pos;
				static const size_t digit_last_pos_remain = found_ranged_digit::digit_last_pos;
				static const BlockType mask_data = make_mask_data<BlockType, digit_start_pos_remain, digit_last_pos_remain>::value;
				m_Data &= ~mask_data;									// 반드시 지우고 대입..
				m_Data |= (val << digit_start_pos_remain) & mask_data;
			}
		};

		template<>
		struct set_unique<ranged_digitpack_dec_tag>
		{
			template<class BitSet, int EnumVal, class BlockType, class IntType> static inline
				void call(BlockType& m_Data, IntType val)
			{
				typedef typename find_ranged_digit<BitSet, EnumVal>::type			found_ranged_digit;

				static const size_t digit_start_pos_remain = found_ranged_digit::digit_start_pos;
				static const size_t digit_last_pos_remain = found_ranged_digit::digit_last_pos;

				static const BlockType left_mask = make_dec_mask<BlockType, digit_last_pos_remain + 1>::value;
				static const BlockType right_mask = make_dec_mask<BlockType, digit_start_pos_remain>::value;
				static const BlockType val_size_mask = make_dec_mask<BlockType, digit_last_pos_remain + 1 - digit_start_pos_remain>::value;
				BlockType result_value = m_Data / left_mask * val_size_mask;
				result_value += val % val_size_mask;
				result_value *= right_mask;
				result_value += m_Data % right_mask;
				m_Data = result_value;
			}
		};
		
		template<class Tag>
		struct set_same_index
		{
			template<class BitSet, int EnumVal, class BlockType, size_t NUM_BLOCK, class IntType> static inline
				void call(BlockType(&m_arrData)[NUM_BLOCK], IntType val)
			{
				typedef typename find_ranged_digit<BitSet, EnumVal>::type			found_ranged_digit;
				static const size_t block_digit_size = sizeof(BlockType) * 8;			// 4바이트라면 32 .. 1바이트라면 8비트..

				static const size_t digit_start_pos_index = found_ranged_digit::digit_start_pos / block_digit_size;
				static const size_t digit_start_pos_remain = found_ranged_digit::digit_start_pos % block_digit_size;
				static const size_t digit_last_pos_remain = found_ranged_digit::digit_last_pos % block_digit_size;

				static const BlockType mask_data = make_mask_data<BlockType, digit_start_pos_remain, digit_last_pos_remain>::value;

				m_arrData[digit_start_pos_index] &= ~mask_data;		// 반드시 지우고 대입..
				m_arrData[digit_start_pos_index] |= (val << digit_start_pos_remain) & mask_data;

			}
		};

		template<>
		struct set_same_index< ranged_digitpack_dec_tag  >
		{
			template<class BitSet, int EnumVal, class BlockType, size_t NUM_BLOCK, class IntType> static inline
				void call(BlockType(&m_arrData)[NUM_BLOCK], IntType val)
			{
				typedef typename find_ranged_digit<BitSet, EnumVal>::type			found_ranged_digit;

				static const size_t block_digit_size = BitSet::block_digit_size;
							
				static const size_t digit_start_pos_index = found_ranged_digit::digit_start_pos / block_digit_size;
				static const size_t digit_start_pos_remain = found_ranged_digit::digit_start_pos % block_digit_size;
				static const size_t digit_last_pos_remain = found_ranged_digit::digit_last_pos % block_digit_size;

				static const BlockType left_mask = make_dec_mask<BlockType, digit_last_pos_remain + 1>::value;
				static const BlockType right_mask = make_dec_mask<BlockType, digit_start_pos_remain>::value;
				static const BlockType val_size_mask = make_dec_mask<BlockType, digit_last_pos_remain + 1 - digit_start_pos_remain>::value;
				BlockType result_value = m_arrData[digit_start_pos_index] / left_mask * val_size_mask;
				result_value += val % val_size_mask;
				result_value *= right_mask;
				result_value += m_arrData[digit_start_pos_index] % right_mask;
				m_arrData[digit_start_pos_index] = result_value;
			}
		};


		template<class Tag>
		struct set_different_index
		{
			template<class BitSet, int EnumVal, class BlockType, size_t NUM_BLOCK, class IntType> static inline
				void call(BlockType(&m_arrData)[NUM_BLOCK], IntType val)
			{
				typedef typename find_ranged_digit<BitSet, EnumVal>::type			found_ranged_digit;
				static const size_t block_digit_size = sizeof(BlockType) * 8;			// 4바이트라면 32 .. 1바이트라면 8비트..

				static const size_t digit_start_pos_index = found_ranged_digit::digit_start_pos / block_digit_size;
				static const size_t digit_start_pos_remain = found_ranged_digit::digit_start_pos % block_digit_size;

				static const size_t digit_last_pos_index = found_ranged_digit::digit_last_pos / block_digit_size;
				static const size_t digit_last_pos_remain = found_ranged_digit::digit_last_pos % block_digit_size;

				// 값받아올때는 왼쪽비트부터 따졌지만..값 셋팅할때는 오른쪽비트부터 처리하는게 더 편해보임..

				static const BlockType start_mask_data = make_mask_data<BlockType, digit_start_pos_remain, block_digit_size - 1>::value;

				m_arrData[digit_start_pos_index] &= ~start_mask_data;		// 반드시 지우고 대입..
				m_arrData[digit_start_pos_index] |= BlockType(val << digit_start_pos_remain) & start_mask_data;

				val >>= (block_digit_size - digit_start_pos_remain);
				// val값을 그냥 쉬프트연산으로 변경...임시변수없이 변경되므로 이후처리에서 주의 요망..

				static const BlockType	block_full_mask = BlockType((1 << block_digit_size) - 1);

				for (size_t i = digit_start_pos_index + 1; i < digit_last_pos_index; ++i)
				{
					m_arrData[i] = BlockType(val) & block_full_mask;		// 통으로 변경되므로 and연산 없다고 볼수 있음..
					val >>= block_digit_size;
				}

				static const BlockType last_mask_data = make_mask_data<BlockType, 0, digit_last_pos_remain>::value;
				m_arrData[digit_last_pos_index] &= ~last_mask_data;		// 반드시 지우고 대입..
				m_arrData[digit_last_pos_index] |= BlockType(val) & last_mask_data;
			}
		};

		template<>
		struct set_different_index<ranged_digitpack_dec_tag>
		{
			template<class BitSet, int EnumVal, class BlockType, size_t NUM_BLOCK, class IntType> static inline
				void call(BlockType(&m_arrData)[NUM_BLOCK], IntType val)
			{
				typedef typename find_ranged_digit<BitSet, EnumVal>::type			found_ranged_digit;
				static const size_t block_digit_size = BitSet::block_digit_size;
				
				static const BlockType max_digit_mask = make_dec_mask<BlockType, block_digit_size>::value;

				static const size_t digit_start_pos_index = found_ranged_digit::digit_start_pos / block_digit_size;
				static const size_t digit_start_pos_remain = found_ranged_digit::digit_start_pos % block_digit_size;

				static const size_t digit_last_pos_index = found_ranged_digit::digit_last_pos / block_digit_size;
				static const size_t digit_last_pos_remain = found_ranged_digit::digit_last_pos % block_digit_size;


				static const BlockType start_right_mask = make_dec_mask<BlockType, digit_start_pos_remain>::value;

				BlockType start_val = val % (max_digit_mask / start_right_mask);
				val /= (max_digit_mask / start_right_mask);

				BlockType result_start_val = start_val * start_right_mask;

				result_start_val += m_arrData[digit_start_pos_index] % start_right_mask;

				m_arrData[digit_start_pos_index] = result_start_val;
				
				for (size_t i = digit_start_pos_index + 1; i < digit_last_pos_index; ++i)
				{
					BlockType result_mid_val = val % max_digit_mask;
					val = val / max_digit_mask;

					m_arrData[i] = result_mid_val;
				}

				static const BlockType end_left_mask = make_dec_mask<BlockType, digit_last_pos_remain + 1>::value;
				
				BlockType result_value = m_arrData[digit_last_pos_index] / end_left_mask * end_left_mask;

				result_value += val % end_left_mask;

				m_arrData[digit_last_pos_index] = result_value;
			}

		};



		namespace detail
		{
			template<class Elem>
			struct get_end_index_from_element { static const size_t value = Elem::digit_last_pos + 1; };

			template<>
			struct get_end_index_from_element<mpl::na> { static const size_t value = 0; };

			template<class NewSeq, class T>
			struct transform_T_to_pos_size
			{
				typedef typename mpl::eval_if<mpl::empty<NewSeq>, mpl::na, mpl::back<NewSeq> >::type	NewSeq_LastElement;
				static const size_t start_index = get_end_index_from_element<NewSeq_LastElement>::value;
				typedef ranged_digit_pos_size<T::enum_value, start_index, T::digit_size>		type;
			};

			struct fold_pred_for_transform_pos_size
			{
				template<class NewSeq, class T>	// 빈벡터로 시작..T는 Seq의 Element들..
				struct apply
				{
					typedef typename T::argument_tag			tag_type;

					typedef typename mpl::eval_if < is_same<tag_type, ranged_digit_size_tag>,
						transform_T_to_pos_size<NewSeq, T>, identity<T> >::type  transformed_T;

					typedef typename mpl::push_back<NewSeq, transformed_T>::type	type;
				};
			};
		}
		//
		// Seq에서 digit_size쪽은 digit_pos_size로 원소를 치환해준다...치환시 이전 원소 정보가 필요하기 때문에
		// fold 처리가 필요할 것 같다.. 
		template<class Seq>
		struct transform_pack_into_pos_size
		{
			typedef typename mpl::fold<Seq, mpl::vector<>, detail::fold_pred_for_transform_pos_size >::type type;
		};
		
		namespace detail
		{
			template<size_t LastPos>
			struct fold_pred_for_transform_rev_order
			{
				template<class NewSeq, class T>	// 빈벡터로 시작..T는 Seq의 Element들..
				struct apply
				{
					typedef ranged_digit_pos_size< T::enum_value, LastPos - T::digit_last_pos, T::digit_size>	new_element_type;
					typedef typename mpl::push_back<NewSeq, new_element_type>::type	type;
				};
			};

		}

		// 인덱스 순서를 반대로 변경한다.....
		template<class Seq>
		struct transform_pack_into_rev_order
		{
			typedef typename mpl::max_element<Seq, less_element_for_max_digit_pos>::type found_iterator;
			typedef typename mpl::deref<found_iterator>::type	result_max_bit_type;
			static const size_t last_pos_value = result_max_bit_type::digit_last_pos;			// start인덱스를 max_last_pos - curr_last_pos 로 변경시켜서 처리..
			
			typedef typename mpl::fold<Seq, mpl::vector<>, detail::fold_pred_for_transform_rev_order<last_pos_value> >::type type;
		};
	}

	template<class RangedDigitPack, class BlockType>
	class ranged_bitset_array_base
	{
	public:
		typedef ranged_bitset_array_base<RangedDigitPack, BlockType>	this_type;

		typedef typename RangedDigitPack::pack_vector_type			pack_vector_type;
		typedef typename mpl::tag<RangedDigitPack>::type				tag;

		typedef BlockType											block_type;

		typedef typename ranged_bitset_imp::get_maximum_element_type<this_type>::type	maximum_element_type;

		static const size_t maximum_digit_index = ranged_bitset_imp::find_maximum_digit_index<this_type>::value;
		static const size_t block_digit_size = ranged_bitset_imp::get_block_digit_size<tag, BlockType>::value;	// sizeof(BlockType) * 8;			// 4바이트라면 32 .. 1바이트라면 8비트..

		static const size_t number_block = (maximum_digit_index + block_digit_size) / block_digit_size;

		static const size_t binary_size_value = number_block * sizeof(block_type);

		ranged_bitset_array_base()
		{
			::memset(m_arrData, 0, sizeof(BlockType) * number_block);
		}
		ranged_bitset_array_base(const ranged_bitset_array_base& rhs)
		{
			::memcpy(m_arrData, rhs.m_arrData, sizeof(BlockType) * number_block);
		}
		void	assign(const ranged_bitset_array_base& rhs)
		{
			::memcpy(m_arrData, rhs.m_arrData, sizeof(BlockType) * number_block);
		}

		void	set_bin(const void* binData)
		{
			::memcpy(m_arrData, binData, sizeof(BlockType) * number_block);
		}

		bool	is_equal(const ranged_bitset_array_base& rhs) const
		{
			return ::memcmp(m_arrData, rhs.m_arrData, sizeof(BlockType) * number_block) == 0;
		}

		bool	empty() const 
		{ 
			for (size_t i = 0; i < number_block; ++i)
			{
				if (m_arrData[i] != 0)
					return false;
			}
			return true;
		}	

	

		BlockType*				data() { return m_arrData; }
		BlockType*				data() const { return m_arrData; }

		static size_t			static_size() { return number_block; }
		static size_t			static_bin_size() { return binary_size_value; }

		template<int EnumVal>
		typename ranged_bitset_imp::get_fitted_data_type<this_type, EnumVal>::type get() const
		{
			// m_arrData가 여러개 걸치는 경우 케이스를 고려해야한다...

			typedef typename ranged_bitset_imp::find_ranged_digit<this_type, EnumVal>::type			found_ranged_digit;

			static const size_t digit_start_pos_index = found_ranged_digit::digit_start_pos / block_digit_size;
			static const size_t digit_last_pos_index = found_ranged_digit::digit_last_pos / block_digit_size;

			typedef i3::is_same<found_ranged_digit, ranged_digit_invalid>				is_invalid_ranged_digit;
			typedef mpl::bool_<  digit_start_pos_index == digit_last_pos_index >	is_same_index_t;

			typedef mpl::multi_if<  is_invalid_ranged_digit, ranged_bitset_imp::get_invalid<tag>,
				is_same_index_t, ranged_bitset_imp::get_same_index<tag>,
				ranged_bitset_imp::get_different_index<tag>   >::type	get_function_type;

			return get_function_type::call<this_type, EnumVal>(m_arrData);
		}

		template<int EnumVal, class IntType>
		void	set(IntType val)						// 정수타입이므로 상수참조 처리 하지 않습니다.
		{
			typedef typename ranged_bitset_imp::find_ranged_digit<this_type, EnumVal>::type			found_ranged_digit;
			static const size_t digit_start_pos_index = found_ranged_digit::digit_start_pos / block_digit_size;
			static const size_t digit_last_pos_index = found_ranged_digit::digit_last_pos / block_digit_size;

			typedef i3::is_same<found_ranged_digit, ranged_digit_invalid>				is_invalid_ranged_digit;
			typedef mpl::bool_<  digit_start_pos_index == digit_last_pos_index >	is_same_index_t;

			typedef mpl::multi_if< is_invalid_ranged_digit, ranged_bitset_imp::set_invalid<tag>,
				is_same_index_t, ranged_bitset_imp::set_same_index<tag>,
				ranged_bitset_imp::set_different_index<tag> >::type	set_function_type;

			set_function_type::call<this_type, EnumVal>(m_arrData, val);
		}

		void	clear() { ::memset(m_arrData, 0, sizeof(BlockType) * number_block); }


	private:
		BlockType		m_arrData[number_block];			// DWORD나 INT64일때 하나 뿐이라면 특화처리하는게 좋을것 같다..
	};

	template<class RangedDigitPack, class BlockType>
	class ranged_bitset_unique_base
	{
	public:
		typedef ranged_bitset_unique_base<RangedDigitPack, BlockType>	this_type;

		typedef typename mpl::tag<RangedDigitPack>::type				tag;
		typedef typename RangedDigitPack::pack_vector_type			pack_vector_type;
		typedef BlockType											block_type;

		typedef typename ranged_bitset_imp::get_maximum_element_type<this_type>::type	maximum_element_type;

		static const size_t maximum_digit_index = ranged_bitset_imp::find_maximum_digit_index<this_type>::value;
		static const size_t block_digit_size = ranged_bitset_imp::get_block_digit_size<tag, BlockType>::value;		// sizeof(BlockType) * 8;			// 4바이트라면 32 .. 1바이트라면 8비트..
		static const size_t number_block = 1;
		static const size_t binary_size_value = sizeof(block_type);

		ranged_bitset_unique_base() : m_Data(BlockType()) { }

		ranged_bitset_unique_base(const ranged_bitset_unique_base& rhs)
		{
			m_Data = rhs.m_Data;
		}

		void	assign(const ranged_bitset_unique_base& rhs)
		{
			m_Data = rhs.m_Data;
		}

		void	set_bin(const void* binData)
		{
			m_Data = *((BlockType*)binData);
		}

		bool	is_equal(const ranged_bitset_unique_base& rhs) const
		{
			return m_Data == rhs.m_Data;
		}

		bool	empty() const { return m_Data == 0;  }

		template<int EnumVal>
		typename ranged_bitset_imp::get_fitted_data_type<this_type, EnumVal>::type get() const
		{
			typedef typename ranged_bitset_imp::find_ranged_digit<this_type, EnumVal>::type		found_ranged_digit;
			typedef i3::is_same<found_ranged_digit, ranged_digit_invalid>				is_invalid_ranged_digit;
			typedef typename mpl::if_< is_invalid_ranged_digit, ranged_bitset_imp::get_invalid<tag>,
				ranged_bitset_imp::get_unique<tag> >::type get_function_type;

			return get_function_type::call<this_type, EnumVal>(m_Data);
		}

		template<int EnumVal, class IntType>
		void	set(IntType val)						// 정수타입이므로 상수참조 처리 하지 않습니다.
		{
			typedef typename ranged_bitset_imp::find_ranged_digit<this_type, EnumVal>::type		found_ranged_digit;
			typedef i3::is_same<found_ranged_digit, ranged_digit_invalid>				is_invalid_ranged_digit;
			typedef typename mpl::if_< is_invalid_ranged_digit, ranged_bitset_imp::set_invalid<tag>,
									ranged_bitset_imp::set_unique<tag>  >::type set_function_type;
			set_function_type::call<this_type, EnumVal>(m_Data, val);
		}

		void	clear() { m_Data = BlockType(); }

		BlockType*				data() { return &m_Data; }
		const BlockType*		data() const { return &m_Data; }

		static size_t			static_size() { return 1; }
		static size_t			static_bin_size() { return binary_size_value; }


	private:
		BlockType		m_Data;
	};

	template<class RangedDigitPack, class BlockType>
	struct find_ranged_bitset_base
	{
		typedef typename mpl::tag<RangedDigitPack>::type				tag;

		static const size_t maximum_digit_index = ranged_bitset_imp::find_maximum_digit_index<RangedDigitPack>::value;
		static const size_t block_digit_size = ranged_bitset_imp::get_block_digit_size<tag, BlockType>::value;				// sizeof(BlockType) * 8;			// 4바이트라면 32 .. 1바이트라면 8비트..
		static const size_t number_block = (maximum_digit_index + block_digit_size) / block_digit_size;

		typedef typename mpl::if_c < (number_block > 1),
			ranged_bitset_array_base<RangedDigitPack, BlockType>,
			ranged_bitset_unique_base<RangedDigitPack, BlockType> > ::type	type;
	};

	template<int EnumVal, class IntType>
	struct ranged_digitvalue_static
	{
		typedef typename mpl::eval_if<	is_const<IntType>,
			remove_const<IntType>,
			add_reference<IntType> >::type  result_type;
		typedef typename remove_const<IntType>::type	const_result_type;

		typedef result_type					parameter_type;
		typedef result_type					data_type;

		ranged_digitvalue_static(parameter_type val) : value(val) {}
		operator const_result_type () const { return value; }
		const_result_type get() const { return value; }

		operator result_type() { return value; }
		result_type get() { return value; }

		data_type value;
	};

	template<class IntType>
	struct ranged_digitvalue_dynamic
	{
		typedef typename mpl::eval_if<	is_const<IntType>,
			remove_const<IntType>,
			add_reference<IntType> >::type  result_type;
		typedef typename remove_const<IntType>::type	const_result_type;

		typedef result_type					parameter_type;
		typedef result_type					data_type;

		ranged_digitvalue_dynamic(int enum_val, parameter_type val) : enum_value(enum_val), value(val) {}

		int					get_enum_value() const { return enum_value; }

		operator const_result_type () const { return value; }
		const_result_type	get_value() const { return value; }

		operator result_type() { return value; }
		result_type get_value() { return value; }

		int			enum_value;
		data_type	value;
	};
	
	namespace rb_imp
	{
		template<class BitSet, size_t ElemPos>
		struct mk_arg
		{
			typedef typename mpl::tag<BitSet>::type			tag;
			typedef typename BitSet::pack_vector_type		PackVector;
			typedef typename mpl::size<PackVector>::type	PackVectorSize;

			typedef typename mpl::eval_if_c < (ElemPos < PackVectorSize::value),
				mpl::at_c<PackVector, ElemPos>,
				i3::identity<ranged_digit_invalid> >::type		element_type;

			static const size_t digit_size = element_type::digit_size;

			static const size_t data_size_value = ranged_bitset_imp::get_fitted_data_size<tag, digit_size>::value;

			typedef mpl::size_t<data_size_value>		data_size_type;
			
			typedef typename mpl::multi_if < is_same< data_size_type, mpl::size_t<0> >, const swallow_copy&,
				is_same < data_size_type, mpl::size_t<8> >, unsigned char,
				is_same < data_size_type, mpl::size_t<16> >, unsigned short,
				is_same < data_size_type, mpl::size_t<32> >, unsigned long,
				is_same< data_size_type, mpl::size_t<64> >, unsigned long long, const swallow_copy&>::type type;
		};
		
		template<class BitSet, size_t ElemPos, class T>
		struct is_convertible_to_T
		{
			typedef typename mk_arg<BitSet, ElemPos>::type		mk_arg_type;

			typedef typename mpl::eval_if< is_same<mk_arg_type, const swallow_copy&>,
				false_type,
				i3::is_convertible<mk_arg_type, T> >::type type;

			static const bool value = type::value;
		};
	

		template<class BitSet, size_t ElemPos>
		struct get_enum
		{
			typedef typename BitSet::pack_vector_type		PackVector;
			typedef typename mpl::size<PackVector>::type	PackVectorSize;

			typedef typename mpl::eval_if_c < (ElemPos < PackVectorSize::value),
				mpl::at_c<PackVector, ElemPos>,
				i3::identity<ranged_digit_invalid> >::type	element_type;

			static const int value = element_type::enum_value;
		};

		template<class Arg>
		struct setter
		{
			template<int EnumVal, class BitSet> static __forceinline
			void call(BitSet& bitSet, Arg data)
			{
				bitSet.set<EnumVal>(data);
			}
		};

		template<>
		struct setter<const swallow_copy&>
		{
			template<int EnumVal, class BitSet> static __forceinline
			void call(BitSet& bitSet, const swallow_copy& )
			{
				// // // none...
			}
		};

		
		template<bool is_convert_possible>
		struct get_extract
		{
			template<class T, int EnumVal, class BitSet>	static __forceinline
			void	call(T& data, const BitSet& bitSet)
			{
				data = bitSet.get<EnumVal>();
			}
		};

		template<>
		struct get_extract<false>
		{
			template<class T, int EnumVal, class BitSet>	static __forceinline
			void	call(T& data, const BitSet& bitSet) 
			{
				// // // none...
			}
		};
		
	}

	namespace ranged_bitset_imp
	{
		template<class BitSet>
		class reference
		{
		public:
			typedef safe_bool<reference>					safe_bool_t;

			typedef typename BitSet::maximum_element_type	maximum_element_type;
				
			reference(BitSet& owner, int enumValue) :
				m_owner(owner), m_enumValue(enumValue) {}

			template<class IntType> __forceinline typename i3::enable_if<is_integral<IntType>,
			reference&>::type operator=(IntType data_val)
			{
				m_owner.set(m_enumValue, data_val);
			}
			
			operator maximum_element_type () const { return m_owner.get(m_enumValue); }
			
			bool	operator!() const { return m_owner.get(m_enumValue) == 0; }	

			operator typename safe_bool_t::type() const { return safe_bool_t::call(m_owner.get(m_enumValue) != 0); }
			
			template<class IntType> __forceinline typename i3::enable_if<is_integral<IntType>,
			reference&>::type operator+=(IntType data_val) { m_owner.set(m_enumValue, m_owner.get(enumValue) + data_val);	return *this;  }

			template<class IntType> __forceinline typename i3::enable_if<is_integral<IntType>,
			reference&>::type operator-=(IntType data_val) {	m_owner.set(m_enumValue, m_owner.get(enumValue) - data_val);	return *this;	}

			template<class IntType> __forceinline typename i3::enable_if<is_integral<IntType>,
			reference&>::type operator*=(IntType data_val) { m_owner.set(m_enumValue, m_owner.get(enumValue) * data_val);	return *this;	}

			template<class IntType> __forceinline typename i3::enable_if<is_integral<IntType>,
			reference&>::type operator/=(IntType data_val) { m_owner.set(m_enumValue, m_owner.get(enumValue) / data_val);  return *this;	}

			template<class IntType> __forceinline typename i3::enable_if<is_integral<IntType>,
			reference&>::type operator%=(IntType data_val) { m_owner.set(m_enumValue, m_owner.get(enumValue) % data_val); return *this;  }

			// 전증가/전감소 만 지원..
			reference& operator++() { m_owner.set(m_enumValue, m_owner.get(enumValue) + 1); return *this; }
			reference& operator--() { m_owner.set(m_enumValue, m_owner.get(enumValue) - 1); return *this; }
		
			maximum_element_type operator++(int) { maximum_element_type tmp = m_owner.get(enumValue); m_owner.set(m_enumValue, tmp +1); return tmp; }
			maximum_element_type operator--(int) { maximum_element_type tmp = m_owner.get(enumValue); m_owner.set(m_enumValue, tmp -1); return tmp; }
			
		private:
			BitSet&		m_owner;
			int			m_enumValue;

		};
		
	}


	namespace ranged_bitset_imp
	{
		template<class BitSet, size_t numSet>
		struct dynamic_setter;

		template<class BitSet, size_t numSet>
		struct dynamic_getter;
	}

}

#define		ranged_bitset_make_enum_constant			0x01
#define		ranged_bitset_set_case_implement			0x02
#define		ranged_bitset_get_case_implement			0x03

#define pp_iteration_params_1	( 3, (1, ranged_bit_pack_limit, "ranged_bitset_impl.h" ))	// 1단계 파일 반복..
#include pp_iterate()
	
#undef		ranged_bitset_get_case_implement
#undef		ranged_bitset_set_case_implement
#undef		ranged_bitset_make_enum_constant

#endif		// ranged_bitset_impl_h

#else		// pp_is_iterating

#if			pp_iteration_depth() == 1

#define		i_		pp_frame_iteration(1)					// 1~40까정..

namespace i3
{
	namespace ranged_bitset_imp
	{
		template<class BitSet>
		struct dynamic_setter<BitSet, i_>
		{
			typedef typename BitSet::pack_vector_type  PackVector;

			template<class IntType> static inline
			void call(BitSet& owner, int enumVal, IntType data_val)
			{

#define			pp_iteration_params_2 (4, (0, pp_dec(i_), "ranged_bitset_impl.h", ranged_bitset_make_enum_constant) )
#include		pp_iterate()
				
				switch (enumVal)
				{
#define			pp_iteration_params_2 (4, (0, pp_dec(i_), "ranged_bitset_impl.h", ranged_bitset_set_case_implement) )
#include		pp_iterate()			
				default:		owner.set<-1>(data_val);	break;
				}
			}
		};
		
		template<class BitSet>
		struct dynamic_getter<BitSet, i_>
		{
			typedef typename BitSet::block_type		BlockType;
			typedef typename BitSet::pack_vector_type  PackVector;
			typedef typename BitSet::maximum_element_type maximum_element_type;

			static inline
			maximum_element_type     call(const BitSet& owner, int enumVal)
			{

#define			pp_iteration_params_2 (4, (0, pp_dec(i_), "ranged_bitset_impl.h", ranged_bitset_make_enum_constant) )
#include		pp_iterate()

				switch (enumVal)
				{
#define			pp_iteration_params_2 (4, (0, pp_dec(i_), "ranged_bitset_impl.h", ranged_bitset_get_case_implement) )
#include		pp_iterate()	
				default:	break;
				}
				return maximum_element_type(owner.get<-1>());
			}

		};
	}
}

#undef i_

#elif		pp_iteration_depth() == 2

#define j_		pp_frame_iteration(2)

#if			pp_iteration_flags() == ranged_bitset_make_enum_constant
				
			typedef typename mpl::at_c<PackVector, j_>::type		pp_cat(ranged_bit, j_);
			static const int pp_cat(enumVal, j_) = pp_cat(ranged_bit, j_)::enum_value;

#elif		pp_iteration_flags() == ranged_bitset_set_case_implement
			
			case pp_cat(enumVal, j_):	
				owner.set<pp_cat(enumVal, j_)>(data_val); 
			break;

#elif		pp_iteration_flags() == ranged_bitset_get_case_implement

			case pp_cat(enumVal, j_):	
				return maximum_element_type(owner.get<pp_cat(enumVal, j_)>());
#endif 

#undef j_

#endif		// pp_iteration_depth()


#endif		// pp_is_iterating