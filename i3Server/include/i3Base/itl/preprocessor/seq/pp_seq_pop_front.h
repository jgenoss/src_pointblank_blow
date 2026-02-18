#pragma once

#include "pp_seq.h"

#include "detail/pp_seq_pop_front_impl.h"

#define pp_seq_pop_front(seq)		pp_seq_pop_front_impl(seq)
