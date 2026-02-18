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
#include "i3Base/pool/class_object_pool.h"

#include "i3Math/structure/frustum_plane.h"

#define		I3_VFC_LEFT			0x00000001
#define		I3_VFC_RIGHT		0x00000002
#define		I3_VFC_TOP			0x00000004
#define		I3_VFC_BOTTOM		0x00000008
#define		I3_VFC_NEAR			0x00000010
#define		I3_VFC_FAR			0x00000020
#define		I3_VFC_ALL			0x0000003F

#define		I3SG_TRACER_STYLE_NO_VIEWMATRIX		0x00000001

#define		I3SG_TRACER_STATE_OFFSCREEN			0x00000001
#define		I3SG_TRACER_STATE_GEN_SHADOW		0x00000002

struct I3SG_TRACER_CONTEXT
{
	MATRIX					m_ViewMatrix;
	MATRIX					m_InvViewMatrix;
	MATRIX					m_ProjectionMatrix;
	REAL32					m_Time = 0.0f;
	bool					m_bFrustCull = false;
};

struct i3AlphaOBB
{
	VEC3D	m_center;
	VEC3D	m_extent;
	VEC3D	m_axis[3];
};

struct i3AlphaSortNodeSet : i3::class_object_pool<i3AlphaSortNodeSet>
{
	i3Node*					m_pAlphaRoot = nullptr;
	INT32					m_startNodeAttrIndex = 0;
	i3RenderAttrListAttr*	m_NodeAttrList = nullptr;
	REAL32					m_fMinDepth = 0.0f;				// 최저 깊이..
	REAL32					m_fMaxDepth = 0.0f;				// 최고 깊이..
	i3AlphaOBB				m_OBB;
};

namespace i3 { class aabb; }

class I3_EXPORT_SCENE i3SceneTracer : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3SceneTracer, i3ElementBase);

private:

protected:
	UINT32						m_Style = 0;
	UINT32						m_State = 0;
	i3AttrStackManager *		m_pAttrStackManager;		// [initialize at constructor]
	i3RenderContext *			m_pContext = nullptr;
	i3RenderAttrListAttr *		m_pOutput;					// [initialize at constructor]
	i3RenderAttrListAttr *		m_pDefOutput;				// [initialize at constructor]
	i3MatrixStack *				m_pMatrixStack;				// [initialize at constructor]
	i3ObjectPool *				m_pModelViewMatrixAttrPool;	// [initialize at constructor]
	i3CollisionContext *		m_pCollContext = nullptr;
	i3SceneGraphContext *		m_pSgContext = nullptr;
	i3SoundContext *			m_pSoundCtx = nullptr;
#if defined ( I3_PHYSX)
	i3PhysixContext *			m_pPhysixCtx = nullptr;
#endif

	UINT32						m_DisableFlag = I3_NODEFLAG_DISABLE;
	
	i3::vector<i3RenderAttrListAttr*>	m_OutputList;

	I3MATRIXINFO 				m_MatrixInfo;

	REAL32						m_Time = 0.0f;
	REAL32						m_fManualTime = 0.0f;
	i3Camera *					m_pCurCamera = nullptr;

	MATRIX					m_ViewMatrix;
	MATRIX					m_InvViewMatrix;
	MATRIX					m_ProjectionMatrix;
	MATRIX					m_ViewAndProjectMatrix;
	bool						m_bFrustCull = true;

	VEC4D					m_FrustPlane[6];
	VEC4D					m_OrgFrustPlane[6];

	UINT32						m_PlaneMask = 0;
	UINT32						m_FrameID = 0;
	bool						m_bCulled = false;

	i3RefStack					m_AnimCtxStack;
	i3RefStack					m_BoneMatrixStack;

	i3::vector<i3::aabb*>		m_AABBStack;			// 이런게 필요해서 만든다.. (여기서 벡터의 원소는 각노드에서 쓰는 일련의 업데이트함수의 변수 - 일단은 각노드에서 동적할당된 것만 생각..)

#if !defined( I3_NO_PROFILE)
	INT32						m_NodeCount = 0;
