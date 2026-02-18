#pragma once

#define pp_seq_first_n_impl(n, seq)		pp_if(n, pp_tuple_elem, pp_tuple_eat_3)(2, 0, pp_seq_split(n, seq (nil)))
