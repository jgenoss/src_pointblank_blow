#pragma once

namespace i3
{
	namespace detail
	{
		class esft2_deleter_wrapper
		{
		public:
			esft2_deleter_wrapper() {}
			template<class T> void set_deleter(const shared_ptr<T>& deleter) { m_deleter = deleter; }
			template<class T> void operator()(T*) { m_deleter.reset(); }
		private:
			shared_ptr<void> m_deleter;
		};
	}
	
	template<class T>
	class enable_shared_from_this2
	{
	public:
		shared_ptr<T> shared_from_this() {  init_weak_once(); return shared_ptr<T>(m_weak_this); }
		shared_ptr<const T> shared_from_this() const {  init_weak_once(); return shared_ptr<T>(m_weak_this); }
		
		template<class X, class Y>
		void			internal_accept_owner( shared_ptr<X>* ppx, Y* py) const
		{
			if (m_weak_this.use_count() == 0 )
			{
				m_weak_this = shared_ptr<T>(*ppx, py); 
			}
			else
			if ( m_shared_this.use_count() != 0)
			{
				detail::esft2_deleter_wrapper* pd = get_deleter<detail::esft2_deleter_wrapper>(m_shared_this);
				pd->set_deleter(*ppx);
				ppx->reset( m_shared_this, ppx->get() );
				m_shared_this.reset();
			}
		}

	protected:
		enable_shared_from_this2() {}
		enable_shared_from_this2(const enable_shared_from_this2&) {}
		enable_shared_from_this2& operator=(const enable_shared_from_this2&) { return *this; }
		~enable_shared_from_this2() {}

	private:
		
		void init_weak_once() const
		{
			if( m_weak_this.empty() )
			{
				m_shared_this.reset( static_cast<T*>( 0 ), detail::esft2_deleter_wrapper() );
				m_weak_this = m_shared_this;
			}
		}

		mutable weak_ptr<T> m_weak_this;
		mutable shared_ptr<T> m_shared_this;
		
	};

}