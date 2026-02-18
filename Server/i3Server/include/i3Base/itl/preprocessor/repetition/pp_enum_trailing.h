#pragma once

#include "pp_repeat.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_enum_trailing_impl.h"

#if 0
#define pp_enum_trailing(count, macro, data)
#endif 

#define pp_enum_trailing	pp_enum_trailing_impl
