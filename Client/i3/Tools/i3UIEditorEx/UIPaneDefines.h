#ifndef __UIPANE_DEFINES_H__
#define __UIPANE_DEFINES_H__

//Auto scroll when mouse hovering near the top edge or the bottom edge
enum TIMER_ID
{
	TIMER_HOVERING = 1,
	TIMER_SCROLLUP,
	TIMER_SCROLLDOWN,
};

#define HOVERING_VALID_RANGE	10
#define HOVERING_TIMEOUT		200
#define SCROLLING_FREQUENCY		100



#endif //__UIPANE_DEFINES_H__