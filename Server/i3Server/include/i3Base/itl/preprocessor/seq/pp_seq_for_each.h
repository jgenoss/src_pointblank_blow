#pragma once

#include "../arithmetic/pp_dec.h"
#include "../repetition/pp_for.h"
#include "pp_seq.h"
#include "pp_seq_size.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_seq_for_each_impl.h"

#define pp_seq_for_each(macro, data, seq)		pp_seq_for_each_impl(macro, data, seq)
#define pp_seq_for_each_r(r, macro, data, seq)	pp_seq_for_each_r_impl(r, macro, data, seq)
