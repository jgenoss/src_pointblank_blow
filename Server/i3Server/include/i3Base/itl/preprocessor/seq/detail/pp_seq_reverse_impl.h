#pragma once

#define pp_seq_reverse_impl(seq)			pp_seq_fold_left(pp_seq_reverse_o, pp_empty, seq)()
#define pp_seq_reverse_o(s, state, elem)	(elem) state
#define pp_seq_reverse_s_impl(s, seq)		pp_seq_fold_left_ ## s(pp_seq_reverse_o, pp_empty, seq)()
