#pragma once


#define pp_iif_impl_res0(t, f)	f 
#define pp_iif_impl_res1(t, f)	t
#define pp_iif_impl0(res)		res
#define pp_iif_impl( bit, t, f )	pp_iif_impl0(pp_iif_impl_res ## bit(t, f))

