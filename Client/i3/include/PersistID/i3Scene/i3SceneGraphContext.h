#if !defined( __I3_SG_CONTEXT_H)
#define __I3_SG_CONTEXT_H

#include "i3LayerRootNode.h"
#include "i3Camera.h"
#include "i3ShadowMap.h"
#include "i3ParticleRenderNode.h"

class i3SceneTracer;

class I3_EXPORT_SCENE i3SceneGraphContext : public i3ElementBase
{
	I3_CLASS_DEFINE( i3SceneGraphContext);
protected:
	i3List *				m_pLightList;
	i3List *				m_pCameraList;

	i3LayerRootNode *		m_p3DRoot;
	i3AttrSet *				m_p2DRoot;
	i3LayerRootNode *		m_p2DMainRoot;
	i3Node			*		m_p3DMainRoot;
	i3ParticleRenderNode*	m_pParticleRoot;

	i3ShadowMap *			m_pShadowMap;

	bool					m_bManualBound;
	i3BoundBox				m_BoundBox;

	i3Camera *				m_pCurCamera;

	INT32					m_idxLastSubCamera;

	INT32					m_curPortal;

protected:
	void				_Init2DDefaultAttrs(void);
	void				_BuildShadowMap( i3Light * pLight, i3SceneTracer * pTracer);

	void				_Rec_CalcBound( i3Node * pNode, i3BoundBox * pBound, i3MatrixStack * pStack);

	void				_UpdateRT( i3Camera * pCam, i3SceneTracer * pTracer);

public:
	i3SceneGraphContext(void);
	virtual ~i3SceneGraphContext(void);

	bool				Create(void);

	i3LayerRootNode  *	get3DRoot(void)						{ return m_p3DRoot; }
	i3AttrSet *			get2DRoot(void)						{ return m_p2DRoot; }
	i3LayerRootNode *	get2DMainRoot(void)					{ return m_p2DMainRoot; }
	i3Node			*	get3DMainRoot(void)					{ return m_p3DMainRoot; }

	INT32				getLightCount(void)					{ return m_pLightList->GetCount(); }
	i3Light *			getLight( INT32 idx)				{ return (i3Light *) m_pLightList->Items[idx]; }
	i3List *			getLightList(void)					{ return m_pLightList; }
	void				addLight( i3Light * pLight);
	void				removeLight( i3Light * pLight);
	void				removeAllLights(void);

	INT32				getCameraCount(void)				{ return m_pCameraList->GetCount(); }
	i3Camera *			getCamera( INT32 idx)				{ return (i3Camera *) m_pCameraList->Items[idx]; }
	void				addCamera( i3Camera * pCam);
	void				removeCamera( i3Camera * pCam);
	void				removeAllCameras(void);

	INT32				getCurrentPortal(void)				{ return m_curPortal; }
	void				setCurrentPortal( INT32 id)			{ m_curPortal = id; }

	i3Camera *			getCurrentCamera(void)				{ return m_pCurCamera; }
	void				setCurrentCamera( i3Camera * pCam)	{ I3_REF_CHANGE( m_pCurCamera, pCam); }

	bool				getManualBoundEnable(void)			{ return m_bManualBound; }
	void				setManualBoundEnable( bool bFlag)	{ m_bManualBound = bFlag; }

	void				setBound( VEC3D * pMin, VEC3D * pMax)	{ m_BoundBox.SetMinMax( pMin, pMax); }

	void				CalcBoundWST( i3BoundBox * pBox);

	virtual void		OnUpdate( i3SceneTracer * pTracer);
};

#endif

