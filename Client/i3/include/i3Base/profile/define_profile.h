
// #pragma once나 인클루드가드는 사용하지 않는다..  매번 적용 가능하도록 해야한다..

#undef i3_profile_enable
#undef i3_prof_scope
#undef i3_prof_func
#undef i3_prof_start
#undef i3_prof_end

#undef i3_prof_reset
#undef i3_prof_save
#undef i3_prof_snapshot

#undef i3_prof_degrade_factor
#undef i3_prof_degrade_threshold	

//

#include "profile_core.h"	// 디폴트로 항상 들어가는게 좋다..

#define i3_profile_enable

#define i3_prof_scope(name, ...)	::i3::profile::scope _prof_(name, __VA_ARGS__)
#define i3_prof_func(...)			i3_prof_scope(__FUNCSIG__, __VA_ARGS__)
#define i3_prof_start(name, ...)	::i3::profile::manager::i()->start(name, __VA_ARGS__)
#define i3_prof_end()						::i3::profile::manager::i()->end()

#define i3_prof_reset()						::i3::profile::manager::i()->reset()
#define i3_prof_save(filename)				::i3::profile::manager::i()->save(filename, false)
#define i3_prof_snapshot(filename)			::i3::profile::manager::i()->save(filename, true)

#define i3_prof_degrade_factor(factor)		::i3::profile::manager::i()->set_degrade_factor(factor)
#define i3_prof_degrade_threshold(val)		::i3::profile::manager::i()->set_degrade_threshold(val)
