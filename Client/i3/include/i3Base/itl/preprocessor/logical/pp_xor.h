#pragma once

#include "pp_bool.h"
#include "pp_bitxor.h"

#include "detail/pp_xor_impl.h"

#define pp_xor(p, q)		pp_xor_impl(p, q)
