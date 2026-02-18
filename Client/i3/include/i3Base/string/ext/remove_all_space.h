#pragma once

// remove_all_blank/ remove_all_char와 비슷하다..
#include "remove_all_char.h"

namespace i3
{
	
	template<class Literal> inline typename i3::enable_if_iterator<Literal>::type 
	remove_all_space(Literal ctn)
	{
		remove_all_char(ctn, ' ');
	}

	template<class Seq> inline typename i3::disable_if_iterator<Seq>::type
	remove_all_space(Seq& s)
	{
		remove_all_char(s, ' ');
	}
	
}