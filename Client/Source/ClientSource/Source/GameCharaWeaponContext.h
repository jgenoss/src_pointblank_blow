/**
 * \file	GameCharaWeaponContext.h
 *
 * Declares the game chara weapon context class.
 */


#if !defined( __GAME_CHARA_WEAPON_CONTEXT_H__)
#define __GAME_CHARA_WEAPON_CONTEXT_H__

#include "GameCharaContextBase.h"
#include "GameCharaWeaponControlDef.h"


class CGameCharaWeaponControl;
class CGameCharaWeaponControl1PV;
class CGameCharaWeaponControl3PV;

class CGameCharaWeaponContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaWeaponContext, CGameCharaContextBase);

	friend class CGameCharaBase;
	friend class CGameCharaMaterialContext;

	friend class CGameCharaWeaponControl;
	friend class CGameCharaWeaponControl1PV;
	

protected:
	// WeaponController
	VAR_LR(CGameCharaWeaponControl*,66)	m_pCurrentWeaponControl = nullptr;
	VAR_LR(CGameCharaWeaponControl1PV*,67)	m_p1PVWeaponControl = nullptr;
	VAR_LR(CGameCharaWeaponControl3PV*,68)	m_p3PVWeaponControl = nullptr;

	VAR_LR(WeaponBase*, 69)	m_pWeapon[WEAPON_SLOT_COUNT] = { nullptr, };
	VAR_LR(WeaponBase*,70)	m_pStaticCurrentWeapon = nullptr;

	VAR_LR(WEAPON_SLOT_TYPE,71) m_nCurWeaponSlotNum = WEAPON_SLOT_PRIMARY;
	VAR_LR(WEAPON_SLOT_TYPE,72) m_nOldWeaponSlotNum = WEAPON_SLOT_UNKNOWN;
	VAR_LR(WEAPON_SLOT_TYPE,73) m_nSwapWeaponSlotNum = WEAPON_SLOT_PRIMARY;

	VAR_LR(WeaponBase*,74) m_pPurchasePrimWeapon = nullptr;		// 현재 등에 매고 있는 무기

	i3Node *			m_pDummyRightHand = nullptr;			//	오른손 붙는 더미
	i3Node *			m_pDummyLeftSideHand = nullptr;		//  오른손 그립의 보조 더미(왼손)
	i3Node *			m_pDummyLeftHand = nullptr;			//	왼손 붙는 더미
	i3Node *			m_pDummyRightSideHand = nullptr;		// 왼손 그립의 보조 더미(오른손)
	i3Node *			m_pDummyPrimPurchase = nullptr;		// 주무기 붙는 더미
	i3Node *			m_pWeaponAttachRoot = nullptr;		// 실제 붙어 있는 더미
	CHARA_WEAPON_ATTACH_HAND	m_nWeaponAttachHand = CHARA_WEAPON_GRIP_RIGHT_HAND;

	REAL32				m_rDropedWeaponCheckTime = 0.0f;	// 드랍 웨폰에 대한 체크 타임
	typedef std::deque<INT32> deqThrowWeaponIdxCon;
	deqThrowWeaponIdxCon m_deqThrowList;

	bool				m_bForcedChange = false;			// 다중무기끼리 빠른 무기교체할 경우 강제로 변경
	UINT8				m_nPrevMultiWeaponIdx = 0;			// 다중무기 교체 시 렌더링에서 제거하기 위하여 이전 장착한 다중무기 인덱스를 보관

