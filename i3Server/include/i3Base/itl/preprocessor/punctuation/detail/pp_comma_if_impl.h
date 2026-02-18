#pragma once


#define pp_comma_if_impl(cond)		pp_if(cond, pp_comma, pp_empty)()
