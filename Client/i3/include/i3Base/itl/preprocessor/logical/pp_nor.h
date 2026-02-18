#pragma once

#include "pp_bool.h"
#include "pp_bitnor.h"

#include "detail/pp_nor_impl.h"

#define pp_nor(p, q)	pp_nor_impl(p, q)
