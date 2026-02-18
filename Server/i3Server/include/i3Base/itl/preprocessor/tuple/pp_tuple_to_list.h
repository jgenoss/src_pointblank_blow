#pragma once

#include "detail/pp_tuple_to_list_impl.h"

#define pp_tuple_to_list(size, tuple)		pp_tuple_to_list_impl(size, tuple)
