#pragma once

#define pp_is_empty_impl(x)		pp_is_empty_i(pp_is_empty_helper x ())
#define pp_is_empty_i(test)		pp_is_empty_ii(pp_split(0, pp_cat(pp_is_empty_def_, test)))
#define pp_is_empty_ii(id)		id

#define pp_is_empty_helper()				1
#define pp_is_empty_def_1					1, pp_nil
#define pp_is_empty_def_pp_is_empty_helper	0, pp_nil
