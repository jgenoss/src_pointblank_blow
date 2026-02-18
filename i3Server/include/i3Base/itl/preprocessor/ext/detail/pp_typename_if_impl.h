#pragma once

#define pp_typename_if_impl(cond)					pp_if(cond, pp_typename, pp_empty)()
