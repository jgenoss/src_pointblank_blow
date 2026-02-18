#pragma once

#include "../arithmetic/pp_add.h"		// pp_while, pp_cat, pp_auto_rec, pp_tuple_elem Æ÷ÇÔ
#include "../arithmetic/pp_sub.h"
#include "pp_repeat.h"

#include "detail/pp_repeat_from_to_impl.h"

#if 0
#define pp_repeat_from_to(first, last, macro, data)
#define pp_repeat_from_to_d(d, first, last, macro, data)
#endif 

#define pp_repeat_from_to		pp_repeat_from_to_impl
#define pp_repeat_from_to_d		pp_repeat_from_to_d_impl
