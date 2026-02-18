#include "StdAfx.h"
#include "GlobalVariable.h"

i3GuiEditorFramework * g_pFramework  = NULL;
i3GuiRoot			 * g_pRoot		 = NULL;

i3GuiControlTemplate * g_pControlTemplate	= NULL;
i3GuiToolResManager	 * g_pResManager		= NULL;
CGuiEditorProfile	 * g_pProfile			= NULL;

extern CPaneResList	 *	g_pResList			= NULL;

void DeleteUnusedObjects()
{
	i3NamedResourceManager * pMng = i3ResourceFile::GetResourceManager( i3Sound::static_meta());
	pMng->DeleteUnusedObjects();

	pMng = i3ResourceFile::GetResourceManager( i3Texture::static_meta());
	pMng->DeleteUnusedObjects();

	pMng = i3ResourceFile::GetResourceManager( i3Animation::static_meta());
	pMng->DeleteUnusedObjects();

	pMng = i3ResourceFile::GetResourceManager( i3Animation2::static_meta());
	pMng->DeleteUnusedObjects();
}
