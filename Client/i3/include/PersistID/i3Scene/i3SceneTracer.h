#if !defined( __I3_SCENE_TRACER_H)
#define __I3_SCENE_TRACER_H

#include "i3Base.h"
#include "i3SoundH.h"
#include "i3AttrStack.h"
#include "i3AttrID.h"
#include "i3AttrStackManager.h"
#include "i3ModelViewMatrixAttr.h"
#include "i3Math.h"
#include "i3Camera.h"
#include "i3CollisionContext.h"
#include "i3SceneGraphContext.h"
#include "i3AttrSet.h"

#if defined (I3_PHYSX)
	#include "i3PhysixContext.h"
#endif

#include "i3AnimationContext.h"
#include "i3BoneMatrixListAttr.h"

#define		I3_VFC_LEFT			0x00000001
#define		I3_VFC_RIGHT		0x00000002
#define		I3_VFC_TOP			0x00000004
#define		I3_VFC_BOTTOM		0x00000008
#define		I3_VFC_NEAR			0x00000010
#define		I3_VFC_FAR			0x00000020
#define		I3_VFC_ALL			0x0000003F

#define		I3SG_TRACER_STYLE_NO_VIEWMATRIX		0x00000001

#define		I3SG_TRACER_STATE_OFFSCREEN			0x00000001

typedef struct _tagi3SceneTracerContext
{
	RT_MATRIX					m_ViewMatrix;
	RT_MATRIX					m_InvViewMatrix;
	RT_MATRIX					m_ProjectionMatrix;
	REAL32						m_Time;
	BOOL						m_bFrustCull;
} I3SG_TRACER_CONTEXT;

class I3_EXPORT_SCENE i3SceneTracer : public i3ElementBase
{
	I3_CLASS_DEFINE( i3SceneTracer);

private:

protected:
	UINT32						m_Style;
	UINT32						m_State;
	i3AttrStackManager *		m_pAttrStackManager;
	i3RenderContext *			m_pContext;
	i3RenderAttrListAttr *		m_pOutput;
	i3RenderAttrListAttr *		m_pDefOutput;
	i3MatrixStack *				m_pMatrixStack;
	i3ObjectPool *				m_pModelViewMatrixAttrPool;
	i3CollisionContext *		m_pCollContext;	
	i3SceneGraphContext *		m_pSgContext;
	i3SoundContext *			m_pSoundCtx;
#if defined ( I3_PHYSX)
	i3PhysixContext *			m_pPhysixCtx;
#endif

	UINT32						m_DisableFlag;
	
	i3List						m_OutputList;

	I3MATRIXINFO 				m_MatrixInfo;

	REAL32						m_Time;
	i3Camera *					m_pCurCamera;

	RT_MATRIX					m_ViewMatrix;
	RT_MATRIX					m_InvViewMatrix;
	RT_MATRIX					m_ProjectionMatrix;
	RT_MATRIX					m_ViewAndProjectMatrix;
	BOOL						m_bFrustCull;

	RT_VEC4D					m_FrustPlane[6];
	RT_VEC4D					m_OrgFrustPlane[6];

	UINT32						m_PlaneMask;
	UINT32						m_FrameID;
	bool						m_bCulled;

	i3RefStack					m_AnimCtxStack;
	i3RefStack					m_BoneMatrixStack;

#if !defined( I3_NO_PROFILE)
	INT32						m_NodeCount;
#endif

public:
	i3SceneTracer(void);
	virtual ~i3SceneTracer(void);

	i3RenderContext *		GetRenderContext(void)					{ return m_pContext; }
	void					SetRenderContext( i3RenderContext * pContext);

	i3SceneGraphContext *	getSgContext(void)							{ return m_pSgContext; }
	void					setSgContext( i3SceneGraphContext * pCtx);

	i3CollisionContext *	GetCollisionContext(void)			{ return m_pCollContext; }
	void					SetCollisionContext( i3CollisionContext * pContext)
	{
		m_pCollContext = pContext;
	}

#if defined ( I3_PHYSX)
	i3PhysixContext *		GetPhysixContext(void)			{ return m_pPhysixCtx; }
	void					SetPhysixContext( i3PhysixContext * pContext)
	{
		m_pPhysixCtx = pContext;
	}
#endif

	i3SoundContext *		getSoundContext(void)				{ return m_pSoundCtx; }
	void					setSoundContext( i3SoundContext * pCtx)
	{
		m_pSoundCtx = pCtx;
	}

	UINT32					GetStyle(void)						{ return m_Style; }
	void					SetStyle( UINT32 style)				{ m_Style = style; }
	void					AddStyle( UINT32 style)				{ m_Style |= style; }
	void					RemoveStyle( UINT32 style)			{ m_Style &= ~style; }
	BOOL					IsStyle( UINT32 style)				{ return (m_Style & style) != 0; }

	UINT32					getDisableNodeMask(void)			{ return m_DisableFlag; }
	void					setDisableNodeMask( UINT32 flag)	{ m_DisableFlag = flag; }
	void					addDisableNodeMask( UINT32 mask)	{ m_DisableFlag |= mask; }
	void					removeDisableNodeMask( UINT32 mask)	{ m_DisableFlag &= ~mask; }

	UINT32					getState(void)						{ return m_State; }
	void					setState( UINT32 state)				{ m_State = state; }
	void					addState( UINT32 state)				{ m_State |= state; }
	void					removeState( UINT32 state)			{ m_State &= ~state; }
	BOOL					isState( UINT32 state)				{ return (m_State & state) != 0; }


