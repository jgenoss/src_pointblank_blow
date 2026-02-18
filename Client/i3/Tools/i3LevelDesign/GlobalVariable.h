#if !defined( _GLOBAL_VARIABLE_H__)
#define _GLOBAL_VARIABLE_H__

#include "LevelDesignProfile.h"

extern	CLevelDesignProfile * g_pProfile;
extern i3GameInterface *		g_pInterface;
extern HINSTANCE			g_hInterface;

const char* GetDynStateName(INT32 index);
const char* GetNetSyncModeName(INT32 index);

#endif

