#pragma once

#include "../pp_cat.h"
#include "detail/pp_error_impl.h"

#define pp_error(code)						pp_error_impl(code)
