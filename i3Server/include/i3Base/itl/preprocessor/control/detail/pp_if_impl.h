#pragma once


#define pp_if_impl(cond, t, f)		pp_iif( pp_bool(cond), t, f)
