#include "StdAfx.h"
#include "GlobalVariable.h"

i3GuiEditorFramework * g_pFramework  = NULL;

i3UIControlTemplate * g_pControlTemplate	= NULL;
i3UIToolResManager	 * g_pResManager		= NULL;
CUIEditorProfile	 * g_pProfile			= NULL;

extern CPaneResList	 *	g_pResList			= NULL;

void DeleteUnusedObjects()
{
	i3NamedResourceManager * pMng = i3ResourceFile::GetResourceManager( i3Sound::GetClassMeta());
	pMng->DeleteUnusedObjects();

	pMng = i3ResourceFile::GetResourceManager( i3Texture::GetClassMeta());
	pMng->DeleteUnusedObjects();

	pMng = i3ResourceFile::GetResourceManager( i3Animation::GetClassMeta());
	pMng->DeleteUnusedObjects();

	pMng = i3ResourceFile::GetResourceManager( i3Animation2::GetClassMeta());
	pMng->DeleteUnusedObjects();
}
