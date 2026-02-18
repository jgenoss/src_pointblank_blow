#pragma once

#define pp_seq_pop_back_impl(seq) pp_seq_first_n(pp_dec(pp_seq_size(seq)), seq)
