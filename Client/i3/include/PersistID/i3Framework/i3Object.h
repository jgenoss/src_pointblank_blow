#if !defined( __I3_OBJECT_H)
#define __I3_OBJECT_H

#include "i3GameObj.h"
#include "i3GameResSceneGraph.h"

class i3GameResObject;

typedef struct _tagi3ObjectStateInfo
{
	REAL32					m_HPRatio;
	i3TimeEventGen *		m_pTimeEvent;
	i3SceneGraphInfo *		m_pSg;
	i3GameResSceneGraph *	m_pRes;

	// Context
	i3BinList *				m_pBoneList;
	i3Body *				m_pBody;

} I3_OBJECT_STATE;

enum	I3_OBJECT_PARAM_FIRST
{
	I3_OBJECT_PARAM_F0 = 0,
	I3_OBJECT_PARAM_F1,
	I3_OBJECT_PARAM_F2,
	I3_OBJECT_PARAM_F3,
	I3_OBJECT_PARAM_F4,
	I3_OBJECT_PARAM_F5,
	I3_OBJECT_PARAM_F6,

	I3_OBJECT_PARAM_FCOUNT,
};

enum	I3_OBJECT_PARAM_SECOND
{
	I3_OBJECT_PARAM_S0 = 0,
	I3_OBJECT_PARAM_S1,
	I3_OBJECT_PARAM_S2,
	I3_OBJECT_PARAM_S3,
	I3_OBJECT_PARAM_S4,
	I3_OBJECT_PARAM_S5,
	I3_OBJECT_PARAM_S6,

	I3_OBJECT_PARAM_SCOUNT,
};

enum	I3_OBJECT_PARAM_THIRD
{
	I3_OBJECT_PARAM_T0 = 0,
	I3_OBJECT_PARAM_T1,
	I3_OBJECT_PARAM_T2,
	I3_OBJECT_PARAM_T3,
	I3_OBJECT_PARAM_T4,
	I3_OBJECT_PARAM_T5,
	I3_OBJECT_PARAM_T6,

	I3_OBJECT_PARAM_TCOUNT,
};


class I3_EXPORT_FRAMEWORK i3Object : public i3GameObj
{
	I3_CLASS_DEFINE( i3Object);
protected:
	I3_OBJECT_PARAM_FIRST	m_nFirstParam;
	I3_OBJECT_PARAM_SECOND	m_nSecondParam;
	I3_OBJECT_PARAM_THIRD	m_nThirdParam;

	I3_OBJECT_STATE *		m_pState;
	INT32					m_StateCount;
	INT32					m_OldState;
	INT32					m_CurState;
	INT32					m_nInitialState;

	REAL32					m_MaxHP;
	REAL32					m_HP;

	INT32					m_NetIndex;
	VEC3D					m_vCenterPos;	// Shape's center position

	i3MaterialAttr *		m_pMaterialAttr;

	i3GameResObject *		m_pObjRes;

	BOOL					m_bUpdateAnimMatrix;	// 값을 계산했다면 TRUE, 계산하지 않았으면 FALSE(업데이트가 완료되면 FALSE 상태가된다)
	MATRIX					m_matAnimationMatrix;	// 애니메이션을 하는 경우 다음 프레임의 애니메이션된 매트릭스를 저장

	//VEC3D					m_vJumpPos;

protected:
	void				_SetCurrentSg( bool bInit);
	INT32					_findSg( i3SceneGraphInfo * pSg);

public:
	i3Object(void);
	virtual ~i3Object(void);

	void					setFirstParam( I3_OBJECT_PARAM_FIRST nFirst)	{	m_nFirstParam = nFirst;}
	I3_OBJECT_PARAM_FIRST	getFirstParam( void)							{	return m_nFirstParam;}

	void					setSecondParam( I3_OBJECT_PARAM_SECOND nSecond)	{	m_nSecondParam = nSecond;}
	I3_OBJECT_PARAM_SECOND	getSecondParam( void)							{	return m_nSecondParam;}

	void					setThirdParam( I3_OBJECT_PARAM_THIRD nThird)	{	m_nThirdParam = nThird;}
	I3_OBJECT_PARAM_THIRD	getThirdParam( void)							{	return m_nThirdParam;}

