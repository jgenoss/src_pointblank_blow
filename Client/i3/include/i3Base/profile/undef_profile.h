
//
// #pragma once나 인클루드가드는 사용하지 않는다..  매번 적용 가능하도록 해야한다..
//

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

#define i3_prof_scope(name, ...)		__noop
#define i3_prof_func(...)				__noop
#define i3_prof_start(name, ...)		__noop
#define i3_prof_end()			__noop

#define i3_prof_reset()			__noop	
#define i3_prof_save(filename)	__noop
#define i3_prof_snapshot(filename) __noop

#define i3_prof_degrade_factor(factor)		__noop
#define i3_prof_degrade_threshold(val)		__noop

