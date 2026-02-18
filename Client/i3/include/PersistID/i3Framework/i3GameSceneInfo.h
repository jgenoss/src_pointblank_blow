#if !defined( _I3_GAME_SCENE_INFO_H__)
#define _I3_GAME_SCENE_INFO_H__

#include "i3GameObj.h"
#include "i3GameRes.h"
#include "i3ResourceManager.h"
#include "i3Object.h"

typedef struct __tagi3GameSceneInstance
{
	UINT32							_nIndex;		//
	I3_PERSIST_GAME_INSTANCE		_Instance;

	i3ClassMeta *					_pMeta;
	i3GameObj	*					_pObject;
	i3GameRes	*					_pRes;

	UINT32							_nOffsetAdditionalData;

	inline void Init()
	{
		_nIndex = _nOffsetAdditionalData = 0;
		_pMeta = NULL;
		_pObject = NULL;
		_pRes = NULL;
		i3mem::FillZero( &_Instance, sizeof( _Instance));
	};
}I3_GAMESCENE_INSTANCE;

class I3_EXPORT_FRAMEWORK i3GameSceneInfo : public i3ElementBase
{
	I3_CLASS_DEFINE( i3GameSceneInfo);

protected:
	char					m_szStageName[MAX_PATH];

	I3_PERSIST_GAME_INSTANCE_PACK	m_Pack;
	I3_GAMESCENE_INSTANCE*			m_pInstInfo;

	i3List				*	m_pResourceList;
	i3List				*	m_pInstList;

	i3List				*	m_pStageObjectList;

	i3Framework			*	m_pFramework;
	i3ResourceManager	*	m_pResManager;

	i3Stream			*	m_pStream;
public:
	i3GameSceneInfo( void);
	virtual ~i3GameSceneInfo( void);

	BOOL				Create( i3Framework * pFramework);
	virtual UINT32		InitInstance(void);

	void				setStageName( const char * pszStageName){	i3String::Copy( m_szStageName, pszStageName);}
	///
	i3List		*		getResourceList(void)	{	return m_pResourceList;}
	i3List		*		getInstanceList(void)	{	return m_pInstList;}
	i3List		*		getStageObjectList(void){	return m_pStageObjectList;}

	void				setResourceList(i3List * pList)		{	I3_REF_CHANGE( m_pResourceList, pList);}
	void				setInstanceList(i3List * pList)		{	I3_REF_CHANGE( m_pInstList, pList);}
	void				setStageObjectList(i3List * pList)	{	I3_REF_CHANGE( m_pStageObjectList, pList);}

	void				AddResource( i3GameRes	*	pRes);
	void				AddInstance( i3GameObj * pObj);
	void				AddStageObject( i3Object * pObj);

public:
	UINT32				Save(void);
	virtual BOOL		Load( const char * pStageName);

	i3Stream	*		getStream( void)				{	return m_pStream;}
	void				setStream( i3Stream * pStream)	{ I3_REF_CHANGE( m_pStream, pStream); }
	INT32				GetObjectPersistID( i3GameObj * pObj);
	i3GameObj	*		FindObjectByIndex( UINT32 nIndex);
	i3GameObj *			FindObjectNameHash( UINT32 code);

	//Resource
	BOOL				SaveResource(const char * pszFileName);
	BOOL				LoadResource(const char * pszFileName);

	//Instance
	UINT32				SaveInstance(const char * pszFileName);
	UINT32				LoadInstance(const char * pszFileName);

protected:
	UINT32				_SaveInstance( i3Stream * pStream);
	UINT32				_LoadInstance( i3Stream * pStream);
	UINT32				_LoadInstance2( i3Stream * pStream);
	UINT32				_LoadInstance3( i3Stream * pStream);

	UINT32				_SaveRsc( i3Stream * pStream);
	UINT32				__SaveElement( i3Stream * pStream, i3GameRes * pRes);
};

#endif
