#pragma once

#include "../debug/pp_error.h"		// pp_catÆ÷ÇÔ.
#include "../detail/pp_auto_rec.h"
#include "../tuple/pp_tuple_eat.h"

#include "detail/pp_repeat_impl.h"

#if 0
#define pp_repeat(count, macro, data)		pp_repeat_impl(count, macro, data)
#endif

#define pp_repeat							pp_repeat_impl
