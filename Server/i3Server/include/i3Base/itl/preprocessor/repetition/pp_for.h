#pragma once

#include "../debug/pp_error.h"		// pp_cat Æ÷ÇÔ
#include "../detail/pp_auto_rec.h"

#include "../control/pp_if.h"
#include "../tuple/pp_tuple_eat.h"

#include "detail/pp_for_impl.h"

#if 0
#define pp_for(state, pred, op, macro)
#endif

#define pp_for		pp_for_impl
