#pragma once

#include "../punctuation/pp_comma_if.h"
#include "../repetition/pp_repeat.h"

#include "detail/pp_enum_datas_impl.h"

#define pp_enum_datas(count, data)			pp_enum_datas_impl(count, data)
