#pragma once

//
//  optional ===> 당장 활용되지 않는 미초기화 오브젝트의 경우, 해당 오브젝트의 생성자 호출을 피할수 있는게 제일 큰 장점인 것 같다..
//           ===> 실제로 유효한 값이 들어오더라도, 동적메모리할당이 일어나지 않고, 이미 주어진 메모리영역을 쓰는것도 장점이다..
//           ===> 미초기화된 경우 널값처럼 취급된다..
//           ===> 참조값을 계속 대입할수 있는 특징도 갖고 있음..
//           ---> 복사 생성/복사 대입이 아닌 경우엔 생성이 어려운 특성이 있음.. (복수 인수 생성자나 디폴트생성자의 강제생성을 생각해볼것..)
//                이 때문에, in_place()함수를 사용한다..

#include "optional_fwd.h"
#include "../utility/compare_pointees.h"
#include "../utility/in_place_factory.h"
#include "../utility/typed_in_place_factory.h"
#include "../type_traits/alignment_of.h"
#include "../type_traits/aligned_storage.h"
#include "../type_traits/make_pointer_result.h"
#include "../type_traits/make_const_pointer_result.h"

#pragma push_macro("new")

#undef new

namespace i3
{
	namespace detail
	{
		struct optional_base_non_ref_tag {};
		struct optional_base_ref_tag {};

		template<class T>
		class optional_base 
		{
			typedef typename i3::remove_reference<T>::type															referred_type;
			typedef typename mpl::if_< is_reference<T>, reference_wrapper< referred_type >, T >::type				internal_type;
			typedef i3::aligned_storage<sizeof(internal_type), i3::alignment_of<internal_type>::value>				storage_type;
			
			typedef typename mpl::if_< is_reference<T>, optional_base_ref_tag, optional_base_non_ref_tag>::type		reference_pred_tag;
			
		protected:	
			typedef typename make_reference_result<T>::type				reference_result;
			typedef typename make_const_reference_result<T>::type		const_reference_result;
			typedef typename make_pointer_result<T>::type				pointer_result;
			typedef typename make_const_pointer_result<T>::type			const_pointer_result;
			typedef typename make_parameter<T>::type					parameter_type;

			optional_base() : m_referred_ptr(nullptr) {}
			optional_base( null_type ) : m_referred_ptr(nullptr) {}
			optional_base(parameter_type val) : m_referred_ptr(nullptr) {	construct(val);	}
			
			optional_base(const optional_base& rhs) : m_referred_ptr(nullptr)
			{
				if (rhs.is_initialized())
					construct(rhs.get_value());
			}
			optional_base( bool cond, parameter_type val) : m_referred_ptr(nullptr) {	if (cond) construct(val);	}
			
			template<class InPlaceFactory>
			optional_base( const InPlaceFactory& factory, 
				typename i3::enable_if< i3::is_in_place_factory<InPlaceFactory>, int>::type t = 0) 
			{
				internal_type* internal_data = factory(i3::identity<internal_type>(), m_storage.address() );
				m_referred_ptr = internal_data;
			}
			
			template<class TypedInPlaceFactory>
			optional_base( const TypedInPlaceFactory& factory, 
				typename i3::enable_if< i3::is_typed_in_place_factory<TypedInPlaceFactory>, int>::type t = 0) 
			{
				internal_type* internal_data = factory( m_storage.address() );
				m_referred_ptr = internal_data;
			}
			

			~optional_base() { destroy(); }

			void assign(const optional_base& rhs) 
			{
				if ( is_initialized() )
				{
					if (rhs.is_initialized() )
						assign_value(rhs.get_value(), reference_pred_tag());
					else
						destroy();
				}
				else
				{
					if (rhs.is_initialized() )
						construct(rhs.get_value());
				}
			}

			template<class U>
			void assign(const optional<U>& rhs) 
			{
				if ( is_initialized() )
				{
					if (rhs.is_initialized() )
						assign_value(static_cast<T>(rhs.get()), reference_pred_tag());
					else
						destroy();
				}
				else
				{
					if (rhs.is_initialized() )
						construct(static_cast<T>(rhs.get()));
				}
			}
			
			void assign(parameter_type val) 
			{
				if (is_initialized()) assign_value(val, reference_pred_tag());
				else construct(val); 
			}
			
			void assign(null_type) { destroy(); }
			
			template<class InPlaceFactory> typename i3::enable_if< i3::is_in_place_factory<InPlaceFactory> >::type
			assign(const InPlaceFactory& factory) 
			{
				if ( is_initialized() )
					destroy();
				
				internal_type* internal_data = factory(i3::identity<internal_type>(), m_storage.address() );
				m_referred_ptr = internal_data;
			}

			template<class TypedInPlaceFactory> typename i3::enable_if< i3::is_typed_in_place_factory<TypedInPlaceFactory> >::type
			assign(const TypedInPlaceFactory& factory) 
			{
				if ( is_initialized() )
					destroy();
				
				internal_type* internal_data = factory( m_storage.address() );
				m_referred_ptr = internal_data;
			}

