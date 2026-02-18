#pragma once

#include "../arithmetic/pp_dec.h"
#include "pp_seq_first_n.h"
#include "pp_seq_size.h"

#include "detail/pp_seq_pop_back_impl.h"

#define pp_seq_pop_back(seq)		pp_seq_pop_back_impl(seq)
