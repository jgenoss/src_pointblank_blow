#pragma once

#include "pp_bool.h"
#include "pp_bitand.h"

#include "detail/pp_and_impl.h"

#define pp_and(p, q)						pp_and_impl(p, q)
