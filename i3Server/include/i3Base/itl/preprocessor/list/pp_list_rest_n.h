#pragma once

#include "../arithmetic/pp_dec.h"
#include "../control/pp_while.h"
#include "pp_list_adt.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_list_rest_n_impl.h"

#define pp_list_rest_n(count, list)			pp_list_rest_n_impl(count, list)
#define pp_list_rest_n_d(d, count, list)	pp_list_rest_n_d_impl(d, count, list)

