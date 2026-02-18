#pragma once

#include "detail/pp_tuple_reverse_impl.h"

#define pp_tuple_reverse(size, tuple)		pp_tuple_reverse_impl(size, tuple)
