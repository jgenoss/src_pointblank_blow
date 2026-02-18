#pragma once

// i3System::GetExecPath(out)의 교체 버전..
#include "extract_directoryname.h"
#include "remove_end_slash.h"
#include "get_module_filename.h"
#include "../stack_string.h"

namespace i3
{
	namespace detail
	{
		template<class SeqOrLit, class Ch>
		void get_exec_path_impl( SeqOrLit out)
		{
			static const size_t init_max_len = 255;
			typedef i3::stack_basic_string<mpl::size_t<init_max_len+1>, Ch> string_type;

			string_type	strTemp;
			if ( i3::get_module_filename(strTemp) == 0 )
			{
				I3TRACE( "Could not obtain the module file name\n");
				return;
			}

			i3::extract_directoryname(strTemp, out);
			i3::remove_end_slash(out);
		}
	}

	template<class OutLit> inline typename i3::enable_if_iterator<OutLit>::type 
	get_exec_path( OutLit out)
	{
		typedef typename i3::iterator_value<OutLit>::type Ch;
		detail::get_exec_path_impl<OutLit, Ch>(out);
	}

	template<class OutSeq> inline typename i3::disable_if_iterator<OutSeq>::type
	get_exec_path(OutSeq& out)
	{
		typedef typename i3::range_value<OutSeq>::type Ch;
		detail::get_exec_path_impl<OutSeq&, Ch>(out);
	}


}