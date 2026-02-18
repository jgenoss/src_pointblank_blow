#pragma once

#define pp_seq_subseq_impl(seq, i, len) pp_seq_first_n(len, pp_seq_rest_n(i, seq))
