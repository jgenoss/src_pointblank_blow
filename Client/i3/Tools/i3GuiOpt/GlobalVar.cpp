#include "stdafx.h"
#include "OptionInfo.h"
#include "OptConfig.h"
#include "GlobalVar.h"

COptionInfo *				g_pOption = NULL;
i3Viewer *					g_pViewer = NULL;
i3ViewerFramework *			g_pFramework = NULL;
COptConfig *				g_pOptConfig = NULL;
i3::vector<i3::rc_string>	g_FileList;
CWnd *						g_pRenderPanel = NULL;
bool						g_bProcessing = false;
i3TDKEnvSet *				g_pEnvSet = NULL;

OptClassInfo		g_ClassTbl[ i3SceneOptimizer::CLASS_MAX] =
{
	{ "Geometry",			I3TDK_IMAGELIST_GEOMETRY,	i3SceneOptimizer::CLASS_GEOMETRY },
	{ "Scene-Graph",		I3TDK_IMAGELIST_NODE,		i3SceneOptimizer::CLASS_SCENEGRAPH },
	{ "Texture",			I3TDK_IMAGELIST_TEXTURE,	i3SceneOptimizer::CLASS_TEXTURE },
	{ "Animation",			I3TDK_IMAGELIST_ANIM,		i3SceneOptimizer::CLASS_ANIMATION },
	{ "Sound",				I3TDK_IMAGELIST_SOUND,		i3SceneOptimizer::CLASS_SOUND },
	{ "Collision & Physics",I3TDK_IMAGELIST_BONE,		i3SceneOptimizer::CLASS_PHYSICS },
	{ "N/A",				I3TDK_IMAGELIST_RESOURCE,	i3SceneOptimizer::CLASS_NA },
};

INT32 FindClassTblByClass( INT32 c)
{
	INT32 i;

	for( i = 0; i < i3SceneOptimizer::CLASS_MAX; i++)
	{
		if( g_ClassTbl[i].m_Class == c)
			return i;
	}

	return -1;
}
