#pragma once

# define pp_is_1_impl(x)		pp_is_empty(pp_cat(pp_is_1_helper_, x))
# define pp_is_1_helper_1
