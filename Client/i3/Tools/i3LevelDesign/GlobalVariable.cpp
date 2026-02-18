#include "pch.h"
#include "GlobalVariable.h"

CLevelDesignProfile * g_pProfile = NULL;
i3GameInterface *	g_pInterface = NULL;
HINSTANCE			g_hInterface = NULL;


static const char* gs_dynStateName[]	= { "<None>", "Dynamic,Kinematic","Static","Trigger"};
static const INT32 gs_numDynStateNum	= sizeof(gs_dynStateName) / sizeof(gs_dynStateName[0]);

static const char* gs_netSyncModeName[] = { "(None)", "State Only","Position Only","Transform","Animation" };
static const INT32 gs_numNetSyncModeNum	= sizeof(gs_netSyncModeName) / sizeof(gs_netSyncModeName[0]);


const char* GetDynStateName(INT32 index)
{
	if(gs_numDynStateNum <= index)
		return NULL;

	return gs_dynStateName[index];
}

const char* GetNetSyncModeName(INT32 index)
{
	if(gs_numNetSyncModeNum <= index)
		return NULL;

	return gs_netSyncModeName[index];
}

