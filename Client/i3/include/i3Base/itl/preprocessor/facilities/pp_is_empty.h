#pragma once

#include "../pp_cat.h"
#include "../tuple/pp_tuple_elem.h"
#include "pp_empty.h"
#include "../detail/pp_split.h"

#include "detail/pp_is_empty_impl.h"

#define pp_is_empty(x)		pp_is_empty_impl(x)
