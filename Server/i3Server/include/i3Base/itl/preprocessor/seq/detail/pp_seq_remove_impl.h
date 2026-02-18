#pragma once

#define pp_seq_remove_impl(seq, i) pp_seq_first_n(i, seq) pp_seq_rest_n(pp_inc(i), seq)
