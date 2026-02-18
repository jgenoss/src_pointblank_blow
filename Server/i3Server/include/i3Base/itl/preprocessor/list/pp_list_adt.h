#pragma once

#include "../detail/pp_is_binary.h"
#include "../logical/pp_compl.h"
#include "../tuple/pp_tuple_eat.h"

#include "detail/pp_list_adt_impl.h"

#define pp_list_first(list)					pp_list_first_impl(list)
#define pp_list_rest(list)					pp_list_rest_impl(list)
#define pp_list_is_cons(list)				pp_list_is_cons_impl(list)
#define pp_list_is_nil(list)				pp_list_is_nil_impl(list)

