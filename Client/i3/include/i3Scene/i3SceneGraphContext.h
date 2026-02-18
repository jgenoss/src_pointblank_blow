#if !defined( __I3_SG_CONTEXT_H)
#define __I3_SG_CONTEXT_H

#include "i3LayerRootNode.h"
#include "i3Camera.h"
#include "i3ShadowMap.h"
#include "i3ParticleRenderNode.h"

#include "i3Base/smart_ptr/shared_ptr_fwd.h"
class i3SceneTracer;
class i3XrayScene;

namespace i3 {	typedef i3::shared_ptr<class occ_culler >	occ_culler_ptr; }

class I3_EXPORT_SCENE i3SceneGraphContext : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3SceneGraphContext, i3ElementBase);
protected:
	i3LightListPtr						m_pLightList;
	i3CameraListPtr						m_pCameraList;

	i3LayerRootNode *		m_p3DRoot = nullptr;
	i3AttrSet *				m_p2DRoot = nullptr;
	i3AttrSet *				m_pDebugTextRoot = nullptr;
	i3LayerRootNode *		m_p2DMainRoot = nullptr;
	i3Node			*		m_p3DMainRoot = nullptr;
	i3ParticleRenderNode*	m_pParticleRoot = nullptr;

	i3ShadowMap *			m_pShadowMap = nullptr;

	i3AmbientAttr *			m_pAmbient = nullptr;

	bool					m_bManualBound = false;
	i3BoundBox				m_BoundBox;
	i3BoundBox				m_GlobalBoundBox;

	i3Camera *				m_pCurCamera = nullptr;

	INT32					m_idxLastSubCamera = -1;

	INT32					m_curPortal = 0;
	UINT64					m_curPortalMask = 0;

	UINT64					m_ShadowMask = 0;
	UINT64					m_VisibleMask = 0;

	i3::occ_culler_ptr		m_occ_culler;
	i3XrayScene*			m_pXrayScene = nullptr;

protected:
	void				_Init2DDefaultAttrs(void);
	void				_BuildShadowMap( i3Light * pLight, i3SceneTracer * pTracer);

	void				_Rec_CalcBound( i3Light * pLight, i3Node * pNode, i3BoundBox * pBound, i3MatrixStack * pStack);

	void				_UpdateRT( i3Camera * pCam, i3SceneTracer * pTracer);

public:
	virtual ~i3SceneGraphContext(void);

	bool				Create(void);

	i3LayerRootNode  *	get3DRoot(void)						{ return m_p3DRoot; }
	i3AttrSet *			get2DRoot(void)						{ return m_p2DRoot; }
	i3AttrSet *			getDebugTextRoot(void)				{ return m_pDebugTextRoot;	}
	i3LayerRootNode *	get2DMainRoot(void)					{ return m_p2DMainRoot; }
	i3Node			*	get3DMainRoot(void)					{ return m_p3DMainRoot; }
	void				addDebugLayer(void);

	void				reset(void);

	INT32				getLightCount(void)					{ return (INT32)m_pLightList->size(); }
	i3Light *			getLight( INT32 idx)				{ return (*m_pLightList)[idx]; }
	i3LightListPtr&		getLightList(void)				{ return m_pLightList; }
	void				addLight( i3Light * pLight);
	void				removeLight( i3Light * pLight);
	void				removeAllLights(void);

	INT32				getCameraCount(void)				{ return (INT32)m_pCameraList->size(); }
	i3Camera *			getCamera( INT32 idx)				{ return (*m_pCameraList)[idx]; }
	void				addCamera( i3Camera * pCam);
	void				removeCamera( i3Camera * pCam);
	void				removeAllCameras(void);

	INT32				getCurrentPortal(void)				{ return m_curPortal; }
	void				setCurrentPortal( INT32 id)			{ m_curPortal = id; }

	UINT64				getCurrentPortalMask(void)			{ return m_curPortalMask; }
	void				setCurrentPortalMask( UINT64 id)	{ m_curPortalMask = id;}
	void				AddCurrentPortalMask( bool bAdd, INT32 idx);

	void				setCurrentPortalMasks( UINT64 shadow, UINT64 visibility)	{	m_ShadowMask = shadow;	m_VisibleMask = visibility;}
	UINT64				getPortalShadowMask(void)			{ return m_ShadowMask;}
	UINT64				getPortalVisibleMask(void)			{ return m_VisibleMask;}

	i3Camera *			getCurrentCamera(void)				{ return m_pCurCamera; }
	void				setCurrentCamera( i3Camera * pCam)	{ I3_REF_CHANGE( m_pCurCamera, pCam); }

	const COLORREAL *	getAmbient(void)					{ return m_pAmbient->Get(); }
	void				setAmbient( COLORREAL * pCol)		{ m_pAmbient->Set( pCol); }

	bool				getManualBoundEnable(void)			{ return m_bManualBound; }
	void				setManualBoundEnable( bool bFlag)	{ m_bManualBound = bFlag; }

	void				setBound( VEC3D * pMin, VEC3D * pMax)	{ m_BoundBox.SetMinMax( pMin, pMax); }

	void				UpdateGlobalBound( bool bShadow);

	void				CalcBoundWST( i3Light * pLight, i3BoundBox * pBox);

	virtual void		OnUpdate( i3SceneTracer * pTracer);

	void				SetOccCuller(i3::occ_culler_ptr culler) { m_occ_culler = culler; }
	i3::occ_culler*		GetOccCuller() const { return m_occ_culler.get();  }

	i3XrayScene*		GetXrayScene() const { return m_pXrayScene; }
};

#endif

