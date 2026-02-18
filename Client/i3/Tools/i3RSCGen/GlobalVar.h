#if !defined ( _GLOBAL_VAR_H__)
#define	_GLOBAL_VAR_H__

#include "i3RSCGenProfile.h"
#include "RSCNode.h"
#include "Resource.h"

#include "PaneState.h"

extern Ci3RSCGenProfile * g_pProfile;
extern RSCNode			* g_pRoot;

extern CPaneState		* g_pState;

#define I3RSC_LOG(s)\
		\
		if( g_pState != NULL)\
		{\
			g_pState->Log(s);\
		};
#endif