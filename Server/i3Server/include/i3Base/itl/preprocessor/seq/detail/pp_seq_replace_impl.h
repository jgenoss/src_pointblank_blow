#pragma once

#define pp_seq_replace_impl(seq, i, elem) pp_seq_first_n(i, seq) (elem) pp_seq_rest_n(pp_inc(i), seq)
