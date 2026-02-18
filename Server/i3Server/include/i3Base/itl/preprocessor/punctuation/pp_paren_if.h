#pragma once

#include "../control/pp_if.h"
#include "../facilities/pp_empty.h"
#include "pp_paren.h"

#include "detail/pp_paren_if_impl.h"

#define pp_lparen_if(cond)		pp_lparen_if_impl(cond)
#define pp_rparen_if(cond)		pp_rparen_if_impl(cond)
