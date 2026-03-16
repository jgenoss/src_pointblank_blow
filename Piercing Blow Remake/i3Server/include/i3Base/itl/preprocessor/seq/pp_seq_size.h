#pragma once

#include "../pp_cat.h"
#include "../tuple/pp_tuple_eat.h"

#include "detail/pp_seq_size_impl.h"

#define pp_seq_size(seq)		pp_seq_size_impl(seq)
