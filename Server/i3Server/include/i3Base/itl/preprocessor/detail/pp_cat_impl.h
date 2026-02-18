#pragma once


#define pp_cat_impl(a, b)	pp_cat_impl0(a ## b)
#define pp_cat_impl0(res)	res

