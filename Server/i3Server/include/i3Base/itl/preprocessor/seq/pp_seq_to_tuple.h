#pragma once

#include "pp_seq_enum.h"

#include "detail/pp_seq_to_tuple_impl.h"

#define pp_seq_to_tuple(seq)		pp_seq_to_tuple_impl(seq)
