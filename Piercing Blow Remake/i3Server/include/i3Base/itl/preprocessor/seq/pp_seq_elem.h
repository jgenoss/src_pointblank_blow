#pragma once

#include "../pp_cat.h"
#include "../facilities/pp_empty.h"

#include "detail/pp_seq_elem_impl.h"

#define pp_seq_elem(i, seq)		pp_seq_elem_impl(i, seq)
