#pragma once

# define pp_seq_cat_impl(seq) pp_if(pp_dec(pp_seq_size(seq)), pp_seq_cat_i, pp_seq_head(seq) pp_tuple_eat_1 )(seq) 

# define pp_seq_cat_i(seq) pp_seq_fold_left(pp_seq_cat_o, pp_seq_head(seq), pp_seq_tail(seq))
# define pp_seq_cat_o(s, st, elem) pp_seq_cat_o_i(st, elem)
# define pp_seq_cat_o_i(a, b) a ## b

# define pp_seq_cat_s_impl(s, seq) pp_if( pp_dec(pp_seq_size(seq)), pp_seq_cat_s_i, pp_seq_head(seq) pp_tuple_eat_2 )(s, seq) 
# define pp_seq_cat_s_i(s, seq) pp_seq_fold_left_ ## s(pp_seq_cat_o, pp_seq_head(seq), pp_seq_tail(seq))
