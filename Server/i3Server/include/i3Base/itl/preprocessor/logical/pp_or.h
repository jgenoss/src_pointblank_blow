#pragma once

#include "pp_bool.h"
#include "pp_bitor.h"

#include "detail/pp_or_impl.h"

#define pp_or(p, q)		pp_or_impl(p, q)
