#pragma once

#include "detail/pp_tuple_rem_impl.h"

#define pp_tuple_rem(size)					pp_tuple_rem_impl(size)
#define pp_tuple_rem_ctor(size, tuple)		pp_tuple_rem_ctor_impl(size, tuple)

