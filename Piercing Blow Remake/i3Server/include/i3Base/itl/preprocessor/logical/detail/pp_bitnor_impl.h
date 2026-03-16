#pragma once

#define pp_bitnor_impl(x, y)	pp_bitnor_i(x, y)
#define pp_bitnor_i(x, y)		pp_bitnor_id(pp_bitnor_ ## x ## y)
#define pp_bitnor_id(id) id

#define pp_bitnor_00 1
#define pp_bitnor_01 0
#define pp_bitnor_10 0
#define pp_bitnor_11 0
