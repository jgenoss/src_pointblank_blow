#if !defined(pp_is_iterating)

#ifndef factory_h
#define factory_h

#include "../itl/function_types/ft_any_to_function_type.h"
#include "../itl/function_types/ft_modify_function_result_type.h"
#include "../itl/function_types/ft_modify_function_parameters.h"
#include "../itl/container_util.h"
#include "../itl/preprocessor/repetition/pp_enum_binary_params.h"
#include "../itl/preprocessor/repetition/pp_enum_trailing_params.h"

#define factory_arity_limit		5

#include "detail/factory_creator.h"

#pragma push_macro("new")
#undef new

//
// 팩토리 메서드와 매니저컨테이너를 조합한 형태를 갖고, 임의로 간단이 factory란 클래스라고 명명해둠..
// 디버그 모드에서 추적이 반드시 되어야하므로, file,line인자를 둔 디버그버전 함수를 넣는다..
// file,line삽입 방식에는 매크로가 필요한데, 멤버함수이름이 일반적이면 한정하기가 어렵다.. (일반함수화,일반함수매크로 혹은 멤버함수이름을 고유하게 지정)
// 일반함수화를 하면 매개변수타입지정때, 참조타입인수에는 제약이 생김...일반함수매크로방식을 쓰는것은 괜찮은데, 일단은 멤버함수이름을 다소 어렵게 정하는 방식을 채택한다..
//
namespace i3
{
	namespace detail
	{
		template<class CreateFun>
		struct make_factory_function_type
		{
			typedef typename ft::any_to_function_type<CreateFun>::type		force_function_type;
			typedef typename ft::result_type<force_function_type>::type		result_type;

			typedef typename mpl::if_<is_pointer<result_type>, mpl::identity<mpl::_>, i3::add_pointer<mpl::_> >::type result_transform;
			typedef typename ft::modify_function_result_type<force_function_type, result_transform>::type modified;
			typedef typename ft::modify_function_parameters<modified, make_parameter<mpl::_> >::type type;
		};
	}

	template<class Key, class CreateFun>
	class factory_base;

	template<class Key, class CreateFun>
	class factory : public factory_base<Key, typename detail::make_factory_function_type< CreateFun >::type >
	{
	};
	
}

#define		pp_iteration_params_1	(3, (0, factory_arity_limit, "factory.h"))		// 0 ~ 5까지 특화 / 자체재귀..(양개구간임에 유의할것)
#include	pp_iterate()

namespace i3
{
#define create_factory_object_no_dbg(k, ...)	create_factory_object(k, __VA_ARGS__)	

#if defined(_DEBUG)
#define create_factory_object(k, ...)			create_factory_object(k, __VA_ARGS__, __FILE__, __LINE__)
#endif
}


#pragma pop_macro("new")

#endif

#else // pp_is_iterating

#define i_				pp_frame_iteration(1)

namespace i3
{

	template<class Key, class R pp_enum_trailing_params(i_, class P)>
	class factory_base<Key, R (pp_enum_params(i_, P))> 
	{
	public:

		typedef  R												(create_function_type)(pp_enum_params(i_, P));	

		typedef Key												key_type;
		typedef	detail::factory_creator<R (pp_enum_params(i_, P))>	creator; 
		typedef i3::unordered_map<key_type, creator*>		container_type;
		static const size_t										arity = i_;
		typedef typename i3::remove_pointer<R>::type			result_class_type;

		factory_base() {}
		~factory_base() { i3::cu::for_each_delete(m_ctn); }

		template<class D>
		void	register_creator(const Key& k, i3::identity<D>)
		{
			typedef typename mpl::if_<	is_abstract<D>, 
								detail::factory_creator_na<create_function_type>, 
								detail::factory_creator_derived<create_function_type, D> >::type derived_type;

			i3::pair<typename container_type::iterator, bool> res = m_ctn.insert( container_type::value_type(k, 0) );
			if (!res.second) delete res.first->second; 
			res.first->second = new derived_type;
		}

		template<class CustomFun>
		void	register_creator(const Key& k, CustomFun f)		// 함수객체,함수포인터..Fun에 대해서는 너무 지저분하지 않게 되도록 잘 정의해야함..
		{													// 디버깅 용도의 file, line이 잘들어가도록 해야함...
			typedef detail::factory_creator_custom<create_function_type, CustomFun>	custom_type;
			i3::pair<typename container_type::iterator, bool> res = m_ctn.insert( container_type::value_type(k, 0) );
			if ( !res.second ) delete res.first->second; 
			res.first->second = new custom_type(f);
		}

		void	unregister_creator( const Key& k)
		{
			container_type::iterator it = m_ctn.find(k);
			if (it == m_ctn.end() ) return;
			delete it->second;
			m_ctn.erase(it);
		}

		creator*	find_creator(const Key& k) const
		{
			container_type::const_iterator it = m_ctn.find(k);
			return ( it != m_ctn.end() ) ? it->second : nullptr;
		}

		R		create_factory_object(const key_type& k pp_comma_if(i_) pp_enum_binary_params(i_, P, p)) const
		{                             
			typename container_type::const_iterator 	it =	m_ctn.find(k);
			if ( it == m_ctn.end() ) return R();
			return (*it->second)( pp_enum_params( i_, p));
		}

#if defined(_DEBUG)
		R		create_factory_object(const key_type& k pp_comma_if(i_) pp_enum_binary_params(i_, P, p) , const char* file, int line) const
		{
			typename container_type::const_iterator 	it =	m_ctn.find(k);
			if ( it == m_ctn.end() ) return R();
			return (*it->second)( pp_enum_params( i_, p) pp_comma_if(i_) file, line);
		}
#endif
	private:
		container_type			m_ctn;
	};
}

#undef i_

#endif // pp_is_iterating

