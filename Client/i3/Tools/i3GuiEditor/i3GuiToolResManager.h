#pragma once

class i3GuiToolResManager : public i3NamedElement
{
	I3_CLASS_DEFINE( i3GuiToolResManager, i3NamedElement );

public:
	i3GuiToolResManager(void);
	virtual ~i3GuiToolResManager(void);

protected:
//	i3::vector<class i3GuiToolImageElement*> m_pTextureList;	// 쓰이지 않아 주석..
//	List	* m_pAnimationList;								// 쓰이지 않아 주석..
//	List	* m_pSceneGraphList;							// 쓰이지 않아 주석..
//	List	* m_pSoundList;									// 쓰이지 않아 주석..

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
