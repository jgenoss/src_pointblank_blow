#pragma once

namespace i3
{
	//
	// 알다시피....이 클래스를 다른 기본클래스와 함께 쓰려면 다중상속을 허용해야함..(다른 방법은 없음)
	//

	template<class T>
	class enable_shared_from_this
	{
	public:
		shared_ptr<T>		shared_from_this() { return shared_ptr<T>(m_weak_this); }
		shared_ptr<const T>	shared_from_this() const { return shared_ptr<T>(m_weak_this); }

		template<class X, class Y>
		void				internal_accept_owner( const shared_ptr<X>* ppx, Y* py) const 
		{
			if ( m_weak_this.expired() ) m_weak_this = shared_ptr<T>(*ppx, py); 
		}

	protected:
		enable_shared_from_this() {}
		enable_shared_from_this(const enable_shared_from_this&) {}
		enable_shared_from_this& operator=(const enable_shared_from_this&) { return *this; }
		~enable_shared_from_this() {}
	private:
		mutable weak_ptr<T>	m_weak_this;
	};

}