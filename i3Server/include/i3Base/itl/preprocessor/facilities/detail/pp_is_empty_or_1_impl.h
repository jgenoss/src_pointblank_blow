#pragma once

#define pp_is_empty_or_1_impl(x) pp_iif( pp_is_empty(x pp_empty()), 1 pp_empty, pp_is_1)(x)
