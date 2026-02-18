#if !defined( __GAME_WEAPON_MANAGER_H__)
#define __GAME_WEAPON_MANAGER_H__

#include "Weapon.h"
#include "WeaponThrowKnife.h"
#include "WeaponItemID.h"

class CWeaponGlobal;
class CGameCharaBase;
class BGLoadThread;

class CHUD_Scope;

class WeaponDecoySndInterval
{
	friend class CGameWeaponManager;
public:
	WeaponDecoySndInterval(); 
	~WeaponDecoySndInterval();

	REAL32 GetInterval(INT32 index) const
	{ return m_interval[index]; }

	INT32 GetNum() const { return m_num; }
	bool GetLoop() const { return m_loop; }

private:
	REAL32*			m_interval;
	INT32			m_num;
	bool			m_loop;
};



class CDominationSkillSystem;

class CGameWeaponManager : public i3ElementBase
{
	I3_CLASS_DEFINE( CGameWeaponManager, i3ElementBase);
	
	friend class CDominationSkillSystem;
protected:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Common Information 
	i3Stage			*		m_pStage;			//Stage Pointer 

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Weapon Resource List 
	INT32					m_nWeaponIDCount;							//
	INT32					m_nWeaponCount[ WEAPON_CLASS_COUNT];		//	실제 클래스별 무기 갯수
	i3::vector<i3GameResChara*>	m_WeaponResList[ WEAPON_CLASS_COUNT];		//	무기 리소스 (i3GameResChara *)

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Weapon Class Pointer List 
	i3::vector<WeaponBase*>	m_CharaWeaponList;				// 캐릭터가 들고 있는 무기 리스트
	i3::vector<WeaponBase*>	m_DropWeaponList;				// 월드에 떨어진 무기 리스트

	i3::vector<WeaponGrenade*>*		m_pThrowWeaponList;				// 동작하는 수류탄 (2중포인터->단일로 바꾼다. 2012.05.31.수빈)
	i3::vector<WeaponGrenade*>*		m_pThrowItemList;				// 동작하는 견제류 아이템 (2중포인터->단일로 바꾼다. 2012.05.31.수빈)
	i3::vector<WeaponGrenadeShell*>*		m_GrenadeShellWeaponList;		// 유탄 (2중포인터->단일로 바꾼다. 2012.05.31.수빈)
	i3::vector<WeaponRocketLauncherShell*> m_RocketLauncherShellWeaponList;// 로켓런쳐(RPG-7)

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	Cheat (DEBUG)
#ifdef ENABLE_TEST_COMMAND	
	bool					m_bUnlimitGun;						//	총기 무한
	bool					m_bUnlimitGrenade;					//	투척류 무한
	INT32					m_CmdGunBullet;						//	총알 설정
#endif
	
protected:	
	void				_LoadRes( WEAPON_CLASS_TYPE classType, INT32 number);
	void				_ProcessDropedWeapon( REAL32 rDeltaTime);	
	

	void				_RemoveAllCharaWeapon( void);

	void				_RemoveDropWeapon( WeaponBase * pWeapon )	
	{ 
		i3::vu::remove(m_DropWeaponList, pWeapon ); 
		I3_SAFE_RELEASE(pWeapon);
	}
	bool				_ReleaseDropWeapon( WeaponBase * pWeapon);	
	INT32				_SearchCharaWeapon( WeaponBase * pWeapon);			//
	INT32				_SearchDropWeapon( WeaponBase * pWeapon);			//
	bool				_AddCharaWeapon( WeaponBase * pWeapon);				//	CharaList에 추가	

	void				_ResetNetworkInfo( WeaponBase * pWeapon);

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//Throw Weapon
private:
	void			_CreateThrow( i3Stage * pStage, WEAPON_CLASS_TYPE type, UINT32 maxCount);
	void			DestroyThrowWeapon(i3Stage * pStage);

public:
	void			CreateThrowWeapon( i3Stage * pStage);
	WeaponBase *	GetFreeThrowWeapon( WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number); 
	void			PushThrowWeapon(WeaponBase	* pWeapon); 
	INT32			getThrowWeaponCount( WEAPON::RES_ID type)
	{ 
		if( m_pThrowWeaponList == nullptr)	return 0;
		I3ASSERT( type < WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE));
		return (INT32)m_pThrowWeaponList[type].size(); 
	}
	WeaponGrenade *	getThrowWeapon( WEAPON::RES_ID type, INT32 idx)
	{ 
		if( m_pThrowWeaponList == nullptr) return nullptr; 
		I3ASSERT( type < WEAPON::getItemCount(WEAPON_CLASS_THROWING_GRENADE));
		if( m_pThrowWeaponList[type].size() > 0)
			return m_pThrowWeaponList[type][idx]; 
		return nullptr;
	}

	WeaponGrenade *	FindThrowWeaponByNetIndex( WEAPON::RES_ID type, INT32 netIdx);

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//GrenadeShell 
	void					CreateGrenadeShellWeapon( i3Stage * pStage);
	void					DestroyGrenadeShellWeapon( i3Stage * pStage);
	void					PushGrenadeShellWeapon( WeaponGrenadeShell * pWeapon);

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//Rocket Launcher Shell (RPG-7)
	void						CreateRocketLauncherShellWeapon( i3Stage * pStage);
	void						DestroyRocketLauncherWeapon( i3Stage * pStage);
	void						PushRocketLauncherWeapon( WeaponRocketLauncherShell * pWeapon);
	const i3::vector<WeaponRocketLauncherShell*>& getRPG7ShellList(void) const { return m_RocketLauncherShellWeaponList;		}
    
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// ThrowKnife
private:
	typedef i3::unordered_map<WEAPON::RES_ID, i3::vector<WeaponThrowKnife*>> tThrowKnifeCon;
	tThrowKnifeCon		m_ThrowKnifeList;

	i3::vector<WeaponThrowKnife*> * _GetThrowKnifeList( WEAPON::RES_ID num);
	void				_CreateThrowKnifeInstance( i3Stage * pStage, WEAPON::RES_ID weaponNumber);
	void				_CreateThrowKnife( i3Stage * pStage, CWeaponInfo * pWeaponInfo, bool bMode = false);
	void				_CreateThrowKnife(  i3Stage * pStage, WEAPON_CLASS_TYPE mainClassType);
	void				DestroyThrowKnife( i3Stage * pStage);

