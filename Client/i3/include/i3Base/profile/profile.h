#pragma once



#define i3_prof_scope(name, ...)	
#define i3_prof_func(...)			
#define i3_prof_start(name, ...)	
#define i3_prof_end()					

#define i3_prof_reset()					
#define i3_prof_save(filename)			
#define i3_prof_snapshot(filename)		

#define i3_prof_degrade_factor(factor)	
#define i3_prof_degrade_threshold(val)	


#ifdef i3_profile_enable
#include "define_profile.h"
#else
#include "undef_profile.h"
#endif

