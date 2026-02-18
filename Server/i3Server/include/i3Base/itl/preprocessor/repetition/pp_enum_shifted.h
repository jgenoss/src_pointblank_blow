#pragma once

#include "../arithmetic/pp_dec.h"
#include "../arithmetic/pp_inc.h"
#include "../punctuation/pp_comma_if.h"
#include "pp_repeat.h"		// pp_error, pp_auto_rec, pp_catÆ÷ÇÔ..
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_enum_shifted_impl.h"

#if 0
#define pp_enum_shifted(count, macro, data)
#endif

#define pp_enum_shifted		pp_enum_shifted_impl
