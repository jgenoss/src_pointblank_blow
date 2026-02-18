#pragma once

#define pp_wstringize_impl(text)		pp_wstringize_i(text)
#define pp_wstringize_i(text)			pp_wstringize_ii(#text)
#define pp_wstringize_ii(str)			L ## str
