#if !defined( __I3_LV_RES_OBJECT_H)
#define __I3_LV_RES_OBJECT_H

#include "i3LevelRes.h"
#include "i3LevelResSceneGraph.h"

class I3_EXPORT_TDK i3LevelResObject : public i3LevelRes
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResObject, i3LevelRes);

protected:
	i3Object *		m_pObject = nullptr;

	i3LevelResSceneGraph *		m_pSgRes = nullptr;

public:
	i3LevelResObject(void);
	virtual ~i3LevelResObject(void);

	i3Object *		getObject(void)					{ return m_pObject; }
	void			setObject( i3Object * pObj);

	virtual i3LevelElement3D *	CreateInstance( i3LevelScene * pScene) override;
	virtual bool				OnValidateBrief( bool bForce) override;
	virtual bool				OnValidate( i3LevelScene * pScene, bool bForce) override;
	virtual i3GameRes *			CreateGameRes(void) override;
	virtual i3Node *			GetElementAttachNode( i3LevelScene * pScene) override;

	virtual UINT32				OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32				OnLoad( i3ResourceFile * pResFile) override;
};

#endif
