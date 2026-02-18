#if !defined( __GLOBAL_VAR_H)
#define __GLOBAL_VAR_H

#include "OptionInfo.h"
#include "OptConfig.h"

extern i3Viewer	*					g_pViewer;
extern i3ViewerFramework *			g_pFramework;
extern COptionInfo*					g_pOption;
extern COptConfig *					g_pOptConfig;
extern i3::vector<i3::rc_string>	g_FileList;
extern	CWnd *						g_pRenderPanel;
extern bool							g_bProcessing;
extern i3TDKEnvSet *				g_pEnvSet;

struct OptClassInfo
{
	char						m_szName[128];
	INT32						m_idxImage;
	i3SceneOptimizer::CLASS		m_Class;

};

extern OptClassInfo		g_ClassTbl[];

INT32 FindClassTblByClass( INT32 c);

#endif