public:
	void				CreateAllThrowKnife( i3Stage * pStage);
	void				PushThrowKnife( WeaponBase * pWeapon);

	const WeaponDecoySndInterval * GetDecoySndInterval(WEAPON_CLASS_TYPE weaponCT=WEAPON_CLASS_ASSAULT);
	const WeaponDecoySndInterval * GetEventDecoySndInverval(void);

	// 내구도 관련
private:
	static	REAL32	s_BrokenWeaponDecreaseDamage;		//	무기 데미지
	static	REAL32	s_BrokenWeaponVertRecoil;			//	수직 반동
	static	REAL32	s_BrokenWeaponHorzRecoil;			//	수평 반동

	WeaponDecoySndInterval *	m_decoySndInterval;
	WeaponDecoySndInterval *	m_EventDecoySndInterval; 

#if defined( I3_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
	i3::vector<bool> 		m_WeaponResCheckList[ WEAPON_CLASS_COUNT];
#endif

public:
	template<typename T>
	static	inline	T	WeaponEnduranceZeroDamage(T Value)
	{
		return static_cast<T>(static_cast<REAL32>(Value) / s_BrokenWeaponDecreaseDamage);
	}

	template<typename T>
	static	inline	T	WeaponEnduranceZeroVertRecoil(T Value)
	{
		return static_cast<T>(static_cast<REAL32>(Value) * s_BrokenWeaponVertRecoil);
	}

	template<typename T>
	static	inline	T	WeaponEnduranceZeroHorzRecoil(T Value)
	{
		return static_cast<T>(static_cast<REAL32>(Value) * s_BrokenWeaponHorzRecoil);
	}
	static	void	InitValue();

public:
	CGameWeaponManager( void);
	virtual ~CGameWeaponManager( void);

	void	Create( i3Stage * pWorld = nullptr);
	void	Destroy( void );

	void	OnUpdate( REAL32 rDeltaTime );

	void	EnterStage( i3Stage * pStage );
	void	LeaveStage( bool bDestroy = false);


	///////////////////////////////////////////////////////////////////////////////////////////////////
	void				RemoveAllDropWeapon( void);	
	UINT32				GetCharaWeaponListCount( void)				{ return m_CharaWeaponList.size(); }
	WeaponBase *		GetCharaWeapon( UINT32 i)					{ I3ASSERT( i < m_CharaWeaponList.size()); return m_CharaWeaponList.at(i); }
	bool				ReleaseCharaWeapon( WeaponBase * pWeapon);		
	bool				MoveWeaponCharaToDrop( WeaponBase * pWeapon);											//	CharaList에서 DropList로 무기 인스턴스 이동
	bool				MoveWeaponDropToChara( WeaponBase * pWeapon, bool bOnlyList, UINT8 nSyncNumber, bool bCheckDropList, bool bPushWeaponList);	//	DropList에서 CharaList로 무기 인스턴스 이동	

	/** \brief 무기 생성
		\param[in] pOwner 사용 유저
		\param[in] Itemid 무기 ID
		\param[in] subItem 보조로 생성되는 ID
		\param[in] MustCreate 반드시 생성 여부 (false 이면 Instance 리스트에서 찾는다)
		\param[in] bCreateInWeapon 무기가 생성하는 무기(듀얼 계열) */
	WeaponBase *		QueryWeapon( CGameCharaBase * pOwner, T_ItemID Itemid, T_ItemID subItem = 0, bool MustCreate = false, bool bCreateInWeapon = false );
	WeaponBase *		_FindFreeWeapon( T_ItemID Itemid);
	void				_ClearGarbageCharaWeaponList( void);
	WeaponBase *		_CreateNewWeapon( T_ItemID Itemid, T_ItemID subItem, CGameCharaBase * pOwner, bool bCreateInWeapon );
	WeaponBase *		_CreateDinoWeapon(T_ItemID ItemID, T_ItemID subItem, CGameCharaBase * pOwner );

	void				RemoveWeaponRes( bool bRemoveCommonRes);
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	bool				IsExistResource( WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number);
#endif
	i3GameResChara *	QueryResource( T_ItemID ItemID, bool bDirectLoad = true);
	i3GameResChara *	QueryResource( WEAPON_CLASS_TYPE classType, INT32 number, bool bDirectLoad = true);
	WeaponBase *		FindDropWeapon( CGameCharaBase * pOwner, WEAPON_CLASS_TYPE type, INT32 nNumber);
	WeaponBase	*		FindDropWeaponByIdx( INT32 nIdx );					//
	INT32				GetDropListCount( void)			{	return (INT32)m_DropWeaponList.size();				}
	WeaponBase *		GetDropWeapon( INT32 idx )		{ return m_DropWeaponList[idx]; }
	WeaponBase *		SetDropWeapon( INT32 idx, WEAPON_SLOT_TYPE nWeaponSlot, INT16 nDropIdx, bool bNextWeapon = true, bool bDirectPickup = false);
	void				PushDropWeaponToChara( WeaponBase * pWeapon );					//C4에서만 사용합니다. 이름이 부적절
	void				MoveShapePosition( WeaponBase * pWeapon, VEC3D * pPos );

	void				PreloadWeaponResource( WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number);
	bool				IsRegisteredWeapon(WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number);

	// 라운드 종료이면 월드에 있는 무기들을 안보이게 해야합니다.
	void				StageRoundEnd( void);

	void				PreStartRound();

	//	Cheat (DEBUG)