			bool				is_initialized() const { return m_referred_ptr != nullptr; }

			void				construct(parameter_type val) { construct_impl( val, reference_pred_tag() ); }
			void				destroy() {	destroy_impl(  reference_pred_tag() ); }


			const_reference_result	get_value() const	{ return *m_referred_ptr; } 
			reference_result		get_value()			{ return *m_referred_ptr; }

			const_pointer_result	get_value_ptr() const	{ return m_referred_ptr; } 
			pointer_result			get_value_ptr()			{ return m_referred_ptr; }

		private:
	
			void					assign_value(parameter_type val, optional_base_non_ref_tag) {	*m_referred_ptr	= val; }
			void					assign_value(parameter_type val, optional_base_ref_tag) { construct(val); }


			
			void				construct_impl(parameter_type val, optional_base_ref_tag )
			{
				internal_type* internal_data = new (m_storage.address()) internal_type(val);
				NetworkDump_Malloc( internal_data, sizeof(internal_type), __FILE__, __LINE__);
				m_referred_ptr = internal_data->get_pointer();
			}

			void				construct_impl( parameter_type val, optional_base_non_ref_tag )
			{
				internal_type* internal_data = new (m_storage.address()) internal_type(val);
				NetworkDump_Malloc( internal_data, sizeof(internal_type), __FILE__, __LINE__);
				m_referred_ptr = internal_data;
			}
	
			void				destroy_impl( optional_base_ref_tag ) { m_referred_ptr = nullptr; } // 소멸자 호출을 꼭해주지 않아도 된다.
			void				destroy_impl( optional_base_non_ref_tag) 
			{ 
				if (m_referred_ptr)
				{
					NetworkDump_Free( m_referred_ptr, __FILE__, __LINE__);
					i3::destroy(m_referred_ptr); 	m_referred_ptr = nullptr;
				}
			}

			referred_type*		m_referred_ptr;			// 부스트와는 달리 이것을 사용한다..(실제로 쓰는 값에 바로 접근하는게 좋다..)
			storage_type		m_storage;
		};

	}

	template<class T>
	class optional : public detail::optional_base<T>
	{
	public:
		typedef detail::optional_base<T>		base_type;
		typedef optional<T>						this_type;
	
		typedef typename base_type::parameter_type				parameter_type;
		typedef typename base_type::reference_result			reference_result;
		typedef typename base_type::const_reference_result		const_reference_result;
		typedef typename base_type::pointer_result				pointer_result;
		typedef typename base_type::const_pointer_result		const_pointer_result;

		optional() {}
		optional(null_type t) : base_type(t) {}
		optional(parameter_type val) : base_type(val) {}
		optional(bool cond, parameter_type val) : base_type(cond, val) {}
		
		template<class U>
		optional( const optional<U>& rhs) {  if ( rhs.is_initialized() ) this->construct(rhs.get()); }

		optional(const optional& rhs) : base_type(static_cast<const base_type&>(rhs)) {}

		template<class InPlaceFactory>
		optional( const InPlaceFactory& factory) : base_type(factory) {}

		~optional() {}

		
		template<class U>
		optional&	operator=(const optional<U>& rhs) {	assign(rhs); return *this; }
		optional&	operator=(optional& rhs) { assign(rhs); return *this; }
		optional&	operator=(parameter_type val) { assign(val); return *this; }
		optional&	operator=(null_type t) { assign(t);	return *this; }

		template<class InPlaceFactory>
		optional&	operator=(const InPlaceFactory& factory) {   assign(factory);  return *this; }

		void		reset() { base_type::destroy(); }
		void		reset(parameter_type val) { base_type::assign(val); }

		bool				is_initialized() const { return base_type::is_initialized(); }

		void		swap(optional& rhs) { i3::swap(*this, rhs); }		// 일반함수 구현이 필요하다..

		reference_result			get() { return base_type::get_value(); }
		const_reference_result		get() const { return base_type::get_value(); }

		reference_result			get_value_or(reference_result default_val)  { return is_initialized() ? base_type::get_value() : default_val; }
		const_reference_result		get_value_or(const_reference_result default_val) const { return is_initialized() ? base_type::get_value() : default_val; }
		
		pointer_result				get_pointer() { return base_type::get_value_ptr(); }
		const_pointer_result		get_pointer() const { return base_type::get_value_ptr(); }
		
		pointer_result				operator->() { return base_type::get_value_ptr(); }
		const_pointer_result		operator->() const { return base_type::get_value_ptr(); }
		
		reference_result			operator *() { return base_type::get_value(); }
		const_reference_result		operator *() const { return base_type::get_value(); }
		
		operator typename safe_bool<this_type>::type () const { return safe_bool<this_type>::call(base_type::is_initialized()); }
		bool						operator !() const { return !base_type::is_initialized(); }

	private:
		template<class U> friend void swap(optional<U>&, optional<U>&);
	};

	template<class T> inline
	optional<T>		make_optional(const T& v) { return optional<T>(v); }

	template<class T> inline
	optional<T>		make_optional(bool cond, const T& v) { return optional<T>(cond, v); }

