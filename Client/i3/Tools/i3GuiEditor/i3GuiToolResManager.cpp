#include "StdAfx.h"
#include ".\i3guitoolresmanager.h"

I3_CLASS_INSTANCE( i3GuiToolResManager); //, i3NamedElement );


i3GuiToolResManager::i3GuiToolResManager(void)
{
}

i3GuiToolResManager::~i3GuiToolResManager(void)
{
}

BOOL i3GuiToolResManager::_IsLoaded( const char * pszName )
{
	return TRUE;
}

INT32	i3GuiToolResManager::AddResource( i3Texture * pTexture )
{
	return 0;
}

INT32	i3GuiToolResManager::AddResource( i3Animation * pAnimation )
{
	return 0;
}

INT32	i3GuiToolResManager::AddResource( i3SceneObject * pSceneObject )
{
	return 0;
}
//
//INT32	i3GuiToolResManager::AddResource( i3Sound * pSound )
//{
//	return 0;
//}


INT32	i3GuiToolResManager::LoadTexture( const char * pszFileName)
{
	return 0;
}

INT32	i3GuiToolResManager::LoadAnimation( const char * pszFileName)
{
	return 0;
}

INT32	i3GuiToolResManager::LoadSceneGraph( const char * pszFileName)
{
	return 0;
}
//
//INT32	i3GuiToolResManager::LoadSound( const char * pszFileName)
//{
//	return 0;
//}


i3Texture		*	i3GuiToolResManager::GetTexture( INT32 nIndex)
{
	return NULL;
}

i3Animation		*	i3GuiToolResManager::GetAnimation( INT32 nIndex)
{
return NULL;
}
i3SceneObject	*	i3GuiToolResManager::GetSceneGraph( INT32 nIndex)
{
	return NULL;
}

//i3Sound			*	i3GuiToolResManager::GetSound( INT32 nIndex)
//{
//	return NULL;
//}

#if defined ( I3_COMPILER_VC )
#pragma pack( push, 4)
#endif

namespace pack
{
	struct GuiToolRes
	{
		char	m_ID[4] = { 0 }; //
		INT32	m_nVer = 0;
		INT32	m_nTemplateCount = 0;
		INT32	m_nTextureCount = 0;
		INT32	m_nAnimationCount = 0;
		INT32	m_nSceneCount = 0;
	};
}

#if defined ( I3_COMPILER_VC )
#pragma pack(pop)
#endif

BOOL	i3GuiToolResManager::SaveResource( const char * pszFileName )
{
	return TRUE;
}

BOOL	i3GuiToolResManager::LoadResource( const char * pszFileName )
{
	return TRUE;
}

BOOL	i3GuiToolResManager::RemoveTexture( i3Texture * pTexture )
{
	return TRUE;
}

BOOL	i3GuiToolResManagerRemoveAnimation( i3Animation * pAnimation)
{
	return TRUE;
}

BOOL i3GuiToolResManager::RemoveSceneObject( i3SceneObject * pScene)
{
	return TRUE;
}

//BOOL i3GuiToolResManager::RemoveSound( i3Sound * pSound)
//{
//	return TRUE;
//}


BOOL i3GuiToolResManager::RemoveAllResources()
{
	return TRUE;
}
