#pragma once

#include "../control/pp_if.h"
#include "../facilities/pp_empty.h"
#include "pp_comma.h"

#include "detail/pp_comma_if_impl.h"

#define pp_comma_if(cond)					pp_comma_if_impl(cond)
