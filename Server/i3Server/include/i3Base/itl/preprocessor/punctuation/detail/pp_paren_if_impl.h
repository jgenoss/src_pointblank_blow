#pragma once

#define pp_lparen_if_impl(cond) pp_if(cond, pp_lparen, pp_empty)()
#define pp_rparen_if_impl(cond) pp_if(cond, pp_rparen, pp_empty)()