	void					SetTime( REAL32	tm)					{ m_Time = tm; }
	REAL32					GetTime(void)						{ return m_Time; }

	i3RenderAttrListAttr *	GetOutput(void)						{ return m_pOutput; }
	i3MatrixStack *			GetModelViewMatrixStack(void)		{ return m_pMatrixStack ;}
	i3AttrStackManager *	GetAttrStackManager(void)			{ return m_pAttrStackManager; }

	i3ModelViewMatrixAttr *	AllocModelViewMatrixAttr(void)		{ return (i3ModelViewMatrixAttr *) m_pModelViewMatrixAttrPool->Alloc(); }

	void					SetCamera( i3Camera * pCamera);
	void					RestoreCamera( i3Camera * pCam);
	i3Camera *				getCurrentCamera(void)				{ return m_pCurCamera; }

	void					SetViewMatrix( RT_MATRIX * pViewMat, RT_MATRIX * pInvViewMat);
	RT_MATRIX *				GetViewMatrix(void)				{ return &m_ViewMatrix; }

	RT_MATRIX *				GetInverseViewMatrix(void)				{ return &m_InvViewMatrix; }
	void					SetProjectionMatrix( RT_MATRIX * pMat);

	RT_MATRIX *				getViewProjectionMatrix(void)			{ return &m_ViewAndProjectMatrix; }

	RT_MATRIX *				GetProjectionMatrix(void)				{ return & m_ProjectionMatrix; }
	RT_VEC4D *				GetFrustumPlane( INT32 Idx)				{ return & (m_FrustPlane[Idx]); }

	I3MATRIXINFO *			GetMatrixEnvironment(void)				{ return &m_MatrixInfo; }

	void					SetFrustumCullEnable( BOOL bFlag)		{ m_bFrustCull = bFlag; }
	BOOL					GetFrustumCullEnable(void)				{ return m_bFrustCull; }

	void					pushAttr( i3RenderAttr * pAttr, INT32 prio = I3_DEF_ATTR_PRIORITY)
	{
		m_pAttrStackManager->PushAttr( pAttr, prio);
	}

	void					popAttr( INT32 id, INT32 prio = I3_DEF_ATTR_PRIORITY)
	{
		m_pAttrStackManager->PopAttr( id, prio);
	}

	void					AddAttributeToOutput( i3RenderAttr * pAttr)
	{
		if( m_pAttrStackManager->isEnabled())
		{
			m_pAttrStackManager->SetFlushState( pAttr->GetID());

			#if defined( NO_DISPLAYLIST_RENDER)
				pAttr->Apply( m_pContext);
			#else
				m_pOutput->AppendAttr( pAttr);
			#endif
		}
	}

	void					flushPushedAttrs( i3RenderAttrListAttr * pList)
	{
		m_pAttrStackManager->FlushPushedAttrs( pList);
	}

	void					PushOutputList( i3RenderAttrListAttr * pNewOutputList);
	void					PopOutputList(void);

	bool					IsInsideFrustum( i3BoundBox * pBox);
	INT32					IsInsideFrustum( REAL32 size, VEC3D * vCenter);
	UINT32					GetPlaneMask(void)						{ return m_PlaneMask; }
	void					SetPlaneMask( UINT32 mask)				{ m_PlaneMask = mask; }

	bool					getCulledState(void)					{ return m_bCulled; }
	void					setCulledState( bool bFlag)				{ m_bCulled = bFlag; }

	bool					isAttrStackManagerEnabled(void)			{ return m_pAttrStackManager->isEnabled(); }
	void					setAttrStackManagerEnable( bool bFlag)	{ m_pAttrStackManager->setEnable( bFlag); }

	UINT32					getFrameID(void)						{ return m_FrameID; }

	void					EnableAttrs( INT32 * pIDTable, INT32 count, BOOL bFlag)
	{
		m_pAttrStackManager->EnableAttrs( pIDTable, count, bFlag);
	}

	i3AnimationContext *	getCurrentAnimationContext(void)		{ return (i3AnimationContext *) m_AnimCtxStack.getTop(); }
	void					PushAnimationContext( i3AnimationContext * pCtx)
	{
		m_AnimCtxStack.Push( pCtx);
	}
	void					PopAnimationContext( i3AnimationContext ** pCtx = NULL)
	{
		m_AnimCtxStack.Pop( (i3ElementBase **) pCtx);
	}

	i3BoneMatrixListAttr *	getCurrentBoneMatrixList(void)		{ return (i3BoneMatrixListAttr *) m_BoneMatrixStack.getTop(); }
	void					PushBoneMatrixList( i3BoneMatrixListAttr * pCtx)
	{
		m_BoneMatrixStack.Push( pCtx);
	}
	void					PopBoneMatrixList( i3BoneMatrixListAttr ** pCtx = NULL)
	{
		m_BoneMatrixStack.Pop( (i3ElementBase **) pCtx);
	}


	void					SaveContext( I3SG_TRACER_CONTEXT * pCtx);
	void					RestoreContext( I3SG_TRACER_CONTEXT * pCtx);

	void					OutputListReset(void);
	void					Reset(void);
	void					Trace( i3Node * pRoot);

	#if !defined( I3_NO_PROFILE)
	INT32					getNodeCount(void)					{ return m_NodeCount; }
	void					addNodeCount(void)					{ m_NodeCount++; }
	#endif

	friend class i3Node;
};

#endif
