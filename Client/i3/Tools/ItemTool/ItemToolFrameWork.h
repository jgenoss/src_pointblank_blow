#pragma once

#include "ItemTabManager.h"
#include "ItemToolViewer.h"

typedef struct _tagInitBoneInfo
{
	char	m_szName[256];
	MATRIX	m_Matrix;
} INIT_BONE_INFO;

class cItemToolFrameWork : public i3ViewerFramework
{
public:
	I3_CLASS_DEFINE(cItemToolFrameWork, i3ViewerFramework);

	cItemToolFrameWork(void);
	virtual ~cItemToolFrameWork(void);

	virtual void OnCreate();
	virtual void OnUpdate( RT_REAL32 timeDelta);
	

	virtual void	SetSceneGraph( i3Node * pNode, bool bReset = true);

	virtual void	OnMButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnMouseMove(UINT32 nFlag, POINT point);

	void	RefineSg( bool bResetView = true);
	void	SetDynamicState( bool bFlag);
	void	BuildInitBone();
	void	ApplyInitBone();

	static I3SG_TRAVERSAL_RESULT _BuildInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack);
	static I3SG_TRAVERSAL_RESULT _ApplyInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack);

	INT32				getInitBoneCount(void)						{ return (INT32)m_InitBoneList.size(); }
	INIT_BONE_INFO *	getInitBone( INT32 idx)						{ return m_InitBoneList[idx]; }
	INIT_BONE_INFO *	addInitBone( const char * pszBone, MATRIX * pMatrix);
	INIT_BONE_INFO *	findInitBone( const char * pszBone);
	void				removeAllInitBone(void);

	cItemTabManager* GetItemTabManager() { return m_pItemTabManager;}
	
	INT32	GetCurrentTAB() { return (INT32)m_pItemTabManager->GetCurrentTAB(); }
	void	SetCurrentTAB(INT32 nTabIdx) { m_pItemTabManager->SetCurrentTAB((E_ITEM_TABLIST)nTabIdx); }

	void SetSceneGraphTool(i3SceneGraphInfo* pSgInfo) { I3_REF_CHANGE(m_pSgInfo, pSgInfo); }

	eSwapChainType GetType() const { return m_eSwapChainType; }
	void SetType(eSwapChainType val) { m_eSwapChainType = val; }

	virtual void OnResetCameraPosition();

	bool GetIsResetCamera() const { return m_bResetCamera; }
	void SetResetCamera(bool val) { m_bResetCamera = val; }
protected:

	cItemTabManager*	m_pItemTabManager;
	i3SceneGraphInfo*	m_pSgInfo;
	i3Node*				m_pInterLinkNode;
	i3Node*				m_pNormalViewRoot;

	i3::vector<INIT_BONE_INFO*>	m_InitBoneList;

	eSwapChainType m_eSwapChainType;

	bool m_bResetCamera;

};
