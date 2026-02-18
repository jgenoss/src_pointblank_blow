#if !defined( __I3_VIEWER2_FRAMEWORK_H)
#define __I3_VIEWER2_FRAMEWORK_H

#include "i3Viewer2Axis.h"
#include "i3Viewer2LightAxis.h"
#include "OptionInfo.h"
#include "Panel_SceneGraph.h"

enum	I3_VIEWER_EDIT_MODE
{
	I3_VIEWER_EDIT_MODE_NORMAL = 0,
	I3_VIEWER_EDIT_MODE_LIGHT
};

struct INIT_BONE_INFO : i3::class_common_pool<INIT_BONE_INFO>
{
	char	m_szName[256] = { 0 };
	MATRIX	m_Matrix;
};

static i3Transform *			m_pSelBoundRoot = NULL;

static void ResetBoundMat(i3Node* pNode);
void * getResetBoundMatFunc(void);

static i3Geometry * pGeoX = NULL;
static i3Geometry * pGeoY = NULL;
static i3Geometry * pGeoZ = NULL;
static i3GeometryAttr * pGeoAttrX = NULL;
static i3GeometryAttr * pGeoAttrY = NULL;
static i3GeometryAttr * pGeoAttrZ = NULL;



class i3Viewer2Framework : public i3ViewerFramework
{
	I3_CLASS_DEFINE( i3Viewer2Framework, i3ViewerFramework);
private:
	REAL32 TransFormZ;
protected:
	I3_VIEWER_EDIT_MODE		m_EditMode;
	i3SceneGraphInfo *		m_pSgInfo;

	i3Node *				m_pSelNode;
	

	i3ReflectMapBindAttr *	m_pReflectMapAttr;

	i3Transform *			m_pBSPPlane;

	i3Node *				m_pInterLinkNode;

	i3Node *				m_pNormalViewRoot;
	i3AttrSet *				m_pMeshViewRoot;
	i3AttrSet *				m_pCollViewRoot;
	i3AttrSet *				m_pWireViewRoot;

	i3::vector<INIT_BONE_INFO*>	m_InitBoneList;

	i3Viewer2Axis *			m_pAxis;
	i3Viewer2LightAxis *	m_pLightAxis;

	void	_GetFullTransform( i3Node * pNode, MATRIX * pMtx);

protected:
	// Octree ฐüทร
	i3AttrSet *				m_pOctreeBoundRoot;
	i3VertexArray *			m_pOctreeBoundLineVA;

	void				_CreateOctreeBound(void);
	void				_SelectOctree( i3OcTreeNode * pNode);

	void			_RenderAxis( i3Node * pNode);

protected:
	void			BuildInitBone(void);
	void			ApplyInitBone(void);

	static I3SG_TRAVERSAL_RESULT _BuildInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack);
	static I3SG_TRAVERSAL_RESULT _ApplyInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack);

	INT32				getInitBoneCount(void)						{ return (INT32)m_InitBoneList.size(); }
	INIT_BONE_INFO *	getInitBone( INT32 idx)						{ return m_InitBoneList[idx]; }
	INIT_BONE_INFO *	addInitBone( const char * pszBone, MATRIX * pMatrix);
	INIT_BONE_INFO *	findInitBone( const char * pszBone);
	void				removeAllInitBone(void);

	void				_SetIsolateMode( bool bFlag);
	void				GetFullTransform( i3Node * pNode, MATRIX * pMatrix);
	void				ResizeAxisByBound(i3Transform * pTrans);
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

	void			ApplyOption(void);
	void			ApplyOptionColors(void);
	void			ApplyOptionShadow(void);
	bool			ApplyOptionReflectMap(void);
	void			ApplyOptionCamera(void);
	void			RecordCam( INT32 idx);
	void			SetCamPos( INT32 idx);

	void			SelectNode( i3Node * pNode);
	void			UnselectNode( i3Node * pNode);

	void			SetLight( COLORREAL * pDiff, COLORREAL * pSpec, INT32 shadowSize);

	void			Optimize(i3SceneOptimizer * pOpt, CSceneGraphTreeCtrl * pSceneGraphCtrl);

	I3_VIEWER_EDIT_MODE		getEditMode(void)		{ return m_EditMode; }
	void					setEditMode( I3_VIEWER_EDIT_MODE mode);

	void					setViewMode( VIEWER_MODE mode, bool bReset = false);

	//////////////////////////////////////////
	void			SetDynamicState( bool bFlag);

	virtual void	OnCreate(void) override;
	virtual void	OnUpdate(REAL32 timeDelta) override;

	virtual void	OnMouseWheel( UINT nFlag, short zDelta, POINT point) override;
	virtual void	UpdateLightTx() override;

	void			ResizeAxis();
	void			RePositionLightAxis();
	void			ResizeLightAxis();
	

	bool			m_bLightAxisVisible;
	void			SetLightAxisVisible(bool bVisible);
	bool			IsLightAxisVisible()				{return m_bLightAxisVisible;}

	REAL32			GetLightRho()						{ return m_LightRho; }
	void			SetLightRho(REAL32 fVal);
	REAL32			GetLightTheta()						{ return m_LightTheta; }
	void			SetLightTheta(REAL32 fVal);
};

#endif