public:
	
	// 해당 슬롯의 무기
	WeaponBase *		getWeapon( WEAPON_SLOT_TYPE nSlotNum);
	void				setWeapon( WEAPON_SLOT_TYPE nSlotNum, WeaponBase * pWeapon)
	{
		I3_BOUNDCHK( nSlotNum, WEAPON_SLOT_COUNT);
		I3_REF_CHANGE( m_pWeapon[ nSlotNum], pWeapon);
	}

	// 현재 들고 있는 무기
	WeaponBase *		getCurrent(void)						{ I3_BOUNDCHK( m_nCurWeaponSlotNum, WEAPON_SLOT_COUNT);	return getWeapon( m_nCurWeaponSlotNum);	}
	// 현재 들고 있는 무기 슬롯
	WEAPON_SLOT_TYPE	getCurrentSlotNum( void )					{ return m_nCurWeaponSlotNum;								}
	void				setCurrentSlotNum( WEAPON_SLOT_TYPE nSlot)	{ m_nCurWeaponSlotNum = nSlot; }
	// 이전 무기 슬롯
	WEAPON_SLOT_TYPE	getOldSlotNum( void )					{ return m_nOldWeaponSlotNum;								}
	void				setOldSlotNum( WEAPON_SLOT_TYPE nSlot)	{ m_nOldWeaponSlotNum = nSlot; }
	// 다음 프레임에 바뀔 무기 슬롯
	WEAPON_SLOT_TYPE	getSwapSlotNum( void )					{ return m_nSwapWeaponSlotNum;								}
	void				setSwapSlotNum( WEAPON_SLOT_TYPE nSlot)	{ m_nSwapWeaponSlotNum = nSlot; }

	// 등에 매고 있는 무기
	WeaponBase*			getBackpack( void )						{ return m_pPurchasePrimWeapon; }
	void				setBackpack( WeaponBase * pWeapon)		{ m_pPurchasePrimWeapon = pWeapon; }
	
	// 등에 붙일 무기의 위치 노드
	i3Node *			getDummyPrimPurchase( void)				{ return m_pDummyPrimPurchase; }

	WeaponBase *		getStaticCurrentWeapon(void)			{ return m_pStaticCurrentWeapon;}
	void				setStaticCurrentWeapon(WeaponBase * pW)	{ m_pStaticCurrentWeapon = pW;}

	void				setForcedChange( bool bForced)			{ m_bForcedChange = bForced; }
	bool				getForcedChange(void)					{ return m_bForcedChange; }

	void				setPrevMultiWeaponIdx(UINT8 idx)		{ m_nPrevMultiWeaponIdx = idx; }
	UINT8				getPrevMultiWeaponIdx()					{ return m_nPrevMultiWeaponIdx; }

	// 권총집의 무기
	/*WeaponBase*			getSidepack( void )						{ return m_pPurchaseSideWeapon; }
	void				setSidepack( WeaponBase * pWeapon)		{ m_pPurchaseSideWeapon = pWeapon; }*/
	
	// 권총집의 위치 노드
	//i3Node *			getDummySidePurchase( void)				{ return m_pDummySidePurchase; }

	i3Node *			getDummyLeftHand( void)					{ return m_pDummyLeftHand; }
	i3Node *			getDummyRightHand( void)				{ return m_pDummyRightHand; }
	// 현재 붙어 있는 무기의 노드
	i3Node *			getAttachRoot( void)					{ return m_pWeaponAttachRoot;			}
	void				setAttachHand( CHARA_WEAPON_ATTACH_HAND nAttachHand)
						{	
							switch( nAttachHand)
							{
							case CHARA_WEAPON_GRIP_RIGHT_HAND:	
								{
#ifdef ENABLE_ATTACH_HAND_OUTPUT
								I3TRACE("setAttachHand : CHARA_WEAPON_GRIP_RIGHT_HAND\n");
#endif
									m_pWeaponAttachRoot = m_pDummyRightHand;		
								}break;
							case CHARA_WEAPON_BARREL_LEFT_HAND:
								{
#ifdef ENABLE_ATTACH_HAND_OUTPUT
									I3TRACE("setAttachHand : CHARA_WEAPON_BARREL_LEFT_HAND\n");
#endif
							#if defined( USE_RELOAD_NEWANIMATION)
									m_pWeaponAttachRoot = m_pDummyLeftHand;
							#else
									m_pWeaponAttachRoot = m_pDummyLeftSideHand;
							#endif
								}break;
							case CHARA_WEAPON_GRIP_LEFT_HAND :	
								{
#ifdef ENABLE_ATTACH_HAND_OUTPUT
									I3TRACE("setAttachHand : CHARA_WEAPON_GRIP_LEFT_HAND\n");
#endif
									m_pWeaponAttachRoot = m_pDummyLeftHand;			
								}break;
							case CHARA_WEAPON_BARREL_RIGHT_HAND :	
								{
#ifdef ENABLE_ATTACH_HAND_OUTPUT
									I3TRACE("setAttachHand : CHARA_WEAPON_BARREL_RIGHT_HAND\n");
#endif
									m_pWeaponAttachRoot = m_pDummyRightSideHand;	
								}break;
							default: 
								{
#ifdef ENABLE_ATTACH_HAND_OUTPUT
									I3TRACE("setAttachHand : CHARA_WEAPON_BARREL_RIGHT_HAND\n");
#endif
									m_pWeaponAttachRoot = nullptr;		
								}break;
							}
							m_nWeaponAttachHand = nAttachHand;
						}
	CHARA_WEAPON_ATTACH_HAND getAttachHand( void)						{ return m_nWeaponAttachHand;			}

	// 드랍된 무기 체크 타임
	REAL32				getDropedWeaponCheckTime( void)					{ return m_rDropedWeaponCheckTime; }
	void				setDropedWeaponCheckTime( REAL32 tm)			{ m_rDropedWeaponCheckTime = tm; }

	WEAPON_SLOT_TYPE	getNextUsableWeaponSlot(void);
	WEAPON_SLOT_TYPE	getPrevUsableWeaponSlot(void);
	bool				checkUsableWeaponSlot( WEAPON_SLOT_TYPE slot, bool bCheckMultiWeapon);

	void				NoUseWeapon( WeaponBase* pWeapon);

