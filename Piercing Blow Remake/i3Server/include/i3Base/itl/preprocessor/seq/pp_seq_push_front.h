#pragma once

#include "detail/pp_seq_push_front_impl.h"

#define pp_seq_push_front(seq, elem)		pp_seq_push_front_impl(seq, elem)
