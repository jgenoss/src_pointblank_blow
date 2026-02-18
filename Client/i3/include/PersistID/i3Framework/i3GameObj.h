#ifndef __I3_GAME_OBJ_H
#define __I3_GAME_OBJ_H

#include "i3GameRes.h"
#include "i3GameResSceneGraph.h"
#include "i3GameObjBase.h"



// ============================================================================
//
// i3GameObj
//
// ============================================================================

class I3_EXPORT_FRAMEWORK i3GameObj : public i3GameObjBase
{
	I3_CLASS_DEFINE( i3GameObj);
protected:
	i3SceneObject *					m_pSceneObj;
	
	// Volatile Members
	i3AttrSet *						m_pFadeCtrl;
	i3MaterialAttr *				m_pFadeMaterialAttr;
	i3BlendEnableAttr *				m_pFadeBlendEnableAttr;
	i3BlendModeAttr *				m_pFadeBlendModeAttr;

	//Initial Transform State
	bool							m_bHasInitialMatrix;
	MATRIX							m_mtxInitial;

	//Initial Dynamic State
	bool							m_bInitialDynamicActive;
	I3GAME_DYNAMIC_STATE			m_InitialDynamicState;

	//Current Dynamic State
	I3GAME_DYNAMIC_STATE			m_nDynamicState;
	bool							m_bDynamicActive;

	//
	i3GameObj *						m_pEventSender;

	INT32							m_idxPortal;
	INT32							m_idxPrevPortal;

	// Dynamic 상태에서는 Matrix Stack이 무의미하며,
	// 임의의 World 위치로 옮겨가 있을 수 있기 때문에
	// 해당 상태인 경우에는 역으로 Simulate된 결과 Matrix를 가져와야 한다.

protected:
#if defined ( I3_PHYSX)
	void			_SetExplosion( i3PhysixShapeSet * pShapeSet, VEC3D * pCenterPos, REAL32 rExplosionRange, REAL32 rExplosionPower );
#endif

	void						_SetWorldSectionIndex( INT32 idx);

	virtual void				_ForceVisible( BOOL bFlag);

public:
	i3GameObj(void);
	virtual ~i3GameObj(void);

	virtual void	SetInstanceSg( i3Node * pNode);			// Internal use only; GameRes에서 호출되는 함수.

	i3SceneObject *		getSceneObject(void)			{ return m_pSceneObj; }
	i3Node *			getAttachNode(void)				{ return m_pFadeCtrl; }

	MATRIX	*		GetInitialMatrix(void)				{	return &m_mtxInitial;}
	void			SetInitialMatrix( MATRIX * pMtx, bool bInit = true);

	INT32				getPortalID(void)				{ return m_idxPortal; }
	void				setPortalID( INT32 id)			{ m_idxPortal = id; }

	// Position
	RT_MATRIX *		GetMatrix(void)					{ return m_pSceneObj->GetMatrix(); }
	void			SetMatrix( RT_MATRIX  * pMtx);

	i3GameObj *		getEventSender( void )				{ return m_pEventSender; }

	RT_VEC3D *		GetPos(void)				{ return i3Matrix::GetPos( m_pSceneObj->GetMatrix()); }
	void			SetPos( RT_VEC3D * pVec);
	void			SetPos( RT_REAL32 x, RT_REAL32 y, RT_REAL32 z);
	void			AddPos( RT_REAL32 x, RT_REAL32 y, RT_REAL32 z);

	void			SetPathAnimation( i3Animation * pAnim, I3_ANIM_PLAY_MODE mode);
	void			PlayAnim( i3Animation * pAnim, I3_ANIM_PLAY_MODE mode)
	{
		m_pSceneObj->PlayAnim( pAnim, I3_ANIM_BLEND_MODE_REPLACE, 0.0f, 0.0f, 1.0f, mode, I3_ANIM_PLAY_MODE_STOP);
	}

	REAL32				getTimeScale(void)							{ return m_pSceneObj->GetTimeScale(); }
	void				setTimeScale( REAL32 scale)					{ m_pSceneObj->SetTimeScale( scale); }

	virtual void		OnBindResource(void);
	virtual BOOL		OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);
	virtual void		OnChangeTime( REAL32 tm);
	virtual void		OnInstancing( void * pUserDefault = NULL);
	virtual void		OnFadeStart(void);
	virtual void		OnFading( REAL32 tm, REAL32 rate);
	virtual void		OnFadeEnd(void);

	void				SetVisibleOnlyScene( BOOL bTrue);

	virtual void		OnUpdateWorldSection(void);

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
	//void			PHYSIX_SetDynamicEnable( BOOL bValue, UINT32 addFlag = 0, UINT32 removeFlag = 0 );
	void			SetDynamicUpdateEnable( bool bEnable);
	// Set force to AllShape
	void			PHYSIX_SetForce( VEC3D * pLinearF, VEC3D * pAngularF );
	// Set force to AllShape by Centerposition
	void			PHYSIX_SetExplosionForce( VEC3D * pCenterPos, REAL32 rExplosionRange, REAL32 rExplosionPower );

	void			PHYSIX_SetSleeping( BOOL bEnable );

protected:
	void			_UpdateDynamicState( i3Node * pNode);
#endif

};

#endif // __I3_GAME_OBJ_H
