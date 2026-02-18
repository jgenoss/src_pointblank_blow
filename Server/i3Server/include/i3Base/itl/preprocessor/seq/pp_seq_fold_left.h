#pragma once

#include "../arithmetic/pp_dec.h"
#include "../control/pp_if.h"
#include "../debug/pp_error.h"	// pp_cat
#include "../detail/pp_auto_rec.h"
#include "pp_seq.h"
#include "pp_seq_size.h"

#include "detail/pp_seq_fold_left_impl.h"

#if 0
#define pp_seq_fold_left(op, state, seq) ...
#endif

#define pp_seq_fold_left	pp_seq_fold_left_impl
