#pragma once

#define pp_bitxor_impl(x, y)	pp_bitxor_i(x, y)
#define pp_bitxor_i(x, y)		pp_bitxor_id(pp_bitxor_ ## x ## y)
#define pp_bitxor_id(id)		id

#define pp_bitxor_00 0
#define pp_bitxor_01 1
#define pp_bitxor_10 1
#define pp_bitxor_11 0
