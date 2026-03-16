#pragma once

#include "pp_seq_first_n.h"
#include "pp_seq_rest_n.h"

#include "detail/pp_seq_insert_impl.h"

#define pp_seq_insert(seq, i, elem)		pp_seq_insert_impl(seq, i, elem)
