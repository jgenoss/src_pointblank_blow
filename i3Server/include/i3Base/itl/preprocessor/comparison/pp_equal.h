#pragma once

#include "pp_not_equal.h"
#include "../logical/pp_compl.h"

#include "detail/pp_equal_impl.h"

#define pp_equal(x, y)						pp_equal_impl(x, y)

