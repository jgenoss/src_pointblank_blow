#if !defined( __GLOBAL_VAR_H)
#define __GLOBAL_VAR_H

#include "OptionInfo.h"

extern COptionInfo *		g_pOption;
extern i3PackNode *			g_pPack;
extern CImageList *			g_pSysImageList;
extern bool					g_bVerbose;
extern char					g_szVolumeName[];
extern char					g_szPath[];

void PRINTF( const char *format, ...);

#endif
