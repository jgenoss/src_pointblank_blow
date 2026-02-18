#pragma once

#include "../control/pp_if.h"
#include "../facilities/pp_empty.h"
#include "pp_typename.h"

#include "detail/pp_typename_if_impl.h"

#define pp_typename_if(cond)				pp_typename_if_impl(cond)
