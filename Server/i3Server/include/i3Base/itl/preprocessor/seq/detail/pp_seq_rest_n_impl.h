#pragma once

#define pp_seq_rest_n_impl(n, seq) pp_tuple_elem(2, 1, pp_seq_split(pp_inc(n), (nil) seq pp_empty))()
