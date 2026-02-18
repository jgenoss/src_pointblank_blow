#pragma once

#include "../arithmetic/pp_inc.h"
#include "../control/pp_while.h"
#include "pp_list_adt.h"
#include "../tuple/pp_tuple_elem.h"

#include "detail/pp_list_size_impl.h"

#define	pp_list_size(list)		pp_list_size_impl(list)	
#define pp_list_size_d(list)	pp_list_size_d_impl(list)