	template<class T> inline
	typename optional<T>::const_reference_result get(const optional<T>& o) { return o.get(); }

	template<class T> inline
	typename optional<T>::reference_result get(optional<T>& o) { return o.get(); }
	
	template<class T> inline
	typename optional<T>::const_pointer_result get(const optional<T>* o) { return o->get_pointer(); }

	template<class T> inline
	typename optional<T>::pointer_result get(optional<T>* o) { return o->get_pointer(); }
	
	template<class T> inline
	typename optional<T>::const_reference_result get_optional_value_or( const optional<T>& o, typename optional<T>::const_reference_result v)
	{
		return o.get_value_or(v);
	}
	
	template<class T> inline
	typename optional<T>::reference_result get_optional_value_or( optional<T>& o, typename optional<T>::reference_result v)
	{
		return o.get_value_or(v);
	}

	template<class T> inline
	typename optional<T>::const_pointer_result get_pointer(const optional<T>& o) { return o.get_pointer(); }

	template<class T> inline
	typename optional<T>::pointer_result get_pointer(optional<T>& o) { return o.get_pointer(); }
	
//// 
	template<class T> inline
	void	swap(optional<T>& x, optional<T>& y)
	{
		const bool hasX = x.is_initialized();
		const bool hasY = y.is_initialized();

		if (!hasX && !hasY) 
			return;

		if (!hasX) { x.construct(y.get());	y.reset(); }
		else 
		if ( !hasY ) { y.construct(x.get());	x.reset();	}
		else
			i3::swap(x.get(), y.get());
	}

//// 비교연산자 구현..

	template<class T> inline bool operator == ( optional<T> const& x, optional<T> const& y ) { return equal_pointees(x,y); }
	template<class T> inline bool operator < ( optional<T> const& x, optional<T> const& y ) { return less_pointees(x,y); }
	template<class T> inline bool operator != ( optional<T> const& x, optional<T> const& y ) { return !( x == y ) ; }
	template<class T> inline bool operator > ( optional<T> const& x, optional<T> const& y ) { return y < x ; }
	template<class T> inline bool operator <= ( optional<T> const& x, optional<T> const& y ) { return !( y < x ) ; }
	template<class T> inline bool operator >= ( optional<T> const& x, optional<T> const& y ) { return !( x < y ) ; }

// optional<T> vs T cases
	template<class T> inline bool operator == ( optional<T> const& x, T const& y ) { return (!x) ? false : *x == y;	}
	template<class T> inline bool operator < ( optional<T> const& x, T const& y ) { return (!x) ? true : *x < y; }
	template<class T> inline bool operator != ( optional<T> const& x, T const& y ) { return !( x == y ) ; }
	template<class T> inline bool operator > ( optional<T> const& x, T const& y ) { return y < x ; }
	template<class T> inline bool operator <= ( optional<T> const& x, T const& y ) { return !( y < x ) ; }
	template<class T> inline bool operator >= ( optional<T> const& x, T const& y ) { return !( x < y ) ; }
//
// T vs optional<T> cases
//
	template<class T> inline bool operator == ( T const& x, optional<T> const& y ) { return (!y) ? false : x == *y; }
	template<class T> inline bool operator < ( T const& x, optional<T> const& y ) { return (!y) ? false : x < *y; }
	template<class T> inline bool operator != ( T const& x, optional<T> const& y ) { return !( x == y ) ; }
	template<class T> inline bool operator > ( T const& x, optional<T> const& y ) { return y < x ; }
	template<class T> inline bool operator <= ( T const& x, optional<T> const& y ) { return !( y < x ) ; }
	template<class T> inline bool operator >= ( T const& x, optional<T> const& y ) { return !( x < y ) ; }

//
// optional<T> vs null_type
//
	template<class T> inline bool operator == ( optional<T> const& x, null_type ) { return !x; }
	template<class T> inline bool operator < ( optional<T> const& , null_type ) { return false; }
	template<class T> inline bool operator != ( optional<T> const& x, null_type y ) { return !( x == y ) ; }
	template<class T> inline bool operator > ( optional<T> const& x, null_type y ) { return y < x ; }
	template<class T> inline bool operator <= ( optional<T> const& x, null_type y ) { return !( y < x ) ; }
	template<class T> inline bool operator >= ( optional<T> const& x, null_type y ) { return !( x < y ) ; }

//
// null_type vs optional<T> 
//
	template<class T> inline bool operator == ( null_type , optional<T> const& y ) { return !y; }
	template<class T> inline bool operator < ( null_type , optional<T> const& y ) {  return y.is_initialized(); }
	template<class T> inline bool operator != ( null_type x, optional<T> const& y ) { return !( x == y ) ; }
	template<class T> inline bool operator > ( null_type x, optional<T> const& y ) { return y < x ; }
	template<class T> inline bool operator <= ( null_type x, optional<T> const& y ) { return !( y < x ) ; }
	template<class T> inline bool operator >= ( null_type x, optional<T> const& y ) { return !( x < y ) ; }


}

#pragma pop_macro("new")

