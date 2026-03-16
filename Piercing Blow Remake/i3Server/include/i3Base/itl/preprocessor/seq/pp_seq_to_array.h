#pragma once

#include "pp_seq_enum.h"		// pp_seq_sizeÆ÷ÇÔ

#include "detail/pp_seq_to_array_impl.h"

#define pp_seq_to_array(seq)		pp_seq_to_array_impl(seq)
