#pragma once

class i3UIToolResManager : public i3NamedElement
{
	I3_CLASS_DEFINE( i3UIToolResManager );

public:
	i3UIToolResManager(void);
	virtual ~i3UIToolResManager(void);

protected:
	i3List	* m_pTextureList;
	i3List	* m_pAnimationList;
	i3List	* m_pSceneGraphList;
	i3List	* m_pSoundList;

protected:
	BOOL	_IsLoaded( const char * pszName );

public:
	INT32	AddResource( i3Texture * pTexture );
	INT32	AddResource( i3Animation * pAnimation );
	INT32	AddResource( i3SceneObject * pSceneObject );
//	INT32	AddResource( i3Sound * pSound );

	INT32	LoadTemplate( const char * pszFileName );
	INT32	LoadTexture( const char * pszFileName);
	INT32	LoadAnimation( const char * pszFileName);
	INT32	LoadSceneGraph( const char * pszFileName);
//	INT32	LoadSound( const char * pszFileName);

	i3Texture		*	GetTexture( INT32 nIndex);
	i3Animation		*	GetAnimation( INT32 nIndex);
	i3SceneObject	*	GetSceneGraph( INT32 nIndex);
//	i3Sound			*	GetSound( INT32 nIndex);

public:
	BOOL	RemoveTexture( i3Texture * pTexture );
	BOOL	RemoveAnimation( i3Animation * pAnimation);
	BOOL	RemoveSceneObject( i3SceneObject * pScene);
//	BOOL	RemoveSound( i3Sound * pSound);

	BOOL	RemoveAllResources();

public:
	BOOL	SaveResource( const char * pszFileName );
	BOOL	LoadResource( const char * pszFileName );
};
