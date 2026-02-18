#pragma once

#include "pp_bool.h"
#include "pp_compl.h"

#include "detail/pp_not_impl.h"

#define pp_not(x)							pp_not_impl(x)
