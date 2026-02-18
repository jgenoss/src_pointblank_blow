#ifndef __I3_GAME_OBJ_H
#define __I3_GAME_OBJ_H

#include "i3GameRes.h"
#include "i3GameResSceneGraph.h"
#include "i3GameObjBase.h"

struct ShadowSectionInfo {
	INT32 _idx;
	INT32 _Count;
};


// ============================================================================
//
// i3GameObj
//
// ============================================================================

class I3_EXPORT_FRAMEWORK i3GameObj : public i3GameObjBase
{
	I3_EXPORT_CLASS_DEFINE( i3GameObj, i3GameObjBase);
protected:
	// Volatile Members
	i3AttrSet *						m_pFadeCtrl = i3AttrSet::new_object_ref();
	i3MaterialAttr *				m_pFadeMaterialAttr = i3MaterialAttr::new_object_ref();;
	i3BlendEnableAttr *				m_pFadeBlendEnableAttr = i3BlendEnableAttr::new_object_ref();
	i3BlendModeAttr *				m_pFadeBlendModeAttr = i3BlendModeAttr::new_object_ref();

	//Initial Transform State
	bool							m_bHasInitialMatrix = false;
	MATRIX							m_mtxInitial;

	//Initial Dynamic State
	bool							m_bInitialDynamicActive = false;
	I3GAME_DYNAMIC_STATE			m_InitialDynamicState = I3GAME_DYNAMIC_STATE_NONE;

	//Current Dynamic State
	I3GAME_DYNAMIC_STATE			m_nDynamicState = I3GAME_DYNAMIC_STATE_NONE;
	bool							m_bDynamicActive = false;

	i3SceneObject *					m_pSceneObj = i3SceneObject::new_object();

	//
	i3GameObj *						m_pEventSender = nullptr;

	INT32							m_idxPortal = 0;
	INT32							m_cntPortal = 0;

	UINT64							m_idxPortalMask = 1;

	i3::vector<ShadowSectionInfo*> m_vecSection;

	INT32							m_Layer = 0;				// Volatile

	// Dynamic 상태에서는 Matrix Stack이 무의미하며,
	// 임의의 World 위치로 옮겨가 있을 수 있기 때문에
	// 해당 상태인 경우에는 역으로 Simulate된 결과 Matrix를 가져와야 한다.

protected:
#if defined ( I3_PHYSX)
	void			_SetExplosion( i3PhysixShapeSet * pShapeSet, VEC3D * pCenterPos, REAL32 rExplosionRange, REAL32 rExplosionPower );
#endif

	virtual void				_ForceVisible( bool bFlag);

public:
	i3GameObj(void);
	virtual ~i3GameObj(void);

	virtual void	SetInstanceSg( i3Node * pNode) override;			// Internal use only; GameRes에서 호출되는 함수.

	i3SceneObject *		getSceneObject(void)			{ return m_pSceneObj; }
	i3Node *			getAttachNode(void)				{ return m_pFadeCtrl; }

	MATRIX	*		GetInitialMatrix(void)				{	return &m_mtxInitial;}
	void			SetInitialMatrix( MATRIX * pMtx, bool bInit = true);

	INT32			getPortalID(void)				{ return m_idxPortal; }
	void			setPortalID( bool bAdd, INT32 id);

	void			SetPortalMask( bool bAdd, INT32 idx);
	UINT64			getPortalMask(void)				{ return m_idxPortalMask;}

	INT32			getLayer(void)					{ return m_Layer; }
	void			setLayer( INT32 layer)			{ m_Layer = layer; }

