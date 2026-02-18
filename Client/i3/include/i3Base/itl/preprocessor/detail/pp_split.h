#pragma once

#define pp_split(n, im)		pp_split_i(n((im)))
#define pp_split_i(n)		pp_split_id(pp_split_ii_ ## n)
#define pp_split_ii_0(s)	pp_split_id(pp_split_0 s)
#define pp_split_ii_1(s)	pp_split_id(pp_split_1 s)
#define pp_split_id(id)		id

#define pp_split_0(a, b)	a
#define pp_split_1(a, b)	b
