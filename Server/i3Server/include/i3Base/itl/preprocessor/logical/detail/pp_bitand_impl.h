#pragma once


#define pp_bitand_impl(x, y)		pp_bitand_i(x, y)
#define pp_bitand_i(x, y)			pp_bitand_id(pp_bitand_ ## x ## y)
#define pp_bitand_id(res)			res
#define pp_bitand_00 0
#define pp_bitand_01 0
#define pp_bitand_10 0
#define pp_bitand_11 1

