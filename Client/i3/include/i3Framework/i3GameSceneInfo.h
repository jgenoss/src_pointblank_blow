#if !defined( _I3_GAME_SCENE_INFO_H__)
#define _I3_GAME_SCENE_INFO_H__

#include "i3GameObj.h"
#include "i3GameRes.h"
#include "i3ResourceManager.h"
#include "i3Object.h"

struct I3_GAMESCENE_INSTANCE
{
	UINT32							_nIndex = 0;		//
	i3::pack::GAME_INSTANCE		_Instance;

	i3ClassMeta *					_pMeta = nullptr;
	i3GameObj	*					_pObject = nullptr;
	i3GameRes	*					_pRes = nullptr;

	UINT32							_nOffsetAdditionalData = 0;
};

struct I3_GAMESCENE_INSTANCE_2
{
	UINT32							_nIndex = 0;		//
	i3::pack::GAME_INSTANCE_2		_Instance;

	i3ClassMeta *					_pMeta = nullptr;
	i3GameObj	*					_pObject = nullptr;
	i3GameRes	*					_pRes = nullptr;

	UINT32							_nOffsetAdditionalData = 0;

	UINT32							pad[8] = { 0 };
};


typedef i3::shared_ptr<i3::vector<i3GameRes*> >	GameResourceListPtr;
typedef i3::shared_ptr<i3::vector<i3GameObj*> > InstanceListPtr;
typedef i3::shared_ptr<i3::vector<i3Object*> >	StageObjectListPtr;

class I3_EXPORT_FRAMEWORK i3GameSceneInfo : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3GameSceneInfo, i3ElementBase);

protected:
	i3::rc_string			m_strStageName;

	i3::pack::GAME_INSTANCE_PACK	m_Pack;
	I3_GAMESCENE_INSTANCE_2*			m_pInstInfo = nullptr;

	GameResourceListPtr		m_spResourceList;	
	InstanceListPtr				m_spInstList;
	StageObjectListPtr		m_spStageObjectList;
	
	i3Framework			*	m_pFramework = nullptr;
	i3ResourceManager	*	m_pResManager = nullptr;

	i3Stream			*	m_pStream = nullptr;
public:
	virtual ~i3GameSceneInfo( void);

	bool				Create( i3Framework * pFramework);
	virtual UINT32		InitInstance(void);

	UINT32				getGameSceneInstanceCount()			{	return m_Pack.m_nInstanceCount; }
	I3_GAMESCENE_INSTANCE_2 *	getGameSceneInstance()		{	return m_pInstInfo; }

	void				setStageName( const char * pszStageName){	m_strStageName = pszStageName; }
	///
	GameResourceListPtr& getResourceList(void)		{	return m_spResourceList;}
	InstanceListPtr&		getInstanceList(void)		{	return m_spInstList;}
	StageObjectListPtr&	getStageObjectList(void)	{	return m_spStageObjectList;}
//	List *				getLayerList(void)			{	return m_LayerList; }

	// 아래의 함수3개는 모두 리스트 원소포인터들의 릴리즈처리가 되지 않는 약점을 그대로 갖고 있다...( 2012.05.15. 수빈)
	void				setResourceList(const GameResourceListPtr& spList)		{	m_spResourceList = spList;	}
	void				setInstanceList(const InstanceListPtr& spList)		{	m_spInstList = spList;  }
	void				setStageObjectList(const StageObjectListPtr& spList)	{	m_spStageObjectList = spList;  }

	void				AddResource( i3GameRes	*	pRes);
	void				AddInstance( i3GameObj * pObj);
	void				AddStageObject( i3Object * pObj);

public:
	UINT32				Save(void);
	virtual bool		Load( const char * pStageName);

	i3Stream	*		getStream( void)				{	return m_pStream;}
	void				setStream( i3Stream * pStream)	{ I3_REF_CHANGE( m_pStream, pStream); }
	INT32				GetObjectPersistID( i3GameObj * pObj);
	i3GameObj	*		FindObjectByIndex( UINT32 nIndex);

	//Resource
	bool				SaveResource(const char * pszFileName);
	bool				LoadResource(const char * pszFileName);

	//Instance
	UINT32				SaveInstance(const char * pszFileName);
	UINT32				LoadInstance(const char * pszFileName);

protected:
	UINT32				_SaveInstance( i3Stream * pStream);
	UINT32				_LoadInstance( i3Stream * pStream);
	UINT32				_LoadInstance2( i3Stream * pStream);
	UINT32				_LoadInstance3( i3Stream * pStream);
	UINT32				_LoadInstance4( i3Stream * pStream);
	UINT32				_LoadInstance5( i3Stream * pStream);

	UINT32				_SaveRsc( i3Stream * pStream);

	void				_SetInstanceMatrix( i3GameObj * pObject, i3::pack::GAME_INSTANCE_2 * pInst);
};

#endif
