#pragma once

#include "detail/pp_seq_push_back_impl.h"

#define pp_seq_push_back(seq, elem)		pp_seq_push_back_impl(seq, elem)