#ifdef ENABLE_TEST_COMMAND	
	void				COMMAND_SetUnlimitGun( bool bVal)		{ m_bUnlimitGun = bVal;						}
	void				COMMAND_SetUnlimitGrenade( bool bVal)	{ m_bUnlimitGrenade = bVal;					}
	bool				COMMAND_IsUnlimitGun( void)				{ return m_bUnlimitGun;						}
	bool				COMMAND_IsUnlimitGrenade( void)			{ return m_bUnlimitGrenade;					}
	
	void				COMMAND_SetGunBullet( INT32 Bullet)		{ m_CmdGunBullet = Bullet;					}
	INT32				COMMAND_GetGunBullet( void)				{ return m_CmdGunBullet;						}
#endif
	
	//======================================================================================//
	//										Backgound load									//
	//======================================================================================//
private:
	enum eWEAPONLOAD_STATE {
		eWEAPONLOAD_STATE_NONE,
		eWEAPONLOAD_STATE_PUSH,		// 예약 상태
		eWEAPONLOAD_STATE_LOADING,
	};

	struct sWEAPONLOADINFO {
		T_ItemID				_ItemID;
		eWEAPONLOAD_STATE	_state;
		BGLoadThread *		_pThread;
	};

	i3FixedMemoryPool *		m_pLoadInfoPool;
	i3::deque<sWEAPONLOADINFO*>		m_BgLoadQueue;				// 차순 로드되는 무기들
	i3::deque<sWEAPONLOADINFO*>		m_BgPriorityLoadQueue;		// 우선 로드되는 무기들

	REAL32		m_rTrashTimer;

	void		__RegisterWeaponResource( WEAPON_CLASS_TYPE classType, INT32 num, i3GameResChara * pRes);
	void		__RemoveWeaponBGLoadInfo(void);
	
	bool		__GetWeaponFileName( char * pszOut, CWeaponInfo * pWeaponInfo);
	const char * __GetWeaponResourcePath( CWeaponInfo * pWeaponInfo);
	const char * __GetWeaponResourcePath( WEAPON_CLASS_TYPE classType, INT32 num);
	const char * __GetWeaponResourcePath( T_ItemID ItemID);

	/** \brief 무기 로드
		\return true : 로딩 완료, false : 로딩중 */
	bool		__LoadWeapon( sWEAPONLOADINFO * pInfo);
	void		__BackgroundLoading( void);

	/** \brief ItemID로 LoadQue에 있는지 확인한다. */
	bool		_IsLoadPushed( T_ItemID itemID, bool bPriority);
	void		_CollectBackgroundClassIDList(i3::vector<size_t>(& inout)[WEAPON_CLASS_COUNT]);
public:
	void		PushLoadWeapon( T_ItemID itemID, bool bPriority = false);

	///** \brief 자신의 인벤토리에 있는 무기는 미리 로딩하자 */
	void		PreloadMyWeapons( void);

	void		TrashWeaponRes( REAL32 tm);
	
	void		CreateAllSpecialDamageFilter();
	void		CreateAllSpecialDieFilter();
	void		CreateAllScopeHudTexture();

	inline const bool isBGLoading(void)				{ return (!m_BgLoadQueue.empty() || !m_BgPriorityLoadQueue.empty()); }
};

#endif

