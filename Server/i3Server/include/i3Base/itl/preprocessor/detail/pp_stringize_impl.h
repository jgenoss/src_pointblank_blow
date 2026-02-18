#pragma once

#define pp_stringize_impl(text)			pp_stringize_a((text))
#define pp_stringize_a(arg)				pp_stringize_i arg
#define pp_stringize_i(text)			#text
