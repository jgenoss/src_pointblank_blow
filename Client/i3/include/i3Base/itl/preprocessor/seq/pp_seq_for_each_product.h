#pragma once

#include "../arithmetic/pp_dec.h"
#include "../repetition/pp_for.h"		// pp_if Æ÷ÇÔ
#include "pp_seq.h"
#include "pp_seq_size.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_seq_for_each_product_impl.h"

#define pp_seq_for_each_product(macro, sets)			pp_seq_for_each_product_impl(macro, sets)
#define pp_seq_for_each_product_r(r, macro, sets)		pp_seq_for_each_product_r_impl(r, macro, sets)