	void				SetStateCount( INT32 cnt);
	INT32				getStateCount(void)					{ return m_StateCount; }

	INT32				getInitialState( void)				{ return m_nInitialState;}
	void				SetInitialState( INT32 nState, bool bInit = true);

	INT32				getOldState(void)					{ return m_OldState; }
	INT32				getCurState(void)					{ return m_CurState; }
	void				setCurState( INT32 state)			{ I3ASSERT( state > -1 && state < m_StateCount); m_CurState = state; }
	I3_OBJECT_STATE *	getState( INT32 idx)				{ return m_pState + idx; }
	void				RemoveAllState(void);
	
	REAL32				getMaxHP(void)						{ return m_MaxHP; }
	void				setMaxHP( REAL32 hp)				{ m_MaxHP = m_HP = hp; }

	REAL32				getCurrentHP( void )				{ return m_HP; }
	void				setCurrentHP( REAL32 rHp )			{ m_HP = rHp; }

	REAL32				getHP( INT32 idx)					{ return m_pState[idx].m_HPRatio; }
	void				setHP( INT32 idx, REAL32 hp)		{ m_pState[idx].m_HPRatio = hp; }
	virtual void		Damage( REAL32 dmg, i3GameObj * pEventSender, i3PhysixShapeSet * pHitShapeSet);
	// 데미지 계산 및 신 타이머를 조정합니다. 이팩트의 타이머 등을 세팅 by dfly
	void				SetDamageNoEffect( REAL32 dmg, REAL32 tm = 1000.f);

	i3TimeEventGen *	getTimeEvent( INT32 idx)			{ return m_pState[idx].m_pTimeEvent; }
	void				setTimeEvent( INT32 idx, i3TimeEventGen * pTimeEvent)
	{
		I3_REF_CHANGE( (m_pState[idx].m_pTimeEvent), pTimeEvent);

		if( pTimeEvent != NULL)
		{
			pTimeEvent->setEventReceiver( this);
		}
	}

	i3SceneGraphInfo *	getSceneGraphInfo( INT32 idx)		{ return m_pState[idx].m_pSg; }
	i3GameResSceneGraph * getRes( INT32 idx)				{ return m_pState[idx].m_pRes; }
	void				setSceneGraphInfo( INT32 idx, i3SceneGraphInfo * pSg, i3GameResSceneGraph * pRes);

	i3MaterialAttr *	getMaterialAttr( void )				{ return m_pMaterialAttr; }
	void				setMaterialAttr( i3MaterialAttr * pAttr )	{ m_pMaterialAttr = pAttr; }

	INT32				getNetIndex( void )					{ return m_NetIndex; }
	void				setNetIndex( INT32 idx )			{ m_NetIndex = idx; }

	VEC3D *				getShapeCenterPos( void )			{ return &m_vCenterPos; }
	void				setShapeCenterPos( VEC3D * pPos )	{ i3Vector::Copy( &m_vCenterPos, pPos ); }

	//VEC3D *				getJumpPos( void)					{ return &m_vJumpPos; }
	//void				setJumpPos( VEC3D * pVec)			{ i3Vector::Copy( &m_vJumpPos, pVec); }
	//void				setJumpPos( REAL32 x, REAL32 y, REAL32 z)	{ i3Vector::Set( &m_vJumpPos, x, y, z); }

	void				Reset(void);
	
	virtual void		OnUpdate( REAL32 tm);
	virtual BOOL		OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);
	virtual void		OnChangeTime( REAL32 tm);

	virtual void		SetVisible( BOOL bTrue = TRUE, REAL32 tm = 0.0f);

	virtual void		Create( i3Framework * pFramework, i3GameResObject * pRes);
	virtual void		OnInstancing( void * pUser = NULL);
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);

	/*
	// 업데이트시에 다음 프레임의 Animation된 Matrix가 필요한 경우 호출
	// CacheMatrix는 이전 프레임의 Matrix입니다. 실제 업데이트 타임에서는 값이 다릅니다.
	MATRIX *				GetAnimatedMatrix( REAL32 rDeltatime);
	// 업데이트가 완료(업데이트의 가장 맨 뒤에 호출)되면 리셋해준다.
	void					ResetAnimatedMatrix( void);
	*/
};

#endif

