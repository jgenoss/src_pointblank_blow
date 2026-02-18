#pragma once

#include "detail/pp_tuple_elem_impl.h"

#define pp_tuple_elem(size, index, tuple)	pp_tuple_elem_impl(size, index, tuple)

