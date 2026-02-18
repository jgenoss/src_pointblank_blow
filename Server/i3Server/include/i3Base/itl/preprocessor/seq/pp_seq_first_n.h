#pragma once

#include "../control/pp_if.h"
#include "detail/pp_seq_split.h"
#include "../tuple/pp_tuple_eat.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_seq_first_n_impl.h"

#define pp_seq_first_n(n, seq)		pp_seq_first_n_impl(n, seq)
