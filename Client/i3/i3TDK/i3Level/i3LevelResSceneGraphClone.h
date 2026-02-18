#if !defined( __I3LV_RES_SCENEGRAPH_CLONE_H)
#define __I3LV_RES_SCENEGRAPH_CLONE_H

#include "i3LevelResSceneGraph.h"

// 이 Class는 ResourceDB에 추가되지 않고, Loading시에 생성되는 Res들을 나타낸다.
// Element의 Scale 변경 등과 같은, Physix의 공유가 불가능한 경우에 사용된다.
class I3_EXPORT_TDK i3LevelResSceneGraphClone : public i3LevelRes
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResSceneGraphClone, i3LevelRes);
protected:
	i3LevelResSceneGraph *		m_pSourceRes = nullptr;

	i3LevelElement3D *			m_pElement = nullptr;
public:
	virtual ~i3LevelResSceneGraphClone(void);

	void					setSourceRes( i3LevelResSceneGraph * pRes);
	i3LevelResSceneGraph *	getSourceRes(void)							{ return m_pSourceRes; }

	// Clone Resource는 항상 1개의 Instance Element와 연관을 맺는다.
	void					setElement( i3LevelElement3D * pElm)		{ m_pElement = pElm; }
	i3LevelElement3D *		getElement(void)							{ return m_pElement; }

	virtual bool		OnValidateBrief( bool bForce) override;
	virtual bool		OnValidate( i3LevelScene * pScene, bool bForce) override;
	virtual bool		OnExport( i3LevelScene * pScene, const char * pszPath) override;
	virtual i3GameRes *	CreateGameRes(void) override;
	virtual i3Node *	GetElementAttachNode( i3LevelScene * pScene) override;
};

#endif
