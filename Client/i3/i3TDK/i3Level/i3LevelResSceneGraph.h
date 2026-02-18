#if !defined( __I3LV_RES_SCENEGRAPH_H)
#define __I3LV_RES_SCENEGRAPH_H

#include "i3LevelRes.h"

enum
{
	I3LV_SG_TYPE_NORMAL = 0,
	I3LV_SG_TYPE_BODY,
	I3LV_SG_TYPE_EFFECT
};

class I3_EXPORT_TDK i3LevelResSceneGraph : public i3LevelRes
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResSceneGraph, i3LevelRes);
protected:

	i3SceneGraphInfo *		m_pInfo = nullptr;

	UINT32					m_SgType = I3LV_SG_TYPE_NORMAL;

public:
	i3LevelResSceneGraph(void);
	virtual ~i3LevelResSceneGraph(void);

	i3SceneGraphInfo *		getSceneGraphInfo(void)			{ return m_pInfo; }

	i3Node *				GetSceneRoot(void)
	{
		if( m_pInfo == nullptr)
			return nullptr;

		return m_pInfo->getInstanceSg();
	}

	void					SetSceneRoot( i3Node * pRoot);

	UINT32			getSgType( void)	{	return m_SgType;}

	virtual i3LevelElement3D *	CreateInstance( i3LevelScene * pScene) override;

	virtual i3Node *	GetElementAttachNode( i3LevelScene * pScene) override;

	// 간략한 정보를 Validate 시키기 위해 Scene에서 호출된다.
	// 실제 Resource를 위한 메모리를 할당하는 등의 처리는 하지 않는다.
	virtual bool		OnValidateBrief( bool bForce) override;

	// 실제 Resource를 메모리로 읽어 들이는 단계.
	virtual bool		OnValidate( i3LevelScene * pScene, bool bForce) override;
	virtual i3GameRes *	CreateGameRes(void) override;
	virtual bool		OnSerialize(void) override;
	virtual void		OnChangeScene( i3LevelScene * pScene) override;

	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
};

#endif