#endif

	i3::vector<i3AlphaSortNodeSet*>	m_pAlphaSortNodeSetPool;
	i3::vector<i3AlphaSortNodeSet*> m_pAlphaSortNoseSetListForDelete;
	i3::vector<i3AlphaSortNodeSet*>	m_pAlphaSortNodeSetList;
	bool							m_bActivateAlphaSortNode = false;

	i3::frustum_plane				m_FrustumPlane;

	//i3TimeSequenceInfo의 이벤트 시간을 수동으로 조절할지 결정
	bool						m_bManualTimeUpdate = false;
	
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
	bool					IsStyle( UINT32 style)				{ return (m_Style & style) != 0; }

	UINT32					getDisableNodeMask(void)			{ return m_DisableFlag; }
	void					setDisableNodeMask( UINT32 flag)	{ m_DisableFlag = flag; }
	void					addDisableNodeMask( UINT32 mask)	{ m_DisableFlag |= mask; }
	void					removeDisableNodeMask( UINT32 mask)	{ m_DisableFlag &= ~mask; }

	UINT32					getState(void)						{ return m_State; }
	void					setState( UINT32 state)				{ m_State = state; }
	void					addState( UINT32 state)				{ m_State |= state; }
	void					removeState( UINT32 state)			{ m_State &= ~state; }
	bool					isState( UINT32 state)				{ return (m_State & state) != 0; }

	void					SetTime( REAL32	tm)					{ m_Time = tm; }
	REAL32					GetTime(void)						{ return m_Time; }

	void					SetManualTime( REAL32	tm)			{ m_fManualTime = tm; }
	REAL32					GetManualTime(void)					{ return m_fManualTime; }

	i3RenderAttrListAttr *	GetOutput(void)						{ return m_pOutput; }
	i3MatrixStack *			GetModelViewMatrixStack(void)		{ return m_pMatrixStack ;}
	i3AttrStackManager *	GetAttrStackManager(void)			{ return m_pAttrStackManager; }

	i3ModelViewMatrixAttr *	AllocModelViewMatrixAttr(void)		{ return (i3ModelViewMatrixAttr *) m_pModelViewMatrixAttrPool->Alloc(); }

	void					SetCamera( i3Camera * pCamera);
	void					RestoreCamera( i3Camera * pCam);
	i3Camera *				getCurrentCamera(void)				{ return m_pCurCamera; }

	void					SetViewMatrix( MATRIX * pViewMat, MATRIX * pInvViewMat);
	MATRIX *				GetViewMatrix(void)				{ return &m_ViewMatrix; }

	MATRIX *				GetInverseViewMatrix(void)				{ return &m_InvViewMatrix; }
	void					SetProjectionMatrix( MATRIX * pMat);

	MATRIX *				getViewProjectionMatrix(void)			{ return &m_ViewAndProjectMatrix; }

	MATRIX *				GetProjectionMatrix(void)				{ return & m_ProjectionMatrix; }
	VEC4D *				GetFrustumPlane( INT32 Idx)				{ return & (m_FrustPlane[Idx]); }

	I3MATRIXINFO *			GetMatrixEnvironment(void)				{ return &m_MatrixInfo; }

	void					SetFrustumCullEnable( bool bFlag)		{ m_bFrustCull = bFlag; }
	bool					GetFrustumCullEnable(void)				{ return m_bFrustCull; }

	void					pushAttr( i3RenderAttr * pAttr, INT32 prio = I3_DEF_ATTR_PRIORITY)
	{
		m_pAttrStackManager->PushAttr( pAttr, prio);
	}

	void					popAttr( INT32 id, INT32 prio = I3_DEF_ATTR_PRIORITY)
	{
		m_pAttrStackManager->PopAttr( id, prio);
	}
	
	void					InitAlphaSortNodeList();
	void					ActivateAlphaSortNode( i3Node* node);
	void					DeactivateAlphaSortNode( i3Node* node);
	void					ResetAlphaSortNodeList();
	void					ResetAlphaSortNodeListForDelete();

	void					DestroyAlphaSortNodeList();
	i3::vector<i3AlphaSortNodeSet*>& GetAlphaSortNodeSetList() { return m_pAlphaSortNodeSetList; }

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

	void					EnableAttrs( INT32 * pIDTable, INT32 count, bool bFlag)
	{
		m_pAttrStackManager->EnableAttrs( pIDTable, count, bFlag);
	}

	i3AnimationContext *	getCurrentAnimationContext(void)		{ return (i3AnimationContext *) m_AnimCtxStack.getTop(); }
	void					PushAnimationContext( i3AnimationContext * pCtx)
	{
		m_AnimCtxStack.Push( pCtx);
	}
	void					PopAnimationContext( i3AnimationContext ** pCtx = nullptr)
	{
		m_AnimCtxStack.Pop( (i3ElementBase **) pCtx);
	}

	i3BoneMatrixListAttr *	getCurrentBoneMatrixList(void)		{ return (i3BoneMatrixListAttr *) m_BoneMatrixStack.getTop(); }
	void					PushBoneMatrixList( i3BoneMatrixListAttr * pCtx)
	{
		m_BoneMatrixStack.Push( pCtx);
	}
	void					PopBoneMatrixList( i3BoneMatrixListAttr ** pCtx = nullptr)
	{
		m_BoneMatrixStack.Pop( (i3ElementBase **) pCtx);
	}

	// 스택변수를 넣게 되는 것에 유의할 것..
	void					PushAABBIntoStack(i3::aabb* a);
	i3::aabb*				PopAABBFromStack();				// 없으면 nullptr리턴...
	i3::aabb*				TopAABBInStack();				// 없으면 nullptr리턴...	// pop된 aabb의 경우, 그다음번 top의 aabb에 merge처리를 해준다.



	void					SaveContext( I3SG_TRACER_CONTEXT * pCtx);
	void					RestoreContext( I3SG_TRACER_CONTEXT * pCtx);

	void					OutputListReset(void);
	void					Reset(void);
	void					Trace( i3Node * pRoot);
	void					PreTrace(i3Node* pRoot);

	#if !defined( I3_NO_PROFILE)
	INT32					getNodeCount(void)					{ return m_NodeCount; }
	void					addNodeCount(void)					{ m_NodeCount++; }
	#endif
	
	bool					IsManualTimeUpdate() const		{ return m_bManualTimeUpdate; }
	void					SetIsManualTimeUpdate(bool val)		{ m_bManualTimeUpdate = val; }

	const i3::frustum_plane&	GetFrustumPlane() const { return m_FrustumPlane;  }

	friend class i3Node;
};

#endif
