#pragma once

#define pp_bitor_impl(x, y)		pp_bitor_i(x, y)
#define pp_bitor_i(x, y)		pp_bitor_id(pp_bitor_ ## x ## y)
#define pp_bitor_id(id)			id

#define pp_bitor_00 0
#define pp_bitor_01 1
#define pp_bitor_10 1
#define pp_bitor_11 1
