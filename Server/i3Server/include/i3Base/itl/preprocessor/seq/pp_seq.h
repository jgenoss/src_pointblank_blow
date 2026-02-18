#pragma once

#include "pp_seq_elem.h"

#include "detail/pp_seq_impl.h"

#define	pp_seq_head(seq)	pp_seq_head_impl(seq)
#define pp_seq_tail(seq)	pp_seq_tail_impl(seq)
#define pp_seq_nil(x)		(x)
