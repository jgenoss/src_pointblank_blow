#pragma once

namespace i3
{

	
	namespace detail
	{
		template<bool>
		struct crtp_factory_register
		{
			template<class D, class Factory>
			static void call()( i3::identity<D> t, Factory* fac)  
			{ 
				i3::fixed_string class_name = get_class_name<D>();
				fac->register_creator( class_name, t);
			}
		};

		template<>
		struct crtp_factory_register<false>
		{
			template<class D, class Factory> static void call()(i3::identity<D>, Factory*)   {}
		};

		template<class D, class Factory>
		void	register_crtp_factory( i3::identity<D> t, Factory* fac)
		{
			static const bool is_concrete = !i3::is_abstract<D>::value;
			crtp_factory_register<is_concrete>::call( t, fac);
		}
	}

	template<class Derived, class Factory>
	struct register_crtp_factory
	{
		register_crtp_factory(Factory* fac) 
		{
			detail::register_crtp_factory( i3::identity<Derived>(), fac);
		}
	};	

	template<class Derived, class Factory>
	void	register_crtp_factory_function(Factory* fac)
	{
		detail::register_crtp_factory(i3::identity<Derived>(), fac);
	}

	template<class Derived, class Factory>
	void	register_crtp_factory_function(i3::identity<Derived> t, Factory* fac)
	{
		detail::register_crtp_factory(t, fac);
	}

	template<class T>
	struct crtp_factory_initializer
	{
		crtp_factory_initializer()
		{
			T::create_explicit();
		}
	};


}
