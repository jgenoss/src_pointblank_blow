#pragma once

#include "../slot/pp_slot.h"
#include "../tuple/pp_tuple_elem.h"

#define pp_local_iterate() "i3Base/itl/preprocessor/iteration/detail/pp_local.h"

#define pp_local_c(n) (pp_local_s) <= n && (pp_local_f) >= n
#define pp_local_r(n) (pp_local_f) <= n && (pp_local_s) >= n