public:
	CGameCharaWeaponContext( void);
	virtual ~CGameCharaWeaponContext( void);

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	// change view
	virtual void	OnChange1PV() override;
	virtual void	OnChange3PV() override;

	virtual void	OnChangeLocal() override;
	virtual void	OnChangeNetwork() override;
	virtual void	OnChangeNPC() override;

	/** \brief Attach Node를 찾는다. */
	virtual void	OnChangeBoneHierarchy( i3Body * pBody) override;
	
	virtual void	OnUpdate( REAL32 rDeltatime) override;

	/** \brief 무기의 무결성 검사를 합니다.
		\note 현재 캐릭터에 해당 무기가 없는 경우 생성해서 들고 있도록 한다. */
	void			WeaponValidCheck( WEAPON_CLASS_TYPE nClassType, WEAPON::RES_ID WeaponNum, WEAPON_SLOT_TYPE slot, UINT8 nMultiWeaponIndex, bool bSyncChange);
	WeaponBase *	FindWeaponByExtensionItemID( T_ItemID extItemID);
	
public:
	// **\brief 게임 모드에 따라 무기를 설정한다. */
	WEAPON_SLOT_TYPE	CreateWeaponByGameMode( UINT8 WeaponFlag);

	INT32			ChangeToEquipedWeapon( INT32 nSlot, UINT8 weapponLimitFlag);

	/** \brief 리스폰 하면 호출됩니다. */
	bool	_Respawn( void);
	void	PushThrowWeaponIndex(INT32 nIdx);
	INT32	GetThrowWeaponIndex();
	void	PopThrowWeaponIndex();

public:
	/** \brief 무기의 색상을 설정한다.
		\param[in] pDiff Diffuse color
		\param[in] pSpec Specular color
		\param[in] alpha Alpha value */
	bool	_Change_Color( COLORREAL * pDiff = nullptr, COLORREAL * pSpec = nullptr, REAL32 alpha = 1.f);

	/** \brief 무기를 변경한다.
		\param[in] slot 무기 슬롯 번호
		\param[in] bDirect 현재 프레임에서 변경 여부
		\note bDirect가 false라면 next frame에서 무기 교체가 이루어집니다.
			다만 캐릭터는 이미 다음 무기의 애니메이션으로 현재 frame에서 설정됩니다. */
	bool	_ChangeWeapon( WEAPON_SLOT_TYPE slot, bool bDirect = false);

	/** \brief 무기를 생성한다.
		\param[in] slot 무기 슬롯 번호
		\param[in] T_ItemID 무기 ItemID */
	bool	_Create_Weapon( WEAPON_SLOT_TYPE slot, T_ItemID Itemid);

	/** \brief 해당 슬롯의 무기를 삭제한다.
		\param[in] slot 무기 슬롯 번호 */
	void	_Delete_Weapon( WEAPON_SLOT_TYPE slot);

	/** \brief 무기를 띄어낸다.
		\param[in] slot 무기 슬롯		*/
	void	_Detach_Weapon( WEAPON_SLOT_TYPE slot);

	/** \brief 무기를 띄어낸다.
		\param[in] pWeapon 무기 pointer */
	void	_Detach_Weapon( WeaponBase * pWeapon);

	/** \brief 무기 버기리
		\param[in] slot 무기 슬롯 번호
		\param[in] pPos 위치
		\param[in] bNextWeapon 다음 무기로 자동 변경 유무 */
	bool	_Drop( WEAPON_SLOT_TYPE slot, VEC3D * pPos, bool bNextWeapon, bool bDirectPickup);

	/** \brief Extension 활성 설정 */
	bool	_Extension( bool bEnable);

	/** \brief 무기 발사
		\param[in] order 공격 형태
		\param[in] pPos 캐릭터 현재 위치
		\param[in] pTarget 목표점 */
	bool	_Fire( WEAPON::FIRE_ORDER order, VEC3D * pPos = nullptr, VEC3D * pTarget = nullptr);

	/** \brief 현재 무기를 사용가능 상태로 변경한다. */
	void	_Idle( void);

	/** \brief 탄약 장전 */
	bool	_Load_Bullet( void);

	/** \brief 탄창 장전 */
	bool	_Load_Magazine( void);

	/** \brief Quick load */
	bool	_Load_Magazine_Dual( void);

	/** \brief 장전 */
	bool	_Load_Magazine_Ready( void);

	/** \brief 무기를 줍는다.
		\param[in] pWeapon weapon pointer */
	bool	_Pickup( WeaponBase * pWeapon, UINT8 nSyncNumber, bool bCheckDropList, bool bPushWeaponList);

	/** \brief 약실에서 탄약을 뺀다
		\note 유탄만 해당됩니다. */
	bool	_Remove_Shell( void);

	/** \brief 모든 무기를 초기화한다. */
	void	_Reset_Weapons( void);

	/** \brief 현재 무기를 설정한다.
		\note 다음 교체되는 무기가 설정되면 바꿔줍니다. */
	void	_SyncWeapon( void);

	/** \brief Zoom */
	bool	_Zoom( void);
};

#endif	// __GAME_CHARA_WEAPON_CONTEXT_H__
