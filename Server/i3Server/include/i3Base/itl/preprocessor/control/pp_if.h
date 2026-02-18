#pragma once

#include "pp_iif.h"
#include "../logical/pp_bool.h"

#include "detail/pp_if_impl.h"

#define pp_if( cond, t, f )					pp_if_impl(cond, t, f)
