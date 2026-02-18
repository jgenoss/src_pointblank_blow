#if !defined( __I3_VIEWER2_FRAMEWORK_H)
#define __I3_VIEWER2_FRAMEWORK_H

#include "OptionInfo.h"

enum	I3_VIEWER_EDIT_MODE
{
	I3_VIEWER_EDIT_MODE_NORMAL = 0,
	I3_VIEWER_EDIT_MODE_LIGHT
};

typedef struct _tagInitBoneInfo
{
	char	m_szName[256];
	MATRIX	m_Matrix;
} INIT_BONE_INFO;

class i3Viewer2Framework : public i3ViewerFramework
{
	I3_CLASS_DEFINE( i3Viewer2Framework);
protected:
	I3_VIEWER_EDIT_MODE		m_EditMode;
	i3SceneGraphInfo *		m_pSgInfo;

	i3Node *				m_pSelNode;
	i3Transform *			m_pSelBoundRoot;

	i3ReflectMapBindAttr *	m_pReflectMapAttr;

	i3Transform *			m_pBSPPlane;

	i3Node *				m_pInterLinkNode;

	i3Node *				m_pNormalViewRoot;
	i3AttrSet *				m_pMeshViewRoot;
	i3AttrSet *				m_pCollViewRoot;
	i3AttrSet *				m_pWireViewRoot;

	i3List					m_InitBoneList;

	void	_GetFullTransform( i3Node * pNode, MATRIX * pMtx);

protected:
	// Octree ฐüทร
	i3AttrSet *				m_pOctreeBoundRoot;
	i3VertexArray *			m_pOctreeBoundLineVA;

	void				_CreateOctreeBound(void);
	void				_SelectOctree( i3OcTreeNode * pNode);

protected:
	void			BuildInitBone(void);
	void			ApplyInitBone(void);

	static I3SG_TRAVERSAL_RESULT _BuildInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack);
	static I3SG_TRAVERSAL_RESULT _ApplyInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack);

	INT32				getInitBoneCount(void)						{ return m_InitBoneList.GetCount(); }
	INIT_BONE_INFO *	getInitBone( INT32 idx)						{ return (INIT_BONE_INFO *) m_InitBoneList.Items[idx]; }
	INIT_BONE_INFO *	addInitBone( const char * pszBone, MATRIX * pMatrix);
	INIT_BONE_INFO *	findInitBone( const char * pszBone);
	void				removeAllInitBone(void);

	void				_SetIsolateMode( bool bFlag);

public:
	i3Viewer2Framework(void);
	virtual ~i3Viewer2Framework(void);

	i3SceneGraphInfo *	getSgInfo(void)				{ return m_pSgInfo; }
	void				setSgInfo( i3SceneGraphInfo * pSgInfo);

	bool			CloseI3S(void);
	bool			LoadI3S( const char * pszPath);
	bool			SaveI3S( const char * pszPath);

	void			RefineSg( bool bResetView = true);

	bool			LoadAnimation( const char * pszPath);

	void			ApplyOpition(void);
	void			RecordCam( INT32 idx);
	void			SetCamPos( INT32 idx);

	void			SelectNode( i3Node * pNode);
	void			UnselectNode( i3Node * pNode);

	void			SetLight( COLORREAL * pDiff, COLORREAL * pSpec, INT32 shadowSize);

	I3_VIEWER_EDIT_MODE		getEditMode(void)		{ return m_EditMode; }
	void					setEditMode( I3_VIEWER_EDIT_MODE mode);

	void					setViewMode( VIEWER_MODE mode, bool bReset = false);

	void			SetGlowEnable( BOOL bFlag);

	//////////////////////////////////////////
	void			SetDynamicState( bool bFlag);

	virtual void	OnCreate(void);
	void			OnDestroy(void);
};

#endif
