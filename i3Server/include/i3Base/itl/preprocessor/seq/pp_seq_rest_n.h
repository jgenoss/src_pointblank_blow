#pragma once

#include "../arithmetic/pp_inc.h"
#include "../facilities/pp_empty.h"
#include "detail/pp_seq_split.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_seq_rest_n_impl.h"

#define pp_seq_rest_n(n, seq)		pp_seq_rest_n_impl(n, seq)
