#if !defined( __I3_OBJECT_H)
#define __I3_OBJECT_H

#include "i3GameObj.h"
#include "i3GameResSceneGraph.h"

class i3GameResObject;




// 타입이 길어서 typedef 로 치환..
// 아래 구조체는 생성/소멸자가 필요..



struct I3_OBJECT_STATE
{
	REAL32					m_HPRatio = 0.0f;
	i3TimeEventGen *		m_pTimeEvent = nullptr;
	i3SceneGraphInfo *		m_pSg = nullptr;
	i3GameResSceneGraph *	m_pRes = nullptr;

	// Context
	BoneListPtr				m_pBoneList;
	i3Body *				m_pBody = nullptr;

};

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
	I3_EXPORT_CLASS_DEFINE( i3Object, i3GameObj);
protected:
	I3_OBJECT_PARAM_FIRST	m_nFirstParam = I3_OBJECT_PARAM_F0;
	I3_OBJECT_PARAM_SECOND	m_nSecondParam = I3_OBJECT_PARAM_S0;
	I3_OBJECT_PARAM_THIRD	m_nThirdParam = I3_OBJECT_PARAM_T0;

	I3_OBJECT_STATE *		m_pState = nullptr;
	INT32					m_StateCount = 0;
	INT32					m_OldState = -1;
	INT32					m_CurState = -1;
	INT32					m_nInitialState = 0;

	REAL32					m_MaxHP = 100.0f;
	REAL32					m_HP = 100.0f;

	INT32					m_NetIndex = -1;
	VEC3D					m_vCenterPos = { 0.0f, 0.0f, 0.0f };	// Shape's center position

	i3MaterialAttr *		m_pMaterialAttr = nullptr;

	i3GameResObject *		m_pObjRes = nullptr;

	bool					m_bUpdateAnimMatrix = false;	// 값을 계산했다면 true, 계산하지 않았으면 false(업데이트가 완료되면 false 상태가된다)
	MATRIX					m_matAnimationMatrix;	// 애니메이션을 하는 경우 다음 프레임의 애니메이션된 매트릭스를 저장

	bool					m_bChangedSg = false;

protected:
	void					_SetCurrentSg( bool bInit);
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

	INT32				GetStateByHP(REAL32 rHP);
	
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

		if( pTimeEvent != nullptr)
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

	bool				isChangedSg( void)					{ return m_bChangedSg; }
	void				resetChangedSgFlag( void)			{ m_bChangedSg = false; }

	virtual void		OnUpdate( REAL32 tm) override;
	virtual bool		OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;
	virtual void		OnChangeTime( REAL32 tm) override;

	virtual void		SetVisible( bool bTrue = true, REAL32 tm = 0.0f);

	virtual void		Create( i3Framework * pFramework, i3GameResObject * pRes);

	virtual void		OnInstancing( I3_ONINSTANCING_INFO * pInfo) override;
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void		OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;

	/*
	// 업데이트시에 다음 프레임의 Animation된 Matrix가 필요한 경우 호출
	// CacheMatrix는 이전 프레임의 Matrix입니다. 실제 업데이트 타임에서는 값이 다릅니다.
	MATRIX *				GetAnimatedMatrix( REAL32 rDeltatime);
	// 업데이트가 완료(업데이트의 가장 맨 뒤에 호출)되면 리셋해준다.
	void					ResetAnimatedMatrix( void);
	*/
};

#endif

