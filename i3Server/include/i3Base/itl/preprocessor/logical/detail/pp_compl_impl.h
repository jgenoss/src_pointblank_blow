#pragma once

#define pp_compl_impl(x)				pp_compl_i(x)
#define pp_compl_i(x)					pp_compl_id(pp_compl_ ## x)
#define pp_compl_id(id)					id
#define pp_compl_0 1
#define pp_compl_1 0
