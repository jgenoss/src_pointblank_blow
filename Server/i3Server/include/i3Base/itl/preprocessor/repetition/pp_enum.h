#pragma once

#include "../punctuation/pp_comma_if.h"
#include "pp_repeat.h"		// pp_cat, pp_error, pp_auto_rec
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_enum_impl.h"

#if 0
#define pp_enum(count, macro, data)
#endif

#define pp_enum		pp_enum_impl
