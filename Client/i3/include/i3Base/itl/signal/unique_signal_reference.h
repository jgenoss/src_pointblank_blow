#if !defined(pp_is_iterating)

#ifndef unique_signal_reference_h
#define unique_signal_reference_h

//
// 시그널류가 복사가 전혀 되지 않는 모듈이기 때문에, 이것은 따로 보관하고, 포인터로 시그널 운용하기 위한 래퍼를 만든다.
// 레퍼런스는 내부 값 변경은 할수 없도록 조정하고, 함수 호출과 상수함수만 쓰일수 있도록 처리함..
// 포인터만 쥐기 때문에 쓰레기값이 되지 않도록 유의해야한다..
//

#include "unique_signal.h"

namespace i3
{
	template<class FunSig>
	class unique_signal_reference;

	template<class>
	class unique_signal_reference_base;

	template<class T>
	struct is_unique_signal_reference : i3::is_template_base_of< i3::unique_signal_reference_base<mpl_>, T> {};

	template<class FunSig>
	class unique_signal_reference_base
	{
	public:
		typedef i3::unique_signal<FunSig>					signal_type;
		typedef typename signal_type::signature_type		signature_type;
		typedef typename signal_type::signature_result_type	signature_result_type;
		typedef typename signal_type::result_type			result_type;

		unique_signal_reference_base() : m_signal(&ms_null_signal) {}
		unique_signal_reference_base(const signal_type& s) : m_signal(&s) {}
		~unique_signal_reference_base() {}

		unique_signal_reference_base(const unique_signal_reference_base& rhs) : m_signal(rhs.m_signal) {}
		void    assign(const unique_signal_reference_base& rhs) { m_signal = rhs.m_signal;  }
		void	assign(const signal_type& s) { m_signal = &s;  }
		bool	empty() const { return m_signal->empty(); }

		void	reset() { m_signal = &ms_null_signal; }
		void	clear() { m_signal = &ms_null_signal; }

		const signal_type&	get_signal() const { return m_signal;  }

	private:
		const signal_type*		m_signal;

		static const signal_type	ms_null_signal;
		template<class> friend class unique_signal_reference;
	};

	template<class FunSig>
	const typename unique_signal_reference_base<FunSig>::signal_type unique_signal_reference_base<FunSig>::ms_null_signal;

	template<class FunSig> inline
	bool operator==(const unique_signal_reference<FunSig>& lhs, const unique_signal_reference<FunSig>& rhs)
	{
		return &lhs.get_signal() == &rhs.get_signal();
	}

	template<class FunSig> inline
	bool operator==(const unique_signal_reference<FunSig>& lhs, const unique_signal<FunSig>& rhs)
	{
		return &lhs.get_signal() == &rhs;
	}
	template<class FunSig> inline
	bool operator==(const unique_signal<FunSig>& lhs, const unique_signal_reference<FunSig>& rhs)
	{
		return &lhs == &rhs.get_signal();
	}

	template<class FunSig> inline
	bool operator!=(const unique_signal_reference<FunSig>& lhs, const unique_signal_reference<FunSig>& rhs) { return !(lhs == rhs); }

	template<class FunSig> inline
	bool operator!=(const unique_signal_reference<FunSig>& lhs, const unique_signal<FunSig>& rhs) {	return !(lhs == rhs); }

	template<class FunSig> inline
	bool operator!=(const unique_signal<FunSig>& lhs, const unique_signal_reference<FunSig>& rhs) { return !(lhs == rhs); }
	
}


#define pp_iteration_params_1 (3, (0, ft_arity_limit, "unique_signal_reference.h") )		// 0부터 반복임..
#include pp_iterate()

namespace i3
{
	template< class FunSig> inline
	void swap(unique_signal_reference<FunSig>& lhs, unique_signal_reference<FunSig>& rhs)
	{
		lhs.swap(rhs);
	}
}


#endif // unique_signal_reference_h

#else  // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3
{

	template<class R pp_enum_trailing_params(i_, class A)>
	class unique_signal_reference< R(pp_enum_params(i_, A))> : public unique_signal_reference_base< R(pp_enum_params(i_, A))>
	{
	public:

		typedef unique_signal_reference< R(pp_enum_params(i_, A))>		 this_type;
		typedef unique_signal_reference_base< R(pp_enum_params(i_, A)) > base_type;
		typedef typename unique_signal_reference_base::signal_type		 signal_type;
		typedef typename unique_signal_reference_base::signature_type	 signature_type;
		typedef typename unique_signal_reference_base::result_type		 result_type;
	
	private:
		pp_repeat_binary_params_with_a_cat(i_, typedef typename make_parameter<A, >::type P, ;)
	public:
		
		unique_signal_reference() {}
		unique_signal_reference(const this_type& rhs) : base_type(rhs) {}
		unique_signal_reference(const signal_type& s) : base_type(s) {}

		unique_signal_reference& operator=(const this_type& rhs) { assign(rhs); return *this; }
		unique_signal_reference& operator=(const signal_type& s) { assign(s); return *this; }
				
		result_type		operator()(pp_enum_binary_params(i_, P, p)) const
		{
			return (*m_signal)(pp_enum_params(i_, p));
		}

		void			swap(this_type& rhs)
		{
			i3::swap(m_signal, rhs.m_signal);
		}
	};

}

#undef  i_

#endif // pp_is_iterating