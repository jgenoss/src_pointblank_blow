#pragma once

#include "../i3string_fwd.h"
#include "../rc_string_fwd.h"
#include "../stack_string_fwd.h"
#include "../fixed_string_fwd.h"

namespace i3
{
	namespace detail_sprintf {	template<class Formatter> class nformatter_io;	}
	namespace detail_sprintf {	template<class Formatter> class formatter_io;	}
	
	template<class SeqOrCh> class sprintf_formatter;

	typedef sprintf_formatter<const char*>			literal_formatter;
	typedef sprintf_formatter<const wchar_t*>		wliteral_formatter;

	typedef sprintf_formatter<i3::string>			string_formatter;
	typedef sprintf_formatter<i3::wstring>			wstring_formatter;
	typedef sprintf_formatter<i3::rc_string>		rc_string_formatter;
	typedef sprintf_formatter<i3::rc_wstring>		rc_wstring_formatter;
	typedef sprintf_formatter<i3::fixed_string>		fixed_string_formatter;
	typedef sprintf_formatter<i3::fixed_wstring>	fixed_wstring_formatter;
	typedef sprintf_formatter<i3::stack_string>		stack_string_formatter;
	typedef sprintf_formatter<i3::stack_wstring>	stack_wstring_formatter;
	
//	sprintf_formatter
	
	class sprintf_formatter_base : private i3::noncopyable {};

	template<class Seq>
	class sprintf_formatter : public sprintf_formatter_base
	{
	public:
		typedef typename i3::range_value<Seq>::type		value_type;
		typedef value_type								Ch;
		typedef detail_sprintf::sprint_local_tag		sprint_object_tag;
		typedef detail_sprintf::sprint_object<Ch, sprint_object_tag>		sprint_object_type;

		sprintf_formatter() {}
		~sprintf_formatter() {}

		sprintf_formatter(const Ch* lit) : m_src_format(lit), m_obj(m_src_format) {}
		template<class Seq2>
		sprintf_formatter(const Seq2& seq) : m_src_format(seq), m_obj(m_src_format) {}

		sprintf_formatter&	operator=(const Ch* lit)	{ m_src_format = lit;	m_obj.reset_format(m_src_format); return *this; }
		template<class Seq2>
		sprintf_formatter&	operator=(const Seq2& seq)	{ m_src_format = seq;	m_obj.reset_format(m_src_format); return *this; }
		
	private:
		Seq						m_src_format;
		sprint_object_type		m_obj;
		template<class Formatter> friend class detail_sprintf::nformatter_io;
		template<class Formatter> friend class detail_sprintf::formatter_io;
	};
	
	//
	namespace detail_sprintf
	{
		template<class Ch>
		class literal_formatter : public sprintf_formatter_base
		{
		public:
			typedef Ch														value_type;
			typedef detail_sprintf::sprint_local_tag						sprint_object_tag;
			typedef detail_sprintf::sprint_object<Ch, sprint_object_tag>	sprint_object_type;

			literal_formatter() {}
			~literal_formatter() {}

			literal_formatter(const Ch* lit) : m_obj( i3::as_literal(lit) ) {}
			template<class Seq>
			literal_formatter(const Seq& seq) : m_obj(i3::as_literal(seq) ) {}

			void reset_format(const Ch* lit) { m_obj.reset_format(i3::as_literal(lit));	}
			template<class Seq>
			void reset_format(const Seq& seq) { m_obj.reset_format(i3::as_literal(seq)); } 
		
		private:
			sprint_object_type		m_obj;
			template<class Formatter> friend class detail_sprintf::nformatter_io;
			template<class Formatter> friend class detail_sprintf::formatter_io;
		};
	}

	template<>
	class sprintf_formatter<const char*> : public detail_sprintf::literal_formatter<char>
	{
	public:
		typedef detail_sprintf::literal_formatter<char>		base_type;
		sprintf_formatter(const char* lit) : base_type(lit) {}
		template<class Seq>
		sprintf_formatter(const Seq& seq) : base_type(seq) {}

		sprintf_formatter&	operator=(const char* lit)	{ base_type::reset_format(lit); return *this; }
		template<class Seq>
		sprintf_formatter&	operator=(const Seq& seq)	{ base_type::reset_format(seq); return *this; }
	};

	template<>
	class sprintf_formatter<const wchar_t*> : public detail_sprintf::literal_formatter<wchar_t>
	{
	public:
		typedef detail_sprintf::literal_formatter<wchar_t>		base_type;
		sprintf_formatter(const wchar_t* lit) : base_type(lit) {}
		template<class Seq>
		sprintf_formatter(const Seq& seq) : base_type(seq) {}
		sprintf_formatter&	operator=(const wchar_t* lit)	{ base_type::reset_format(lit); return *this; }
		template<class Seq>
		sprintf_formatter&	operator=(const Seq& seq)	{ base_type::reset_format(seq); return *this; }
	};


}
