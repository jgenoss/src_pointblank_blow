#pragma once

#include "../debug/pp_error.h"				// pp_cat.h°¡ Æ÷ÇÔµÊ..
#include "../detail/pp_auto_rec.h"			// pp_iif.h 
#include "../tuple/pp_tuple_eat.h"

#include "../list/pp_list_fold_left.h"
#include "../list/pp_list_fold_right.h"
#include "../logical/pp_bitand.h"

#include "../control/pp_if.h"

#include "detail/pp_while_impl.h"

#if 0
#define pp_while(pred, op, state)
#endif 

#define pp_while			pp_while_impl
