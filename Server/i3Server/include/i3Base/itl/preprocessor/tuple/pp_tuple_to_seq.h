#pragma once

#include "detail/pp_tuple_to_seq_impl.h"

#define pp_tuple_to_seq(size, tuple)	pp_tuple_to_seq_impl(size, tuple)