	virtual void	OnTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status) {}

	// Position
	MATRIX *		GetMatrix(void)					{ return m_pSceneObj->GetMatrix(); }
	void			SetMatrix(MATRIX  * pMtx);
	i3GameObj *		getEventSender( void )				{ return m_pEventSender; }
	void			setEventSender( i3GameObj * pObj)	{ m_pEventSender = pObj; }

	VEC3D *		GetPos(void)				{ return i3Matrix::GetPos( m_pSceneObj->GetMatrix()); }
	void			SetPos(VEC3D * pVec);
	void			SetPos(REAL32 x, REAL32 y, REAL32 z);
	void			AddPos(REAL32 x, REAL32 y, REAL32 z);
	void			SetPathAnimation( i3Animation * pAnim, I3_ANIM_PLAY_MODE mode);
	void			PlayAnim( i3Animation * pAnim, I3_ANIM_PLAY_MODE mode)
	{
		m_pSceneObj->PlayAnim( pAnim, I3_ANIM_BLEND_MODE_REPLACE, 0.0f, 0.0f, 1.0f, 1.0f ,mode, I3_ANIM_PLAY_MODE_STOP);
	}

	REAL32				getTimeScale(void)							{ return m_pSceneObj->GetTimeScale(); }
	void				setTimeScale( REAL32 scale)					{ m_pSceneObj->SetTimeScale( scale); }

	virtual void		OnBindResource(void) override;
	virtual bool		OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;
	virtual void		OnChangeTime( REAL32 tm) override;
	virtual void		OnInstancing( I3_ONINSTANCING_INFO * pInfo) override;
	virtual void		OnFadeStart(void) override;
	virtual void		OnFading( REAL32 tm, REAL32 rate) override;
	virtual void		OnFadeEnd(void) override;

	void				SetVisibleOnlyScene( bool bTrue);

	virtual void		OnUpdateWorldSectionMask();

	void				OnPreTraceSceneObject();

	virtual bool		IsDropObj() {return false;}

#if defined ( I3_PHYSX)
	//////////////////////////////////////////////////////////////////////////////////
	//								Physix functions								//
	//////////////////////////////////////////////////////////////////////////////////
	I3GAME_DYNAMIC_STATE		getInitialDynamicState(void)		{ return m_InitialDynamicState; }
	void						setInitialDynamicState( I3GAME_DYNAMIC_STATE state)
	{
		m_InitialDynamicState = state;
	}

	bool						getInitialDynamicActiveState(void)	{ return m_bInitialDynamicActive; }
	void						setInitialDynamicActiveState( bool bFlag)
	{
		m_bInitialDynamicActive = bFlag;
	}

	bool						getDynamicActiveState(void)			{ return m_bDynamicActive; }
	void						setDynamicActiveState( bool bFlag, bool bUpdateToNode = true);

	virtual	I3GAME_DYNAMIC_STATE	getDynamicState( void)	{	return m_nDynamicState;}
	virtual void					SetDynamicState( I3GAME_DYNAMIC_STATE state, bool bUpdateToNode = true);

	//	Dynamic On/Off
	//void			PHYSIX_SetDynamicEnable( bool bValue, UINT32 addFlag = 0, UINT32 removeFlag = 0 );
	void			SetDynamicUpdateEnable( bool bEnable);
	// Set force to AllShape
	void			PHYSIX_SetForce( VEC3D * pLinearF, VEC3D * pAngularF );
	// Set force to AllShape by Centerposition
	void			PHYSIX_SetExplosionForce( VEC3D * pCenterPos, REAL32 rExplosionRange, REAL32 rExplosionPower );

	void			PHYSIX_SetSleeping( bool bEnable );

	void			DeleteAllSection();
	ShadowSectionInfo*	FindSection(INT32 idx);
protected:
	void			_UpdateDynamicState( i3Node * pNode);
#endif

public:
	void			EnableUpdateAABB(bool bEnable);
	bool			IsEnabledUpdateAABB() const;

	void			EnableOccludee(bool bEnable);
	bool			IsEnabledOccludee() const;

	virtual void	AttachScene(void) override;
	virtual void	DetachScene(void) override;

private:
	i3Transform*	m_pRenderTransAABB = nullptr;
	bool			m_bEnableRenderAABB = false;

public:
	void			EnableRenderAABB(bool bEnable);
	bool			IsEnabledRenderAABB() const;

};

#endif // __I3_GAME_OBJ_H
