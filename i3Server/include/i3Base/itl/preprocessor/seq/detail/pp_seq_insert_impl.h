#pragma once

#define pp_seq_insert_impl(seq, i, elem) pp_seq_first_n(i, seq) (elem) pp_seq_rest_n(i, seq)
