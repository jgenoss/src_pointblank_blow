#pragma once

#include "../control/pp_iif.h"
#include "pp_is_1.h"		// pp_is_empty, pp_empty Æ÷ÇÔ..

#include "detail/pp_is_empty_or_1_impl.h"

#define pp_is_empty_or_1(x)		pp_is_empty_or_1_impl(x)
