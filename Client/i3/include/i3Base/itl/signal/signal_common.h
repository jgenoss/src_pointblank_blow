#pragma once

#include "../vector.h"
#include "../list.h"
#include "../../smart_ptr/weak_ptr.h"

namespace i3
{
	namespace detail
	{

		struct connection_holder_base
		{
			typedef i3::list< detail::connection_holder_base* >			embedded_list_type;
			virtual ~connection_holder_base() {}
			virtual void disconnect() = 0;
			virtual void disconnect_by_embedded_object() = 0;
		};
		
	}

	class connection
	{
	public:
		typedef i3::weak_ptr<detail::connection_holder_base>		weak_holder_type;

		connection() {}
		connection( const connection& rhs ) : m_holder( rhs.m_holder) {}
		connection& operator=(const connection& rhs) {	m_holder = rhs.m_holder; return *this; }
		
		~connection() {}

		void	disconnect()
		{
			i3::shared_ptr<detail::connection_holder_base> shared = m_holder.lock();
			if (!shared)
				return;		
			shared->disconnect();
		}
		
		bool	connected() const { return !m_holder.expired(); }

	private:

		connection( const i3::shared_ptr<detail::connection_holder_base>& h ) : m_holder(h) {}
		i3::weak_ptr<detail::connection_holder_base>	m_holder;
	
		template<class, class>	friend class signal_base;
		template<class>			friend class unique_signal_base;
		friend class scoped_connection;
	};
	
	class scoped_connection_set
	{
	public:
		scoped_connection_set() {}
		~scoped_connection_set()
		{
			i3::for_each(m_vec_connection.begin(), m_vec_connection.end(), i3::mem_fun_ref(&connection::disconnect));
		}
		void push_back(const connection& c) { m_vec_connection.push_back(c); }
		void clear() 
		{ 
			i3::for_each(m_vec_connection.begin(), m_vec_connection.end(), i3::mem_fun_ref(&connection::disconnect));  
			m_vec_connection.clear();
		}
	private:
		i3::vector<connection> m_vec_connection;
	};

	inline scoped_connection_set& operator+=(scoped_connection_set& s, const connection& c) { s.push_back(c); return s; }
	inline scoped_connection_set& operator<<(scoped_connection_set& s, const connection& c) { s.push_back(c); return s; }
	
	class scoped_connection : public connection
	{
	public:
		scoped_connection() {}
		scoped_connection(const connection& rhs) : connection(rhs) {}
		~scoped_connection() {  disconnect(); }			// 주 구현은 이것뿐임..(여기서는 복사가 되지 않도록 막아야한다.)
		
		scoped_connection& operator=(const connection& rhs) 
		{
			disconnect();	connection::operator=(rhs); return *this;
		}

		connection release() 
		{
			connection temp(*this);		m_holder.reset();	return temp;
		}
		
	private:
		scoped_connection(const scoped_connection &rhs);
		scoped_connection& operator=(const scoped_connection &rhs);
	};
	

	template<class Base = i3::null_type>
	class shared_disconnect : public Base
	{
	public:
		typedef Base		base_type;
		shared_disconnect(){}
		template<class T>
		shared_disconnect(T arg) : base_type(arg) {}			// 인자생성자를 위해 계속추가될 가능성있음..

#define repeat_shared_disconnect_cons(z, n, d) \
		template<pp_enum_params_1(n, class T)> \
		shared_disconnect( pp_enum_binary_params_1(n, T, a) ) : base_type(pp_enum_params_1(n, a)) {}
		pp_repeat_from_to(2, 6, repeat_shared_disconnect_cons, ~)
#undef	repeat_shared_disconnect_cons		
		
		typedef i3::list<detail::connection_holder_base*>				embedded_list_type;
		typedef embedded_list_type::iterator							embedded_list_iterator;

		~shared_disconnect()
		{
			embedded_list_iterator it_end = m_embedded_list.end();

			for ( embedded_list_iterator it = m_embedded_list.begin(); it != it_end; ++it )
			{
				(*it)->disconnect_by_embedded_object();			// 무조건 실행 가능해야함..
			}
		}
		
		void	disconnect_all() 
		{ 
			embedded_list_iterator it_end = m_embedded_list.end();

			for ( embedded_list_iterator it = m_embedded_list.begin(); it != it_end; ++it )
			{
				(*it)->disconnect_by_embedded_object();
			}

			m_embedded_list.clear();
		}

	private:
		embedded_list_type& get_embedded_connection_list() { return m_embedded_list; }

		embedded_list_type m_embedded_list;

		template<class, class>	friend class signal_base;
		template<class>			friend class unique_signal_base;
	};

	
	
}