#include "StdAfx.h"
#include "i3UIToolResManager.h"

I3_CLASS_INSTANCE( i3UIToolResManager, i3NamedElement );


i3UIToolResManager::i3UIToolResManager(void)
{
}

i3UIToolResManager::~i3UIToolResManager(void)
{
}

BOOL i3UIToolResManager::_IsLoaded( const char * pszName )
{
	return TRUE;
}

INT32	i3UIToolResManager::AddResource( i3Texture * pTexture )
{
	return 0;
}

INT32	i3UIToolResManager::AddResource( i3Animation * pAnimation )
{
	return 0;
}

INT32	i3UIToolResManager::AddResource( i3SceneObject * pSceneObject )
{
	return 0;
}
//
//INT32	i3GuiToolResManager::AddResource( i3Sound * pSound )
//{
//	return 0;
//}


INT32	i3UIToolResManager::LoadTexture( const char * pszFileName)
{
	return 0;
}

INT32	i3UIToolResManager::LoadAnimation( const char * pszFileName)
{
	return 0;
}

INT32	i3UIToolResManager::LoadSceneGraph( const char * pszFileName)
{
	return 0;
}
//
//INT32	i3GuiToolResManager::LoadSound( const char * pszFileName)
//{
//	return 0;
//}


i3Texture		*	i3UIToolResManager::GetTexture( INT32 nIndex)
{
	return NULL;
}

i3Animation		*	i3UIToolResManager::GetAnimation( INT32 nIndex)
{
return NULL;
}
i3SceneObject	*	i3UIToolResManager::GetSceneGraph( INT32 nIndex)
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
struct tag_i3UIToolRes
{
	char	m_ID[4]; //
	INT32	m_nVer;
	INT32	m_nTemplateCount;
	INT32	m_nTextureCount;
	INT32	m_nAnimationCount;
	INT32	m_nSceneCount;
};

#if defined ( I3_COMPILER_VC )
#pragma pack(pop)
#endif

BOOL	i3UIToolResManager::SaveResource( const char * pszFileName )
{
	return TRUE;
}

BOOL	i3UIToolResManager::LoadResource( const char * pszFileName )
{
	return TRUE;
}

BOOL	i3UIToolResManager::RemoveTexture( i3Texture * pTexture )
{
	return TRUE;
}

BOOL	i3UIToolResManager( i3Animation * pAnimation)
{
	return TRUE;
}

BOOL i3UIToolResManager::RemoveSceneObject( i3SceneObject * pScene)
{
	return TRUE;
}

//BOOL i3GuiToolResManager::RemoveSound( i3Sound * pSound)
//{
//	return TRUE;
//}


BOOL i3UIToolResManager::RemoveAllResources()
{
	return TRUE;
}
