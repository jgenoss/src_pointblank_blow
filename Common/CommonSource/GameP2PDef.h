/**
 * \file	d:\Project\PointBlank\Source_New\CommonSource\GameP2PDef.h
 *
 * Declares the game p 2 p def class.
 */


#if !defined( __GAMEP2PDEF_H__ )
#define __GAMEP2PDEF_H__

#include "NetworkP2PDef.h" 

#define MAX_SUICIDE_COUNT			2				// 최대 Suicde Count (Dedicated :수류탄은 2번 처리되야함)
#define MAX_FIREHIT_COUNT			64				// 패킷으로 최대 히트 카운트 (이값이 넘으면 패킷 처리X)
#define MAX_ACTION_COUNT			8

#define MAX_THROWWEAPON_SLOP		16				//버퍼크기는 서버수정이 필요하므로 일단 16개로 해둔다
#define MAX_THROWWEAPON_COUNT		(MAX_THROWWEAPON_SLOP * SLOT_MAX_COUNT)				// 처리가능한 수류탄의 갯수( include GrenadeShell, Rocket )
#define MAX_THROWWEAPON_BUFSIZE		(MAX_THROWWEAPON_COUNT + DINO_THROWWEAPON_COUNT)	// 제압미션 공룡투척무기(grenade idx 허용수치로 사용)
#define MAX_DROPEDWEAPON_COUNT		32				// Droped weapon count
#define OBJ_STATIC_COUNT			220				//  처리가능한 Static Obj
#define OBJ_MOVE_COUNT				4				//  moving object
#define OBJ_DYNAMIC_COUNT			8				//  Dynamic Object	사용하는 Object가 없습니다. 
#define OBJ_ANIM_COUNT				6				// animation object
#define OBJ_CONTROLED_COUNT			16				// controled Object
#define OBJ_RESERVED_COUNT			2
#define OBJ_TOTAL_COUNT				(OBJ_STATIC_COUNT+OBJ_DYNAMIC_COUNT+OBJ_ANIM_COUNT+OBJ_RESERVED_COUNT)	//

#define MAKE_SYNC_WEAPON(ext, usage, multiWeapon) (((ext)<<7) | (((usage)&0x07) << 4) | ((multiWeapon)&0x0F))

#if defined( CLIENT_SIDE)
	class i3Object; 
	class WeaponBase; 
	class CGameCharaBase;
#endif

// Character의 정보 샌드에 대한 체커용 버퍼
struct CASTCHECK_USER
{	
	bool	bCheckBroadcast;		// 보낼 데이터가 있는지 체크
	UINT32	nP2PInfoFlag;			// 센드할 데이터의 종류를 체크해줍니다.

	void	Reset( void )
	{
		bCheckBroadcast	= false;
		nP2PInfoFlag	= 0;
	}
} ;

const REAL32 PI = 3.14159265358979323846f;

namespace UDP_UTIL
{
	inline REAL32 DegreeToRadian( REAL32* rIn )
	{
		return (*rIn) * PI * 0.0055555555555555555555555555555556f;
	}

	/** \brief 2Byte의 REAL16을 REAL32값으로 변환합니다.
		\param[out] pOut REAL32
		\param[in] nIn REAL16
		\return N/A		*/
	inline 
	void ConvertNetToReal( REAL32 * pOut, REAL16 nIn )
	{
		REAL32	fRad;

		fRad = (REAL32)nIn;

		I3ASSERT( fRad > -10000.f);
		I3ASSERT( fRad < 10000.f);

		*pOut = DegreeToRadian( &fRad );

		I3ASSERT( i3Math::isValid( *pOut));
	}

	/** \brief 2Byte의 정수형 Vector를 REAL32형 Vector로 변환합니다.
		\param[out] pOut VEC3D
		\param[in] pIn N_POS structure
		\param[in] fInvScale Scale Value
		\return N/A		*/
	inline
	bool ConvertNet3DtoVec3D( VEC3D *pOut, const N_POS * pIn, LPCSTR szFile = nullptr, int nLine = 0 )
	{
		pOut->vec[0] = ((REAL32)pIn->x);
		pOut->vec[1] = ((REAL32)pIn->y);
		pOut->vec[2] = ((REAL32)pIn->z);

		if( i3Vector::isValid( pOut) == false)
		{
			if( szFile )
			{
				I3PRINTLOG(I3LOG_NOTICE,"Net3d %s %d", szFile, nLine);
			}
		}
		return i3Vector::isValid( pOut);
	}

	/** \brief 2Byte 정수형의 Quaterion을 REAL32형 Quaternion으로 변환합니다.
		\param[out] pOut QUATERNION structure
		\param[in] pIn N_ROT structure
		\param[in] fInvScale Scale Value
		\return N/A		*/
	inline
	void ConvertNetRottoQuat( QUATERNION *pOut, N_ROT *pIn, REAL32 fInvScale )
	{	
		i3Quat::Set( pOut, pIn->Theta * fInvScale, pIn->Rho * fInvScale, pIn->Fan * fInvScale, pIn->Weight * fInvScale );

		I3ASSERT( i3Quat::isValid( pOut));
	}
	
	/** \brief Character Action State Packet을 Parsing합니다.
		\param[out] pFire Fire State
		\param[out] pBody Body State
		\param[out] pBody2 Body2 State
		\param[out] pUpper Upper State
		\param[out] pLower Lower State
		\param[out] pSecFire Secondary Fire State
		\param[out] pHelmet Helmet State
		\param[in] nNetAction Network State
		\return N/A	*/
	inline
	void ConvertNetActiontoChara( UINT8 * pExtension, UINT8 * pWeaponSlot, UINT8 * pMultiWeaponSlot, UINT8 * pFire, UINT8 * pBody, UINT8 * pBody2, UINT8 * pUpper, UINT8 * pLower, UINT8 * pJumpState, UINT8 * pSecFire, UINT8 * pHelmet, UINT32 nNetAction )
	{
		if( pExtension != nullptr)
			*pExtension = (UINT8)((nNetAction >> 31) & 0x01 ); // 1bit
		if( pWeaponSlot != nullptr)
			*pWeaponSlot = (UINT8)((nNetAction >> 28) & 0x07 ); // 3bit
		if( pUpper != nullptr)
			*pUpper = (UINT8)((nNetAction >> 20) & 0xFF);	// 8bit
		if( pLower != nullptr)
			*pLower = (UINT8)((nNetAction>> 16) & 0x0F);	// 4bit
		if( pFire != nullptr)
			*pFire	= (UINT8)((nNetAction >> 15 & 0x01));	// 1bit
		if( pBody != nullptr)
			*pBody	= (UINT8)((nNetAction >> 14) & 0x01);	// 1bit
		if( pBody2 != nullptr)
			*pBody2 = (UINT8)((nNetAction >> 10) & 0x0F);	// 4bit
		if( pMultiWeaponSlot != nullptr)
			*pMultiWeaponSlot = (UINT8)((nNetAction >> 6) & 0x0F);	// 4bit
		if (pJumpState != nullptr)
			*pJumpState = (UINT8)((nNetAction >> 3) & 0x07);	// 3bit		
		if( pSecFire != nullptr)
			*pSecFire = (UINT8)((nNetAction>>2) & 0x01);	// 1bit
		if( pHelmet != nullptr)
			*pHelmet = (UINT8)((nNetAction) & 0x03);	// 2bit
	}

	/** \brief Real32값을 2Byte REAL16으로 변환합니다.
		\param[out] pOut REAL16의 2byte REAL16타입 포인터
		\param[in] fIn REAL32값 */
	inline
	void ConvertRealToNet( REAL16 * pOut, REAL32 fIn )
	{
		if( i3Math::isValid( fIn) == false)
			fIn = 0.f;

		*pOut	= (REAL16)(I3_RAD2DEG( fIn ));
	}

	/** \brief convert Chara Position x, y, z each 2byte using map scale
		\param[out] pOut N_POS structure
		\param[in] pIn VEC3D structure
		\param[in] fScale Scale value
		\return sizeof(N_POS) */
	inline
	UINT16 ConvertVec3DtoNet3D( N_POS * pOut, VEC3D * pIn )
	{
		if( i3Vector::isValid( pIn) == false )
		{
			i3Vector::Zero( pIn);
		}

		pOut->x	= ((REAL16)i3Vector::GetX( pIn ));
		pOut->y = ((REAL16)i3Vector::GetY( pIn ));
		pOut->z = ((REAL16)i3Vector::GetZ( pIn ));

		return sizeof( N_POS);
	}

	/** \brief convert Quaternion to each 2byte value
		\param[out] pOut N_ROT structure
		\param[in] pIn QUATERNION structure
		\param[in] fScale Scale value
		\return sizeof( N_ROT) */
	inline
	UINT16 ConvertQuattoNetRot( N_ROT * pOut, QUATERNION * pIn, REAL32 fScale )
	{
		REAL32 realPos;

		realPos		= i3Quat::GetA( pIn ) * fScale;
		if( i3Math::isValid( realPos) == false)
			realPos = 0;
		pOut->Theta = (INT16) realPos;
		realPos		= i3Quat::GetB( pIn ) * fScale;
		if( i3Math::isValid( realPos) == false)
			realPos = 0;
		pOut->Rho	= (INT16) realPos;
		realPos		= i3Quat::GetC( pIn ) * fScale;
		if( i3Math::isValid( realPos) == false)
			realPos = 0;
		pOut->Fan	= (INT16) realPos;
		realPos		= i3Quat::GetD( pIn ) * fScale;
		if( i3Math::isValid( realPos) == false)
			realPos = 0;
		pOut->Weight = (INT16) realPos;

		return sizeof( N_ROT);
	}

	/** \brief HeadStructure packet을 구성합니다.
		\param[out] pOutPacket
		\param[in] Protocol P2PProtocolList enum value
		\param[in] idx User Index
		\param[in] gameTime Time Counter
		\param[in] roundNum	Round Number
		\param[in] size total packet size
		\param[in] Ping Ping packet flag
		\return sizeof(N_GAME_HEADER) */
	UINT16	BuildHead( char * pOutPacket, UINT8 Protocol, UINT8 idx, REAL32 gameTime, UINT8 roundNum, UINT16 size, bool Ping, UINT8 ui8PacketUpdateIdx, UINT8 RespawnIdx = 0, UINT8 TouchDownIdx = 0 , UINT8 Extra = 0 );

	/** \brief Sub Head structure packet을 구성합니다.
		\param[out] pOutPacket
		\param[in] type P2P_SUB_HEADTYPE enum value
		\param[in] idx User(or Object, Weapon) Index
		\param[in] size sub structure packet size
		\return sizeof( N_GAME_SUBHEAD) */
	UINT16	BuildSubHead( char * pOutPacket, P2P_SUB_HEADTYPE type, UINT16 idx, UINT16 size );

	/** \brief Broadcast용
		\param[in] type Packet Sub type
		\param[in] pos Packet의 Write position
		\param[in] limitSize 한계 size, WritePosition에서 해당 패킷 타입에 따른 크기를 산출하여
			이 크기를 넘으면 false를 반환한다. */
	bool	IsPackSize( P2P_SUB_HEADTYPE type, UINT16 pos, UINT16 limitSize = 1024);

	/** \brief Broadcast용 character packet을 구성하기 전 해당 Flag의 패킷 사이즈를 검사한다.
		\param[in] flag P2P_INFO_FLAG
		\param[in] pos write position
		\param[in] limitSize 최대 packet size. pos에서 정보 packet을 더한 크기가 넘으면 false된다.
		\return bool	*/
	bool	IsCharaPackSize( P2P_INFO_FLAG flag, UINT16 pos, UINT16 limitSize = 1024);

	// 저중력 모드
	inline
	void	SetJumpHoldState(UINT32* pNetAction, CHARA_ACTION_SPACE State)
	{
		if(pNetAction != nullptr)
			*pNetAction = ((*pNetAction) | (State << 3));
	}
	//
	inline
	UINT32	GetJumpHoldState(UINT32 nNetAction)
	{
		return  ((nNetAction >> 3 & 0x07)); //3bit
	}
};

class CPackBit
{
	INT32					m_i32Size;
	INT32					m_i32BitPos;
	char*					m_pBuffer;
public:
	CPackBit( char* pBuffer )	{	m_pBuffer = pBuffer;	m_i32BitPos = 0; m_i32Size = 0;		};
	~CPackBit()	{};

	BOOL					Write( UINT8 ui8Data, INT32 i32BitSize )
	{
		//char* pBuffer = m_pBuffer + m_i32Size;
		//*pBuffer |= ui8Data << i32BitSize;
		//m_i32BitPos += i32BitSize;
		//m_i32Size	= m_i32BitPos / 8;

		INT32 i32Count = i32BitSize / 8;
		if( 0 == (i32BitSize % 8) )	i32Count++;
		char* pBuffer;
		INT32 i32AndBit;
		for( INT32 i = 0 ; i < i32Count ; i++ )
		{
			pBuffer = m_pBuffer + m_i32Size;
			*pBuffer |= (ui8Data & i32AndBit) << i32BitSize;
		}
	}
};

/////////////////////////////////////////////////////////////////////////////////////
// 실제 패킷은 아닙니다. Game과 통신용입니다.
//
class GameNetworkInfoBase
{
public:
	virtual void	Reset() = 0;
	virtual void	CopyTo( GameNetworkInfoBase * pDest ) = 0;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) = 0;
	virtual UINT16	Unpack( const char * pInPacket ) = 0;
	virtual UINT32	PackFull( char * pOutPacket, UINT32 ui32BufSize, P2P_SUB_HEADTYPE eHeadType, UINT16 ui16Idx )
	{
		//Head 크기 체크
		if( sizeof(N_GAME_SUBHEAD) > ui32BufSize )		return 0;
		// Pack
		UINT32 ui32WriteSize = Pack( &pOutPacket[ sizeof(N_GAME_SUBHEAD) ], ui32BufSize );
		if( 0 == ui32WriteSize )						return 0;

		ui32WriteSize += sizeof(N_GAME_SUBHEAD);

		N_GAME_SUBHEAD*	pSubHead = reinterpret_cast<N_GAME_SUBHEAD*>(pOutPacket);
		pSubHead->_Index	= ui16Idx;
		pSubHead->_Type		= (UINT8)eHeadType;
		pSubHead->_Code		= static_cast<UINT16>(ui32WriteSize );
		
		return ui32WriteSize;
	}
};

class GAMEINFO_CHARA_HP : public GameNetworkInfoBase
{
protected:
	UINT16	_nCharaHP = 100;

public:
	virtual void	Reset() override { _nCharaHP = 100; }
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	UINT16	getHP( void) const;
	void	setHP( UINT16 hp);
};

class GAMEINFO_CHARA_WEAPON : public GameNetworkInfoBase
{
	friend class GAMEINFO_CHARA_NETDATA;

protected:
	UINT32				_ui32WeaponID = 0;				// Weapon ItemID
	WEAPON::ATTACK_TYPE	_AttackType = WEAPON::ATTACK_TYPE_L1;
	bool				_bWeaponExtension = false;		// 무기 확장기능 상태 (1 이면 활성화 상태)
	bool				_bSwap = false;					// 무기 교체 상태(1이면 교체된 상태)

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	UINT16	Pack_Param( char * pOutPacket, UINT32 ui32BufSize, WeaponParamClass iClass);
	UINT16	Unpack_Param( const char * pInPacket);
	
	bool	getWeaponExtension( void) const			{ return _bWeaponExtension; }
	void	setWeaponExtension( bool bExtension)	{ _bWeaponExtension = bExtension; }

	bool	getWeaponSwap( void) const				{ return _bSwap; }
	void	setWeaponSwap( bool bSwap)				{ _bSwap = bSwap; }

	UINT32	getWeaponID( void) const				{ return _ui32WeaponID; }
	void	setWeaponID( UINT32 id)					{ _ui32WeaponID = id; }

	WEAPON::ATTACK_TYPE	GetAttackType( void) const	{ return _AttackType; }
	void	SetAttackType( WEAPON::ATTACK_TYPE type)	{ _AttackType = type; }

	WEAPON::FIRE_ORDER getFireOrder( void) const	{ return WEAPON::AttackType2FireOrder( _AttackType); }

	WEAPON_SLOT_TYPE	getWeaponUsage(void) const	{ return WEAPON::ItemID2Slot(_ui32WeaponID); }
	WEAPON_CLASS_TYPE	getWeaponClass(void) const	{ return (WEAPON_CLASS_TYPE)GET_ITEM_SUBTYPE(_ui32WeaponID); }
	UINT32	getWeaponNumber(void) const				{ return GET_ITEM_NUMBER(_ui32WeaponID); }
};

class GAMEINFO_CHARA_POSROT : public GameNetworkInfoBase
{
protected:
	REAL32	_fTheta = 0.0f;				//
	REAL32	_fRho = 0.0f;					//
	VEC3D   _vPos = { 0.0f, 100.0f, 0.0f };					// Chara 위치
	INT16	_SectorIdx = -1;

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	UINT16	Pack_CommandMove( char * pOutPacket);
	UINT16	Unpack_CommandMove( const char * pInPacket);

	REAL32	getTheta( void) const		{ return _fTheta; }
	void	setTheta( REAL32 theta)		{ _fTheta = theta; }

	REAL32	getRho( void) const			{ return _fRho; }
	void	setRho( REAL32 rho)			{ _fRho = rho; }

	const VEC3D *	getPos( void) const	{ return &_vPos; }
	void	setPos( const VEC3D * pPos)	{ i3Vector::Copy( &_vPos, const_cast<VEC3D*>(pPos)); }

	INT16	getSectorIndex( void) const	{ return _SectorIdx; }
	void	setSectorIndex( INT16 idx)	{ _SectorIdx = idx; }

};

class GAMEINFO_ACTIONKEY : public GameNetworkInfoBase
{
protected:
	UINT8	_Count = 0;

	struct _tagActionDATA {
		UINT8	_nMoveKey = 0;					// 입력된 이동 키
		UINT8	_nActionKey = 0;				// 입력된 행동 키
		UINT16	_nUseActionObjectIdx = 0xFFFF;	// 사용할 오브젝트 인덱스

		void	Reset()
		{
			_nMoveKey		= 0;
			_nActionKey		= 0;
			_nUseActionObjectIdx = 0xFFFF;
		}
	};

	_tagActionDATA	_ActionData[ MAX_ACTION_COUNT ];

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;
	virtual UINT16	Pack(UINT8* pui8Count, char * pOutPacket, UINT32 ui32BufSize);

	BOOL			Inset( GameNetworkInfoBase* pDest );

	UINT8	getActionCount()										{ return _Count;						}
	void	setActionCount( UINT8 ui8Count )						{ _Count = ui8Count;					}

	UINT8	getMoveKey( INT32 i32Idx ) const						{ return _ActionData[i32Idx]._nMoveKey; }
	void	setMoveKey( INT32 i32Idx, UINT8 key)					{ _ActionData[i32Idx]._nMoveKey = key; }

	UINT8	getActionKey( INT32 i32Idx ) const						{ return _ActionData[i32Idx]._nActionKey; }
	void	setActionKey( INT32 i32Idx, UINT8 key)					{ _ActionData[i32Idx]._nActionKey = key; }
	void	removeActionKey( INT32 i32Idx, UINT8 key)				{ _ActionData[i32Idx]._nActionKey &= ~key; }

	UINT16	getUseActionObjectIndex( INT32 i32Idx) const			{ return _ActionData[i32Idx]._nUseActionObjectIdx; }
	void	setUseActionObjectIndex( INT32 i32Idx, UINT16 idx)		{ _ActionData[i32Idx]._nUseActionObjectIdx = idx; }
};

class GAMEINFO_CHARA_STATE : public GameNetworkInfoBase
{
protected:
	UINT8	_Body = 0;					// 캐릭터 액션 상태 (앉기 여부/하체)
	UINT8	_Body2 = 0;					// 캐릭터 액션 상태 (줌 상태 여부/상체)
	UINT8	_Upper = 0;					// 캐릭터 액션 상태 (상체)
	UINT8	_Lower = 0;					// 캐릭터 액션 상태 (하체)
	UINT8	_nFireAction = 0;			// 발사 동작 여부( 칼, 수류탄에서 사용됩니다)
	UINT8	_nHelmet = 0;				// 헬멧 유무 ( 0:None, 1:Drop, 2:Equip)
	
	UINT8	_nJumpState = 0;			// 점프관련 특수 상태 ( enum CHARA_ACTION_SPACE )
	UINT8	_FBState = 0;				// 풀바디 동작 : enum DINO_STATE 참조
	UINT8	_FBSubState = 0;			// 풀바디 세부 동작

	bool	_bWeaponExtension = false;
	UINT8	_nMultiWeaponSlot = 0;		// 다중 무기 슬롯 인덱스
	WEAPON_SLOT_TYPE	_nWeaponSlot = WEAPON_SLOT_UNKNOWN;
	WEAPON::FIRE_ORDER	_nWeaponFireOrder = WEAPON::PRIMARY_FIRE;

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	UINT16	Pack_FullBody( char * pOutPacket);
	UINT16	Unpack_FullBody( const char * pInPacket);

	UINT8	getBody( void) const				{ return _Body; }
	void	setBody( UINT8 body)				{ _Body = body; }

	UINT8	getBody2( void) const				{ return _Body2; }
	void	setBody2( UINT8 body2)				{ _Body2 = body2; }

	UINT8	getUpper( void) const				{ return _Upper; }
	void	setUpper( UINT8 upper)				{ _Upper = upper; }

	UINT8	getLower( void) const				{ return _Lower; }
	void	setLower( UINT8 lower)				{ _Lower = lower; }

	UINT8	getJumpState(void) const			{ return _nJumpState; }
	void	setJumpState(UINT8 state)			{ _nJumpState = state; }

	UINT8	getFireAction( void) const			{ return _nFireAction; }
	void	setFireAction( UINT8 action)		{ _nFireAction = action; }

	UINT8	getHelmet( void) const				{ return _nHelmet; }
	void	setHelmet( UINT8 helmet)			{ _nHelmet = helmet; }

	UINT8	getFBState( void) const				{ return _FBState; }
	void	setFBState( UINT8 state)			{ _FBState = state; }

	UINT8	getFBSubState( void) const			{ return _FBSubState; }
	void	setFBSubState( UINT8 state)			{ _FBSubState = state; }

	bool	getWeaponExtension( void) const		{ return _bWeaponExtension; }
	void	setWeaponExtension( bool bVal)		{ _bWeaponExtension = bVal; }

	WEAPON_SLOT_TYPE	getWeaponSlot( void) const			{ return _nWeaponSlot; }
	void	setWeaponSlot( WEAPON_SLOT_TYPE slot)			{ _nWeaponSlot = slot; }

	UINT8	getMultiWeapon()								{ return _nMultiWeaponSlot; }
	void	setMultiWeapon( UINT8 slot)						{ _nMultiWeaponSlot = slot; }

	WEAPON::FIRE_ORDER	getWeaponFireOrder( void) const			{ return _nWeaponFireOrder; }
	void	setWeaponFireOrder( WEAPON::FIRE_ORDER fireOrder)	{ _nWeaponFireOrder = fireOrder; }
};

class GAMEINFO_TAKING_OBJECT : public GameNetworkInfoBase
{
protected:
	bool	_bOnLoadObject = false;			// Object Onload check
	UINT16	_iOnLoadObjectIndex = 0;	// index

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	bool	getOnLoadObject( void) const		{ return _bOnLoadObject; }
	void	setOnLoadObject( bool bVal)			{ _bOnLoadObject = bVal; }

	UINT16	getOnLoadObjectIndex( void) const	{ return _iOnLoadObjectIndex; }
	void	setOnLoadObjectIndex( UINT16 idx)	{ _iOnLoadObjectIndex = idx; }
};

class GAMEINFO_SYNC_OBJECT : public GameNetworkInfoBase
{
protected:
	UINT8	_nSyncObjectState = 0;		// 오브젝트 상태에 따른 캐릭터 싱크 처리
	UINT16	_iSyncObjectIdx = 0;		// 오브젝트 상태에 따른 캐릭터 싱크 처리용 오브젝트 인덱스

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	UINT8	getState( void) const		{ return _nSyncObjectState; }
	void	setState( UINT8 state)		{ _nSyncObjectState = state; }

	UINT16	getIndex( void) const		{ return _iSyncObjectIdx; }
	void	setIndex( UINT16 idx)		{ _iSyncObjectIdx = idx; }
};

class GAMEINFO_RADIO_CHAT : public GameNetworkInfoBase
{
protected:
	UINT8	_nRadio = 0;				// Radio chat
	UINT8	_nSectorIdx = 0;			// World Sector Index

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	UINT8	getRadio( void) const		{ return _nRadio; }
	void	setRadio( UINT8 radio)		{ _nRadio = radio; }

	UINT8	getSectorIndex( void) const	{ return _nSectorIdx; }
	void	setSectorIndex( UINT8 idx)	{ _nSectorIdx = idx; }
};

// 히트 데이터
class GAMEINFO_HIT : public GameNetworkInfoBase
{
protected:
	INT32	_FireHitCount = 0;

	struct _tagHITDATA {
		GAMEINFO_HIT_TYPE	_Type = GHT_AI;
		GAMEINFO_HIT_BYTYPE	_ByType = GHBT_NONE;

		// Hit된 사람 ( max 32개입니다. 샷건은 복수개의 값이 들어옵니다. )
		UINT16	_FireHitWho = 0;		// Object와 Item의 Index로도 사용
		UINT16	_FireHitDamage = 0;		// Damage
		UINT8	_FireHitPart = 0;		// Hit된 사람의 Hitpart
		UINT8	_CritalFlag = 0;		// Default 0 CriticalHit 1
		UINT8	_HelmetFlag = 0;		// Drop Helmet 1이면 drop 2이면 Protection

		// Weapon Info
		GAMEINFO_CHARA_WEAPON	_WeaponInfo;
		VEC3D				_HitDirection;						// Dedicated
		CHARA_DEATH_TYPE	_DeathType = CHARA_DEATH_UN;		// Dedicated
		UINT16				_ByObjectIndex = 0;						// Dedicated
		UINT8				_ByObjectState = 0;						// Dedicated

		VEC3D	_FirePos;	// Dedicated2
		VEC3D	_HitPos;	// Dedicated2

		UINT16 HitbyObjectExplosion = 0;
		UINT8	m_ui8AttackType = 0;
	};

	_tagHITDATA		_tHitData[ MAX_FIREHIT_COUNT];

public:

	virtual void	Reset( void) override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	UINT16			Pack_SingleHit( char * pOutPacket, UINT32 ui32BufSize, INT32 idx);

	UINT16			Pack_Dedicated( char * pOutPacket, UINT32 ui32BufSize, UINT8 count);
	UINT16			Unpack_Dedicated( const char * pInPacket, INT32 & preIdx);

	UINT16			Pack_Dedicated_Explosion( char * pOutPacket, UINT32 ui32BufSize, UINT8 count, UINT16 netSlotIdx);
	UINT16			Unpack_Dedicated_Explosion( const char * pInPacket, INT32 & preIdx);

	UINT16			Pack_Dedicated_Object( char * pOutPacket, UINT32 ui32BufSize, UINT8 count);
	UINT16			Unpack_Dedicated_Object( const char * pInPacket, INT32 & preIdx);


	INT32	getHitCount( void)			{ return _FireHitCount; }
	void	setHitCount( INT32 count)	{ _FireHitCount = count; }

	GAMEINFO_HIT_TYPE	getHitType( INT32 idx) const	
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);
			return GHT_AI;
		}
		return _tHitData[idx]._Type; 
	}

	void	setHitType( INT32 idx, GAMEINFO_HIT_TYPE type)
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);	return;
		}
		_tHitData[idx]._Type = type; 
	}

	GAMEINFO_HIT_BYTYPE	getByType(INT32 idx) const
	{
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);
			return GHBT_NONE;
		}
		return _tHitData[idx]._ByType; 
	}
	void	setByType( INT32 idx, GAMEINFO_HIT_BYTYPE type)		
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT); return;
		}
		_tHitData[idx]._ByType = type; 
	}

	UINT16	getHitWho( INT32 idx) const	
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);	return 0;
		}
		return _tHitData[idx]._FireHitWho; 
	}
	void	setHitWho( INT32 idx, UINT16 who)	
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);	return;
		}
		_tHitData[idx]._FireHitWho = who; 
	}

	UINT8	getHitPart( INT32 idx) const
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);	return 0;
		}
		return _tHitData[idx]._FireHitPart; 
	}
	void	setHitPart( INT32 idx, UINT8 part)	
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT); return;
		}
		_tHitData[idx]._FireHitPart = part; 
	}



	UINT8	GAMEINFO_HIT::getCriticalFlag( INT32 idx) const
	{
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);	return 0;
		}
		return _tHitData[idx]._CritalFlag;
	}

	void	GAMEINFO_HIT::setCriticalFlag( INT32 idx, UINT8 dm)
	{
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);	return;
		}
		_tHitData[idx]._CritalFlag = dm;
	}
	
	
	UINT16	getHitbyObjectExplosion(  INT32 idx)const	{ return _tHitData[idx].HitbyObjectExplosion; }
	void	setHitbyObjectExplosion( INT32 idx, UINT16 Flag)	{ _tHitData[idx].HitbyObjectExplosion = Flag; }

	UINT16	getHitDamage( INT32 idx) const 
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);
			return 0;
		}
		return _tHitData[idx]._FireHitDamage; 
	}
	void	setHitDamage( INT32 idx, UINT16 dm)	
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT); return;
		}
		_tHitData[idx]._FireHitDamage = dm; 
	}

	UINT8	getHelmetFlag( INT32 idx) const
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);	return 0;
		}
		return _tHitData[idx]._HelmetFlag; 
	}
	void	setHelmetFlag( INT32 idx, UINT8 state)	
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);
			return;
		}
		_tHitData[idx]._HelmetFlag = state; 
	}

	UINT32	getWeaponID(INT32 idx)	
	{
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);
			return 0;
		}
			
		return _tHitData[idx]._WeaponInfo.getWeaponID(); 
	}

	void	setWeaponID(INT32 idx, UINT32 weaponID) 
	{
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);		return;
		}
		_tHitData[idx]._WeaponInfo.setWeaponID( weaponID); 
	}

	bool	getWeaponExtension( INT32 idx)				{ I3_BOUNDCHK( idx, MAX_FIREHIT_COUNT); return _tHitData[idx]._WeaponInfo.getWeaponExtension(); }
	void	setWeaponExtension( INT32 idx, bool bVal)	
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);
			return;
		}
		_tHitData[idx]._WeaponInfo.setWeaponExtension( bVal); 
	}

	GAMEINFO_CHARA_WEAPON *	getWeaponInfo( INT32 idx)	
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);
			return nullptr;
		}
		return &_tHitData[idx]._WeaponInfo; 
	}
		
	WEAPON_CLASS_TYPE	getWeaponClass(INT32 idx)	{ return getWeaponInfo(idx)->getWeaponClass(); }
	UINT32				getWeaponNumber(INT32 idx)	{ return getWeaponInfo(idx)->getWeaponNumber(); }
	WEAPON_SLOT_TYPE	getWeaponSlot(INT32 idx)	{ return getWeaponInfo(idx)->getWeaponUsage(); }

	VEC3D *	getHitDirection( INT32 idx)		
	{
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);
			return nullptr;
		}
		return &_tHitData[idx]._HitDirection; 
	}
	void	setHitDirection( INT32 idx, VEC3D * pDir)	
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);
			return;
		}
		i3Vector::Copy( &_tHitData[idx]._HitDirection, pDir); 
	}

	CHARA_DEATH_TYPE	getDeathType( INT32 idx) const		
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);
			return CHARA_DEATH_UN;
		}
		return _tHitData[idx]._DeathType; 
	}
	void	setDeathType( INT32 idx, CHARA_DEATH_TYPE type)	
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);	return;
		}

		_tHitData[idx]._DeathType = type; 
	}

	UINT8	getByObjectState( INT32 idx) const					
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);	return 0;
		}
		return _tHitData[idx]._ByObjectState; 
	}
	void	setByObjectState( INT32 idx, UINT8 state)		
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);	return;
		}
		_tHitData[idx]._ByObjectState = state; 
	}

	UINT16	getByObjectIndex( INT32 idx) const				
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);
			return 0;
		}
		return _tHitData[idx]._ByObjectIndex; 
	}
	void	setByObjectIndex( INT32 idx, UINT16 objIndex)	
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);			return;
		}
		_tHitData[idx]._ByObjectIndex = objIndex; 
	}

	UINT8	GetAttackType(INT32 idx)						{ I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT); return (UINT8)_tHitData[idx]._WeaponInfo.GetAttackType(); }
	void	SetAttackType(INT32 idx, UINT8 ui8AttackType)	
	{ 
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT); return;
		}
		_tHitData[idx]._WeaponInfo.SetAttackType( (WEAPON::ATTACK_TYPE)ui8AttackType);  
	}

#ifdef CLIENT_SIDE
	VEC3D	getFirePos(INT32 idx) const
	{
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);	return VEC3D();
		}
#if defined (USE_ARXAN)
		VEC3D _temp;
		AIP_Getter_HITINFO_FirePos(&_temp,&_tHitData[idx]._FirePos,sizeof(_temp));
		return _temp;
#else
		return _tHitData[idx]._FirePos;
#endif
	}
#else
	VEC3D*	getFirePos( INT32 idx)
	{
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);	return nullptr;
		}
		return &_tHitData[idx]._FirePos;
	}
#endif
	void	setFirePos( INT32 idx, const VEC3D* pPos)
	{
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);	return;
		}

#if defined (USE_ARXAN)
		AIP_Setter_HITINFO_FirePos(&_tHitData[idx]._FirePos, pPos,sizeof(_tHitData[idx]._FirePos));
#else
		i3Vector::Copy(&_tHitData[idx]._FirePos, (VEC3D*)pPos);
#endif
	}

#ifdef CLIENT_SIDE
	VEC3D	getHitPos( INT32 idx) const
	{
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);
			return VEC3D();
		}
	#if defined (USE_ARXAN)
		VEC3D _temp;
		AIP_Getter_HITINFO_HitPos(&_temp,&_tHitData[idx]._HitPos,sizeof(_temp));
		return _temp;
	#else
		return _tHitData[idx]._HitPos;
	#endif
	}
#else
	VEC3D*	getHitPos( INT32 idx)
	{
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);
			return nullptr;
		}
		return &_tHitData[idx]._HitPos;
	}
#endif

	void	setHitPos( INT32 idx, const VEC3D* pPos)
	{
		if (idx < 0 || idx >= MAX_FIREHIT_COUNT)
		{
			I3_BOUNDCHK(idx, MAX_FIREHIT_COUNT);	return;
		}
#if defined (USE_ARXAN)
		AIP_Setter_HITINFO_HitPos(&_tHitData[idx]._HitPos,pPos,sizeof(_tHitData[idx]._HitPos));
#else
		i3Vector::Copy(&_tHitData[idx]._HitPos, (VEC3D*)pPos);
#endif
	}

};

// 발사 패킷에 대한 게임 데이터입니다.
class GAMEINFO_FIRE : public GameNetworkInfoBase
{
protected:
	UINT16	_nId = 0;					// 발사 패킷 ID
	INT32	_nRand = 0;					// Random code값입니다.
	VEC3D	_vStartPos;				// 발사시의 포지션
	VEC3D	_vTargetPos;			// 발사시의 목표점

	REAL32	_TargetX = 0.0f;
	REAL32	_TargetZ = 0.0f;

	UINT16	_nNetSlotIdx = 0;			// 수류탄 계열의 슬롯 번호
	GAMEINFO_CHARA_WEAPON	_WeaponInfo;

#if defined( CHECK_LOADEDBULLET)
	UINT8	_LoadedBullet = 0;			// 탄약 수
	UINT8	_LoadedDualBullet = 0;		// 듀얼 탄창용
#endif

public:
	// 해당 Chara가 쏜 총알에 대한 Hit
	GAMEINFO_HIT _tHitData;

	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest ) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	UINT16	Pack_TargetPos( char * pOutPacket);
	UINT16	Unpack_TargetPos( const char * pInPacket);

	UINT16	Pack_Dino( char * pOutPacket);
	UINT16	Unpack_Dino( const char * pInPacket);

	UINT16	getID( void) const					{ return _nId; }
	void	setID( UINT16 id)					{ _nId = id; }

	INT32	getRand( void) const				{ return _nRand; }
	void	setRand( INT32 rand)				{ _nRand = rand; }

	UINT16	getNetSlotIndex( void) const		{ return _nNetSlotIdx; }
	void	setNetSlotIndex( UINT16 slot)		{ _nNetSlotIdx = slot; }
	void	addNetSlotIndex( void)				{ _nNetSlotIdx++; }
	
	const VEC3D *	getStartPosition( void) const	{ return &_vStartPos; }
	void	setStartPosition( const VEC3D * pPos)	{ i3Vector::Copy( &_vStartPos, const_cast<VEC3D*>(pPos)); }

	const VEC3D *	getTargetPosition( void) const	{ return &_vTargetPos; }
	void	setTargetPosition( const VEC3D * pPos)	{ i3Vector::Copy( &_vTargetPos, const_cast<VEC3D*>(pPos)); }

	REAL32	getTargetX( void) const					{ return _TargetX; }
	void	setTargetX( REAL32 x)					{ _TargetX = x; }

	REAL32	getTargetZ( void) const					{ return _TargetZ; }
	void	setTargetZ( REAL32 z)					{ _TargetZ = z; }

	const GAMEINFO_CHARA_WEAPON* getWeapon() const	{ return &_WeaponInfo;				}

	UINT32	getWeaponID( void) const				{ return _WeaponInfo.getWeaponID(); }
	void	setWeaponID( UINT32 id)					{ _WeaponInfo.setWeaponID( id); }

	UINT32	getWeaponUsage() const					{ return _WeaponInfo.getWeaponUsage();	}
	UINT32	getWeaponClass() const					{ return _WeaponInfo.getWeaponClass();	}
	UINT32	getWeaponNumber() const					{ return _WeaponInfo.getWeaponNumber();	}

	bool	getWeaponExtension( void) const			{ return _WeaponInfo.getWeaponExtension(); }
	void	setWeaponExtension( bool bVal)			{ _WeaponInfo.setWeaponExtension( bVal); }

	WEAPON::ATTACK_TYPE getWeaponAttackType( void) const	{ return _WeaponInfo.GetAttackType(); }
	void	setWeaponAttackType( WEAPON::ATTACK_TYPE type)	{ _WeaponInfo.SetAttackType(type); }

#if defined( CHECK_LOADEDBULLET)
	UINT8	getLoadedBulletCount( void) const		{ return _LoadedBullet;}
	void	setLoadedBulletCount( UINT8 cnt)		{ _LoadedBullet = cnt; }

	UINT8	getLoadedDualBulletCount( void) const	{ return _LoadedDualBullet; }
	void	setLoadedDualBulletCount( UINT8 cnt)	{ _LoadedDualBullet = cnt; }
#endif

	void	CopyToFire( GAMEINFO_FIRE * pDest)
	{
		pDest->_nId			= _nId;
		pDest->_nRand		= _nRand;
		i3Vector::Copy( &pDest->_vStartPos, &_vStartPos );
		i3Vector::Copy( &pDest->_vTargetPos, &_vTargetPos );
		pDest->_nNetSlotIdx	= _nNetSlotIdx;

		_WeaponInfo.CopyTo( &pDest->_WeaponInfo);

#if defined( CHECK_LOADEDBULLET)
		pDest->_LoadedBullet	= _LoadedBullet;
		pDest->_LoadedDualBullet = _LoadedDualBullet;
#endif
	}

	/** \brief 투척무기의 경우 해당 userIdx로 맞는 NetworkIdx를 부여 받은지 확인 */
	bool	isValidThrow( INT32 userIdx)
	{
		if( _nNetSlotIdx >= MAX_THROWWEAPON_COUNT )	return false;
		if( MAX_THROWWEAPON_SLOP * userIdx > _nNetSlotIdx )	return false;
		if( MAX_THROWWEAPON_SLOP * (userIdx + 1) <= _nNetSlotIdx ) return false;

		return true;
	}
} ;

//
// 맞은 부위에 대한 데이터입니다. Recv에만 사용
class GAMEINFO_SUFFER : public GameNetworkInfoBase
{
protected:
	VEC3D	_vHitDir;				// 맞는 방향
	UINT8	_nShooter = 0;				// 쏜 사람
	UINT8	_nShooterType = 0;			// 쏜 케릭터의 타입
	UINT8	_nHitpart = 0;				// 맞은 부위
	UINT8	_nDeathType = 0;			// 맞은 무기에 대해서 죽는 타입
	UINT8	_nNeedDamageMotion = 0;		// Suffer를 받는 쪽에서 데미지 모션 처리가 필요한 경우 1
	bool	_bDeath = false;
	UINT32	_WeaponItemID = 0;			// 맞은 무기 아이디.

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest ) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override { I3ASSERT_0; return 0;}
	virtual UINT16	Unpack( const char * pInPacket) override { I3ASSERT_0; return 0;}

	UINT16	Pack_Death( char * pOutPacket);
	UINT16	Unpack_Death( const char * pInPacket);

	UINT16	Pack_Suffer( char * pOutPacket);
	UINT16	Unpack_Suffer( const char * pInPacket);

	UINT16	Unpack_Death_ForDomination( const char * pInPacket);

	bool	getDeath( void) const			{ return _bDeath; }
	void	setDeath( bool bVal)			{ _bDeath = bVal; }

	UINT8	getDeathType( void) const		{ return _nDeathType; }
	void	setDeathType( UINT8 type)		{ _nDeathType = type; }

	UINT8	getShooterIndex( void) const	{ return _nShooter; }
	void	setShooterIndex( UINT8 idx)		{ _nShooter = idx; }

	UINT8	getShooterType( void) const		{ return _nShooterType; }
	void	setShooterType( UINT8 type)		{ _nShooterType = type; }

	UINT8	getHitPart( void) const			{ return _nHitpart; }
	void	setHitPart( UINT8 part)			{ _nHitpart = part; }

	const VEC3D * getHitDirection( void) const		{ return &_vHitDir; }
	void	setHitDirection( const VEC3D * pDir)	{ i3Vector::Copy( &_vHitDir, const_cast<VEC3D*>(pDir)); }

	UINT8	getNeedDamageMotion( void) const	{ return _nNeedDamageMotion; }
	void	setNeedDamageMotion( UINT8 val)		{ _nNeedDamageMotion = val; }
};

//
// 낙하 데미지 데이터
class GAMEINFO_SUICIDEDAMAGE : public GameNetworkInfoBase
{
protected:
	INT32					_Count = 0;

	struct _tagSUICIDEDAMAGE {
		CHARA_DEATH_TYPE	_DeathType = CHARA_DEATH_UN;
		CHARA_HIT_PART		_HitPart = CHARA_HIT_UNKNOWN;
		UINT16				_nDamage = 0;
		VEC3D				_vHitDir;
		UINT16				_nObjectIdx = 0;					// 0x00 ~ 0xFFFE		0xFFFF는 오브젝트가 아닌경우
		GAMEINFO_CHARA_WEAPON	_WeaponInfo;
		INT32				_KillerIdx = 0;							// 가해자 Index (Dedicated)
	};

	_tagSUICIDEDAMAGE		_tSuicideDamage[ MAX_SUICIDE_COUNT ];

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest ) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	UINT16					Pack_Single( char * pOutPacket, UINT32 ui32BufSize, INT32 idx );
	UINT16					UnPack_Single( const char* pInPacket, INT32 idx );

	INT32	getCount( void) const			{ return _Count; }
	void	setCount( INT32 cnt)			{ _Count = cnt; }

	CHARA_DEATH_TYPE	getDeathType( INT32 idx) const		{ return _tSuicideDamage[idx]._DeathType; }
	void	setDeathType( INT32 idx, CHARA_DEATH_TYPE type)	{ _tSuicideDamage[idx]._DeathType = type; }

	CHARA_HIT_PART getHitPart( INT32 idx) const				{ return _tSuicideDamage[idx]._HitPart; }
	void	setHitPart( INT32 idx, CHARA_HIT_PART part)		{ _tSuicideDamage[idx]._HitPart = part; }

	UINT16	getDamage( INT32 idx) const						{ return _tSuicideDamage[idx]._nDamage; }
	void	setDamage( INT32 idx, UINT16 dmg)				{ _tSuicideDamage[idx]._nDamage = dmg; }

	INT32	getKillerIndex( INT32 idx) const				{ return _tSuicideDamage[idx]._KillerIdx; }
	void	setKillerIndex( INT32 idx, INT32 index)			{ _tSuicideDamage[idx]._KillerIdx = index; }

	T_ItemID	getWeaponID( INT32 idx) const				{ return _tSuicideDamage[idx]._WeaponInfo.getWeaponID(); }
	void		setWeaponID( INT32 idx, T_ItemID id)		{ _tSuicideDamage[idx]._WeaponInfo.setWeaponID( id); }

	bool	getWeaponExtension( INT32 idx) const			{ return _tSuicideDamage[idx]._WeaponInfo.getWeaponExtension(); }
	void	setWeaponExtension( INT32 idx,bool bExtension)	{ _tSuicideDamage[idx]._WeaponInfo.setWeaponExtension( bExtension); }

	WEAPON::ATTACK_TYPE getAttackType( INT32 idx) const		{ return _tSuicideDamage[idx]._WeaponInfo.GetAttackType(); }
	void	setAttackType( INT32 idx, WEAPON::ATTACK_TYPE type) { _tSuicideDamage[idx]._WeaponInfo.SetAttackType(type); }

	const VEC3D * getHitDirection( INT32 idx) const			{ return &_tSuicideDamage[idx]._vHitDir; }
	void	setHitDirection( INT32 idx, const VEC3D * pDir)	{ i3Vector::Copy( &_tSuicideDamage[idx]._vHitDir, const_cast<VEC3D*>(pDir)); }
	void	zeroHitDirection( INT32 idx)					{ i3Vector::Zero( &_tSuicideDamage[idx]._vHitDir); }

	UINT16	getObjectIndex( INT32 idx) const				{ return _tSuicideDamage[idx]._nObjectIdx; }
	void	setObjectIndex( INT32 idx, UINT16 index)		{ _tSuicideDamage[idx]._nObjectIdx = index; }

	INT32	GetWeaponClass(INT32 idx) const					{ return _tSuicideDamage[idx]._WeaponInfo.getWeaponClass(); }
	INT32	GetWeaponUsage(INT32 idx) const					{ return _tSuicideDamage[idx]._WeaponInfo.getWeaponUsage(); }
	INT32	GetWeaponNumber(INT32 idx) const				{ return _tSuicideDamage[idx]._WeaponInfo.getWeaponNumber(); }
};

//
// Mission Data
class GAMEINFO_MISSION : public GameNetworkInfoBase
{
public:
	UINT32				_ItemID = 0;			// Item ID
	INT16				_ObjType = 0;
	INT16				_ObjIdx = 0;

private:
	NET_MISSION_STATE	_eState = NET_MISSION_NONE;
	BOMB_AREA			_eArea = BOMB_AREA_A;
	REAL32				_r32StartTime = 0.0f;

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest ) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	NET_MISSION_STATE	GetState(void)
	{
		return _eState;
	}
	void	SetState( NET_MISSION_STATE State)
	{
		_eState = State;
	}

	BOMB_AREA	GetArea(void)
	{
		return _eArea;
	}
	void	SetArea(BOMB_AREA Area)
	{
		_eArea = Area;
	}

	REAL32	GetStartTime(void)
	{
		return _r32StartTime;
	}
	void	SetStartTime(REAL32 StartTime)
	{
		_r32StartTime = StartTime;
	}
};

class GAMEINFO_BULLET
{
	UINT8					m_ui8LoadBullet[WEAPON::EXTENSION_MAX][WEAPON::HAND_TYPE_MAX] = { { 0,0 } };
	UINT16					m_ui16StandbyBullet[WEAPON::EXTENSION_MAX][WEAPON::HAND_TYPE_MAX] = { { 0, 0 } };
public:
	GAMEINFO_BULLET()	{}
	~GAMEINFO_BULLET()	{}

	void					Reset();
	void					CopyTo( GAMEINFO_BULLET* pDest );

	void					SetBullet( UINT8 (*ppui8LoadBullet)[WEAPON::HAND_TYPE_MAX], UINT16 (*ppui16StandbyBullet)[WEAPON::HAND_TYPE_MAX] );
	void					GetBullet( UINT8 (*ppui8LoadBullet)[WEAPON::HAND_TYPE_MAX], UINT16 (*ppui16StandbyBullet)[WEAPON::HAND_TYPE_MAX] );

	UINT8					GetLoadBulletCount( WEAPON::EXTENSION_TYPE eExt, WEAPON::HAND_TYPE eHand )						{	return m_ui8LoadBullet[ eExt ][ eHand ];		}
	void					SetLoadBulletCount( WEAPON::EXTENSION_TYPE eExt, WEAPON::HAND_TYPE eHand, UINT8 ui8Count )		{	m_ui8LoadBullet[ eExt ][ eHand ] = ui8Count;	}

	UINT16					GetStandbyBulletCount( WEAPON::EXTENSION_TYPE eExt, WEAPON::HAND_TYPE eHand )					{	return m_ui16StandbyBullet[ eExt ][ eHand ];		}
	void					SetStandbyBulletCount( WEAPON::EXTENSION_TYPE eExt, WEAPON::HAND_TYPE eHand, UINT16 ui16Count )	{	m_ui16StandbyBullet[ eExt ][ eHand ] = ui16Count;	}
	
	UINT16					GetMaxBulletCount( WEAPON::EXTENSION_TYPE eExt, WEAPON::HAND_TYPE eHand )						{	return m_ui8LoadBullet[eExt][eHand] + m_ui16StandbyBullet[eExt][eHand];	}
};

// 무기 아이디 : DropList의 아이디입니다( WeaponManager )
// GAMEINFO_GETWEAPON
class GAMEINFO_DROPWEAPON : public GameNetworkInfoBase
{
protected:
	UINT8					_ui8Success = 0;			// 0 - Fail, 1 - Success
	UINT8					_ui8DropType = 0;			// 0 - 기본 드랍, 1 - Death 드랍
	UINT8					_nIdx = 0;					// 드랍무기 인덱스 0 ~ 31
	UINT8					m_ui8MultiWeaponIdx = 0;	// [S->C] 습득한 무기의 MultiWeaponSlotIDX(다중무기슬롯이 아닐 경우, 0)
	GAMEINFO_BULLET			m_Bullet;
	UINT32					_ui32WeaponID = 0;

public:
	virtual void			Reset() override;
	virtual void			CopyTo( GameNetworkInfoBase * pDest ) override;
	virtual UINT16			Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16			Unpack( const char * pInPacket) override;
	
	UINT8					getSuccess( void) const					{ return _ui8Success; }
	void					setSuccess( UINT8 success )				{ _ui8Success = success; }

	UINT8					getDropType( void) const				{ return _ui8DropType; }
	void					setDropType( UINT8 type )				{ _ui8DropType = type; }

	UINT8					getIndex( void) const					{ return _nIdx; }
	void					setIndex( UINT8 idx)					{ _nIdx = idx; }

	INT32					getWeaponClass(void)					{	return (INT32)GET_ITEM_SUBTYPE(_ui32WeaponID);		}
	INT32					getWeaponNumber(void)					{	return (INT32)GET_ITEM_NUMBER(_ui32WeaponID);		}
	INT32					getWeaponUsage(void)					{	return Weapon::GetUsageFromItemID( _ui32WeaponID );	}

	GAMEINFO_BULLET*		GetBullet()								{	return &m_Bullet;	}

	UINT8					GetLoadBulletCount( WEAPON::EXTENSION_TYPE eExt, WEAPON::HAND_TYPE eHand )						{	return m_Bullet.GetLoadBulletCount( eExt, eHand );		}
	void					SetLoadBulletCount( WEAPON::EXTENSION_TYPE eExt, WEAPON::HAND_TYPE eHand, UINT8 ui8Count )		{	m_Bullet.SetLoadBulletCount( eExt, eHand, ui8Count );	}

	UINT16					GetStandbyBulletCount( WEAPON::EXTENSION_TYPE eExt, WEAPON::HAND_TYPE eHand )					{	return m_Bullet.GetStandbyBulletCount( eExt, eHand );		}
	void					SetStandbyBulletCount( WEAPON::EXTENSION_TYPE eExt, WEAPON::HAND_TYPE eHand, UINT16 ui16Count )	{	m_Bullet.SetStandbyBulletCount( eExt, eHand, ui16Count );	}

	UINT8					GetMultiWeaponSlot() const				{ return m_ui8MultiWeaponIdx; }
	void					SetMultiWeaponSlot( UINT8 idx ) 		{ m_ui8MultiWeaponIdx = idx; }

	UINT32					getWeaponID()							{ return _ui32WeaponID;	}
	void					setWeaponID( UINT32 id)					{ _ui32WeaponID = id; }
} ;

typedef class GAMEINFO_DROPWEAPON	GAMEINFO_GETWEAPON;

class GAMEINFO_DIRECTPICKUP : public GameNetworkInfoBase
{
protected:
	GAMEINFO_DROPWEAPON		m_DropWeapon;
	UINT32					_ui32WeaponID = 0;
public:
	virtual void			Reset() override;
	virtual void			CopyTo(GameNetworkInfoBase * pDest) override;
	virtual UINT16			Pack(char * pOutPacket, UINT32 ui32BufSize) override;
	virtual UINT16			Unpack(const char * pInPacket) override;

	UINT32					getWeaponID()			{ return _ui32WeaponID; }
	void					setWeaponID(UINT32 id)	{ _ui32WeaponID = id; }

	UINT8					getWeaponUsage()		{ return (UINT8)(GET_ITEM_TYPE(_ui32WeaponID) - 1); }

	GAMEINFO_DROPWEAPON*	getDropWeapon()			{ return &m_DropWeapon;	}
};

// 터치다운 관련 내용
class GAMEINFO_TOUCHDOWN : public GameNetworkInfoBase
{
protected:
	UINT16			_ui16TouchDownIdx = 0;			// 터치 다운 진행 시 터치한 오브젝트 인덱스
public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest ) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	UINT16	GetTouchDownIdx()					{	return _ui16TouchDownIdx;		}
	void	SetTouchDownIdx( UINT16 ui16Idx )	{	_ui16TouchDownIdx = ui16Idx;	}
};


class GAMEINFO_DOMI_DINO_GRENADE : public GameNetworkInfoBase
{
protected:
	INT8	m_type = INVALID_IDX;					// 어떤 수류탄인지 (enum eDINO_GRENADE)
	INT8	m_calculatorIdx = INVALID_IDX;		// 계산을 담당한 슬롯 번호(0-15)
	INT8	m_slot = INVALID_IDX;					// 몇번 수류탄 슬롯을 사용하는 수류탄인지
	VEC3D	m_targetPos;			// 수류탄의 최종 투척 목적지는 어디인지

public:
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest ) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	UINT8	getType( void) const			{ return m_type; }
	void	setType( INT8 type)				{ m_type = type; }

	UINT8	getCalculatorIndex( void) const	{ return m_calculatorIdx; }
	void	setCalculatorIndex( INT8 idx)	{ m_calculatorIdx = idx; }

	UINT8	getSlot( void) const			{ return m_slot; }
	void	setSlot( INT8 slot)				{ m_slot = slot; }

	const VEC3D * getTargetPos( void) const		{ return &m_targetPos; }
	void	setTargetPos( const VEC3D * pPos)	{ i3Vector::Copy( &m_targetPos, const_cast<VEC3D*>(pPos)); }
};


class GAMEINFO_DOMIOBJECT_REQUEST : public GameNetworkInfoBase
{
public:
	UINT32					_iInfoFlag = 0;

	N_PCINFO_DOMI_INSTALL	install;
	N_PCINFO_DOMI_REPAIR	repair;
	N_PCINFO_DOMI_UPGRADE	upgrade;
	N_PCINFO_DOMI_USE		supplyUse;
	N_PCINFO_DOMI_CATCH		dropItem;
	
	virtual void	Reset() override;
	virtual void	CopyTo( GameNetworkInfoBase* dest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;
};


//
// 수류탄의 관리용입니다.

class GAMEINFO_THROW_WEAPON_NETDATA : public GameNetworkInfoBase
{
protected:
	GAMEINFO_CHARA_WEAPON	_WeaponInfo;
	VEC3D	_vPos;								// 위치( CN은 CS로부터 수류탄의 위치를 받아옵니다 )
	VEC3D	_vDir;								// KEYWORD : WEAPON_C5	부착시의 방향
	UINT8	_nState = 0;							// 수류탄의 진행 상태 ( 1bit : Settled check)
	UINT8	_nBoundType = 0;						// 바운드 타입

	UINT8	_FlyCount = 0;							// 날린 횟수
	UINT8	_nRemainingTime = 0;					// 터질때까지 남은시간
	VEC3D	_vPrevPos;							//이전 위치
	VEC3D	_AttachObjectPos;					// Attach 됐을 때 Object 위치
public:
	virtual void	Reset( void) override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

#if defined( CLIENT_SIDE)
	VEC3D	GetPos(void)
	{
#if defined (USE_ARXAN)
		VEC3D _temp;
		AIP_Getter_THROW_Pos(&_temp,&_vPos,sizeof(_temp));
		return _temp;
#else
		return _vPos;
#endif	// (USE_ARXAN)
	}
#else
	VEC3D*	GetPos(void)		{	return &_vPos;	}
#endif

#if defined( CLIENT_SIDE)
	void	SetPos(VEC3D Pos)
	{
#if defined (USE_ARXAN)
		AIP_Setter_THROW_Pos(&_vPos,&Pos,sizeof(_vPos));
#else
		i3Vector::Copy(&_vPos, &Pos);
#endif	// (USE_ARXAN)
	}
#else
	void	SetPos(VEC3D* pPos)	{ i3Vector::Copy(&_vPos, pPos); }
#endif

#if defined( CLIENT_SIDE)
	VEC3D	GetDir(void)
	{
#if defined (USE_ARXAN)
		VEC3D _temp;
		AIP_Getter_THROW_Dir(&_temp,&_vDir,sizeof(_temp));
		return _temp;
#else
		return _vDir;
#endif	// (USE_ARXAN)
	}
#else
	VEC3D*	GetDir(void)		{	return &_vDir;	}
#endif

#if defined( CLIENT_SIDE)
	void	SetDir( VEC3D Dir )
	{
#if defined (USE_ARXAN)
		AIP_Setter_THROW_Dir(&_vDir,&Dir,sizeof(_vDir));
#else
		i3Vector::Copy(&_vDir, &Dir);
#endif	// (USE_ARXAN)
	}
#else
	void	SetDir(VEC3D* pDir)	{	i3Vector::Copy( &_vDir, pDir );	}
#endif

	UINT8	GetState(void)
	{
#if defined (USE_ARXAN)
		UINT8 _temp;
		AIP_Getter_THROW_State(&_temp,&_nState,sizeof(_temp));
		return _temp;
#else
		return _nState;
#endif
	}
	void	SetState(UINT8 State)
	{
#if defined (USE_ARXAN)
		AIP_Setter_THROW_State(&_nState,&State,sizeof(_nState));
#else
		_nState = State;
#endif
	}

	UINT8	GetBoundType(void)
	{
#if defined (USE_ARXAN)
		UINT8 _temp;
		AIP_Getter_THROW_BoundType(&_temp,&_nBoundType,sizeof(_temp));
		return _temp;
#else
		return _nBoundType; 
#endif
	}
	void	SetBoundType(UINT8 BoundType)
	{
#if defined (USE_ARXAN)
		AIP_Setter_THROW_BoundType(&_nBoundType,&BoundType,sizeof(_nBoundType));
#else
		_nBoundType = BoundType;
#endif
	}

#if defined( CLIENT_SIDE)
	VEC3D	GetAttachPos(void)
	{
#if defined (USE_ARXAN)
		VEC3D _temp;
		AIP_Getter_THROW_ObjectPos(&_temp, &_AttachObjectPos, sizeof(_temp));
		return _temp;
#else
		return _AttachObjectPos;
#endif	// (USE_ARXAN)
	}
#else
	VEC3D*	GetAttachPos(void) { return &_AttachObjectPos; }
#endif

#if defined( CLIENT_SIDE)
	void	SetAttachPos( VEC3D vec)
	{
#if defined (USE_ARXAN)
		AIP_Setter_THROW_ObjectPos(&_AttachObjectPos, &vec, sizeof(_AttachObjectPos));
#else
		i3Vector::Copy(&_AttachObjectPos, &vec);
#endif	// (USE_ARXAN)
	}
#else
	void	SetAttachPos(VEC3D* vec) { i3Vector::Copy(&_AttachObjectPos, vec); }
#endif


	GAMEINFO_CHARA_WEAPON* GetWeapon()				{	return &_WeaponInfo;			}

	UINT32	getWeaponID( void) const				{ return _WeaponInfo.getWeaponID(); }
	void	setWeaponID( UINT32 id)					{ _WeaponInfo.setWeaponID( id); }

	WEAPON::ATTACK_TYPE	getAttackType( void) const	{ return _WeaponInfo.GetAttackType(); }
	void	SetAttackType( WEAPON::ATTACK_TYPE type)	{ _WeaponInfo.SetAttackType( type); }

	bool	getWeaponExtension( void) const			{ return _WeaponInfo.getWeaponExtension(); }
	void	setWeaponExtension( bool bExtension)	{ _WeaponInfo.setWeaponExtension(bExtension); }

	UINT8	getFlyCount( void) const				{ return _FlyCount; }
	void	setFlyCount( UINT8 cnt)					{ _FlyCount = cnt; }

	UINT8	getRemainingTime( void) const			{ return _nRemainingTime; }
	void	setRemainingTime( UINT8 time)			{ _nRemainingTime = time; }

	void	resetPrevPos( void)						{ i3Vector::Zero( &_vPrevPos); }
	const VEC3D *	getPrevPos( void) const			{ return &_vPrevPos; }
	void	setPrevPos( const VEC3D * pVec)			{ i3Vector::Copy( &_vPrevPos, const_cast<VEC3D*>(pVec)); }

	void	resetAttachPos(void) { i3Vector::Zero(&_AttachObjectPos); }
};

struct GAMEINFO_THROW_WEAPON_ASSIST
{
	REAL32	_fInterpolateTime = 0.0f;
	VEC3D	_vPostPos;
	VEC3D	_vDir;

	INT32	_SendCount = 0;			///< UDP 소실을 해결하기 위해 여러번 보내기 위한 카운트
	REAL32	_fSendCountElapsedTime = 0.0f;

	void	Reset( void)
	{
		_fInterpolateTime = 0.f;
		i3Vector::Zero( &_vPostPos);
		i3Vector::Zero( &_vDir);
		_SendCount = 0;
		_fSendCountElapsedTime = 0.f;
	}

	void CopyTo(GAMEINFO_THROW_WEAPON_ASSIST* pDest)
	{
		pDest->_fInterpolateTime = _fInterpolateTime;
		i3Vector::Copy(&pDest->_vPostPos, &_vPostPos);
		i3Vector::Copy(&pDest->_vDir, &_vDir);
		pDest->_SendCount = _SendCount;
	}
};

class GAMEINFO_THROW_WEAPON
{
public:
#if defined( CLIENT_SIDE)
	WeaponBase * _pWeaponBase = nullptr;
#endif

	bool	_bRecv = false;
	
	GAMEINFO_THROW_WEAPON_NETDATA _tNetData;

	// 수류탄 정보의 싱크 보정용
	GAMEINFO_THROW_WEAPON_ASSIST _tAssistData;

	// for Send
	bool	_bSend = false;					// Send할지 체크합니다 ( 1이면 샌드 )
	REAL32	_fSendTime = 0.0f;				// sync 타임 계산용

	// 이 시간이 되면 보내도록 세팅합니다.(던진 수류탄이 바인드될때 세팅됨)
	REAL32	_fBroadcastCheckSleepTime = 0.0f;	// Sleep 된 수류탄의 싱크 타임
	REAL32	_fBroadcastCheckSyncTime = 0.0f;	// 날라가는 수류탄의 싱크 타임
	

public:
	void	Reset();
	void	CopyTo( GameNetworkInfoBase * pDest);

#if defined( CLIENT_SIDE)
	void	Bind( WeaponBase * pWeapon, REAL32 sleepTime, REAL32 syncTime, VEC3D * pInitPos);

	WeaponBase *	getWeaponBase( void) const		{ return _pWeaponBase; }
#endif
};

//#ifdef DOMI_STING_POISON_GRENADE
class GAMEINFO_THROW_WEAPON_DOMI_DINO : public GAMEINFO_THROW_WEAPON
{
protected:
	INT32 _iCalculatorIdx = 0;
	INT32 _iSlotIdx = 0;

public:
	INT32	getCalculatorIndex( void) const		{ return _iCalculatorIdx; }
	INT32	getSlotIndex( void) const			{ return _iSlotIdx; }

#if defined( CLIENT_SIDE)
	void	Bind(WeaponBase * pWeapon, REAL32 sleepTime, REAL32 syncTime, VEC3D * pInitPos, INT32 calculatorIdx, INT32 slotIdx)
	{
		GAMEINFO_THROW_WEAPON::Bind(pWeapon, sleepTime, syncTime, pInitPos);

		_iCalculatorIdx = calculatorIdx;
		_iSlotIdx = slotIdx;
	}
#endif
};

// 오브젝트
// I3_NETWORK_GAMEOBJECT_TYPE_NONE = 0,
// I3_NETWORK_GAMEOBJECT_TYPE_STATIC,			// 고정되어 있다
// I3_NETWORK_GAMEOBJECT_TYPE_MOVE,			// 이동값만
// I3_NETWORK_GAMEOBJECT_TYPE_DYNAMIC,			// 이동 + 회전
// I3_NETWORK_GAMEOBJECT_TYPE_ANIM,			// 애니메이션 키프레임

class GAMEINFO_OBJECT_NETDATA : public GameNetworkInfoBase
{
protected:
	REAL32			_fKey = 0.0f;			// Animation Object Key
	N_POS			_vPos;					// Position
	QUATERNION		_qRot = { 0.0f, 0.0f, 0.0f, 1.0f };	// Rotation
	INT32			_nShellCount = 0;		// WeaponBox용 ShellCount
	//INT32			_nMaxShellCount;		// WeaponBox용 MaxShellCount
	REAL32			_fRespawnTime = 0.0f;	// WeaponBox용 무기 RespawnTime
	T_OBJ			_ObjType;				//	Object 종류 bit flag
	UINT16			_nHP = 0;				// HP
	UINT8			_EventSender = 0xFF;	// User Index (0~16) Damage 깍은 유저 인덱스
	UINT8			_nState = 0;			// Object State (0x80 using anim2, 0x0F : pathindex)
	UINT8			_EnterCharaIdx = 0xFF;	// Entering User index ( 0~16)
	UINT8			_nPathKey = 0;			// Animation Path Key
	
public:
	virtual void	Reset( void) override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	UINT32	PackPacket( char * pOutPacket, UINT32 ui32BufSize );

	UINT8	getState( void) const				{ return _nState; }
	void	setState( UINT8 state)				{ _nState = state; }


	UINT16	getHP( void) const					{ return _nHP; }
	void	setHP( UINT16 hp)					{ _nHP = hp; }

	const N_POS *	getPosition( void) const	{ return &_vPos; }
	void	setPosition( const N_POS * pPos)	{ i3mem::Copy( &_vPos, pPos, sizeof(N_POS) ); }
	void	setPosition( const VEC3D * pPos)	{ N_POS nPos;	UDP_UTIL::ConvertVec3DtoNet3D( &nPos, (VEC3D*)pPos ); i3mem::Copy( &_vPos, &nPos, sizeof(N_POS) ); }

	const QUATERNION * getRotation( void) const	{ return &_qRot; }
	void	setRotation( const QUATERNION * pRot)	{ i3Quat::Copy( &_qRot, const_cast<QUATERNION*>(pRot)); }

	UINT8	getEnterCharaIndex( void) const		{ return _EnterCharaIdx; }
	void	setEnterCharaIndex( UINT8 idx)		{ _EnterCharaIdx = idx; }

	UINT8	getEventSender( void) const			{ return _EventSender; }
	void	setEventSender( UINT8 idx)			{ _EventSender = idx; }

	UINT8	getPathKey( void) const				{ return _nPathKey; }
	void	setPathKey( UINT8 path)				{ _nPathKey = path; }

	REAL32	getKeyFrame( void) const			{ return _fKey; }
	void	setKeyFrame( REAL32 frame)			{ _fKey = frame; }

	INT32	getShellCount( void) const			{ return _nShellCount; }
	void	setShellCount( INT32 cnt)			{ _nShellCount = cnt; }

	//INT32	getMaxShellCount( void) const		{ return _nMaxShellCount; }
	//void	setMaxShellCount( INT32 cnt)		{ _nMaxShellCount = cnt; }

	REAL32	getRespawnTime( void) const			{ return _fRespawnTime; }
	void	setRespawnTime( REAL32 time)		{ _fRespawnTime = time; }
	
	T_OBJ	getObjectType( void) const			{ return _ObjType; }
	void	setObjectType( T_OBJ type)			{ _ObjType = type; }
	void	addObjectType( T_OBJ type)			{ _ObjType |= type; }

	bool CheckObjDataType(T_OBJ Objtype, ...)
	{
		va_list ap;
		bool check = true;
		T_OBJ type;
		va_start(ap, Objtype);
		for(INT32 i = 0; i < Objtype; i++)
		{
			type = va_arg(ap, T_OBJ);
			if(!( _ObjType & type))
			{
				check = false;
				break;
			}
		}
		va_end (ap); 
		return check;
	}

	INFO_DATA_TYPE	GetObjDataType(void)
	{
		if( CheckObjDataType(5, UDP_OBJECT_TYPE_STATE , UDP_OBJECT_TYPE_HP , UDP_OBJECT_TYPE_OWN, UDP_OBJECT_TYPE_ANIMPATH, UDP_OBJECT_TYPE_ANIMKEY  ))
		{	
			return INFO_DATA_OBJECT_ANIM;
		}
		else if( CheckObjDataType(2, UDP_OBJECT_TYPE_OWN, UDP_OBJECT_TYPE_POSITION))
		{
			return INFO_DATA_OBJECT_MOVE;
		}
		else if( CheckObjDataType(2, UDP_OBJECT_TYPE_SHELLCOUNT, UDP_OBJECT_TYPE_RESPAWN ))
		{	
			return INFO_DATA_STAGEINFO_CONTROLED_OBJECT;
		}
		else if( CheckObjDataType(2, UDP_OBJECT_TYPE_ACTIVE))
		{
			return INFO_DATA_OBJECT_STATIC;
		}
		return INFO_DATA_NONE;
	}

	void	CopyStaticObj( GAMEINFO_OBJECT_NETDATA * pDest)
	{
		pDest->_nHP = _nHP;
		pDest->_EventSender = _EventSender;
	}

	void	CopyMovementObj( GAMEINFO_OBJECT_NETDATA * pDest)
	{
		CopyStaticObj( pDest);

		i3mem::Copy( &pDest->_vPos, &_vPos, sizeof(N_POS) );
	}

	void	CopyDynamicObj( GAMEINFO_OBJECT_NETDATA * pDest)
	{
		CopyStaticObj( pDest);

		i3mem::Copy( &pDest->_vPos, &_vPos, sizeof(N_POS) );
		i3Quat::Copy( &pDest->_qRot, &_qRot );
	}

	void	CopyAnimationObj( GAMEINFO_OBJECT_NETDATA * pDest)
	{
		CopyStaticObj( pDest);

		pDest->_nState		= _nState;
		pDest->_fKey		= _fKey;
	}
};

struct GAMEINFO_OBJECT_ASSIST
{
	VEC3D	_vPostPos;
	QUATERNION	_qPostRot = { 0.0f, 0.0f, 0.0f, 1.0f };
	REAL32	_fAssistTime = 0.0f;

	void	Reset( void)
	{
		i3Vector::Zero( &_vPostPos );
		i3Quat::Identity( &_qPostRot );
		_fAssistTime = 0.f;
	}
};


class GAMEINFO_OBJECT
{
public:
#if defined( CLIENT_SIDE)
	i3Object *	_pObjectRef = nullptr;
#endif
	I3_NETWORK_GAMEOBJECT_TYPE _nType = I3_NETWORK_GAMEOBJECT_TYPE_NONE;
	REAL32				_rRadius = 0.0f;
	UINT8				_nInfoSlotIdx = 0;		// 해당 오브젝트 종류 리스트에서의 인덱스(순번이 됩니다)
	bool				_bIsSleep = false;
	bool				_bIntrudeCheck = false;
	bool				_bIntrudeCheckForControledObject = false;

	// Client : 리시브로 받은 데이터 버퍼
	// Host : 센드용으로 사용합니다.
	GAMEINFO_OBJECT_NETDATA _tNetData;

	// 보정용입니다. 리시브시에 세팅됩니다.
	GAMEINFO_OBJECT_ASSIST	_tAssistData;
	
	VEC3D * _vHiterPos = nullptr;

	// 방장만 해당됩니다.
	bool	_bBroadcast = false;	// (only CS)
	REAL32	_fBroadcastTime = 1.0f;// (only CS)

	void	ResetGameInfoObject(void)
	{
#if defined( CLIENT_SIDE)
		_pObjectRef			= nullptr;
#endif
		_vHiterPos			= nullptr;
		_rRadius			= 0.f;
		_nType				= I3_NETWORK_GAMEOBJECT_TYPE_NONE;
		_nInfoSlotIdx		= 0;
		_bBroadcast			= false;
		_fBroadcastTime		= 1.f;
		_bIsSleep	= false;
		_bIntrudeCheck		= false;
		_bIntrudeCheckForControledObject = false;
		_tNetData.Reset();
		_tAssistData.Reset();
	}

};

class GAMEINFO_DROPEDWEAPON_NETDATA : public GameNetworkInfoBase
{
protected:
	//UINT8				_nState;				// 드랍된 상태 ( 0x80 : dropcheck)
	UINT8				_ui8IsDrop = 0;
	UINT8				_ui8State = 0;
	VEC3D				_vPos;					// 위치
	QUATERNION			_qRot = { 0.0f, 0.0f, 0.0f, 1.0f };					// 회전

public:
	virtual void	Reset( void) override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	//void	addState( UINT8 state)				{ _nState |= state; }
	//void	removeState( UINT8 state)			{ _nState &= ~state; }
	UINT8	getDrop()							{ return _ui8IsDrop;	}
	void	setDrop( UINT8 ui8Set )				{ _ui8IsDrop = ui8Set;	}

	UINT8	getState( void) const				{ return _ui8State; }
	void	setState( UINT8 state)				{ _ui8State = state; }

	const VEC3D *	getPosition( void) const	{ return &_vPos; }
	void	setPosition( const VEC3D * pPos)	{ i3Vector::Copy( &_vPos, const_cast<VEC3D*>(pPos)); }

	const QUATERNION * getRotation( void) const	{ return &_qRot; }
	void	setRotation( const QUATERNION * pRot)	{ i3Quat::Copy( &_qRot, const_cast<QUATERNION*>(pRot)); }
} ;

struct GAMEINFO_DROPEDWEAPON_ASSIST
{	
	REAL32			_fAssistTime = 0.0f;			// 인터폴레이션 타임
	VEC3D			_vPostPos;
	QUATERNION		_qPostRot = { 0.0f, 0.0f, 0.0f, 1.0f };
	VEC3D			_vDir;					// 던진 방향

	void	Reset( void)
	{
		_fAssistTime	= 0.f;
		i3Vector::Zero( &_vPostPos);
		i3Quat::Identity( &_qPostRot);
		i3Vector::Zero( &_vDir);
	}
};

struct GAMEINFO_DROPED_WEAPON
{
#if defined( CLIENT_SIDE)
	WeaponBase *	_pWeaponRef = nullptr;
#endif
	
	bool	_bRecv = false;					// 받은 여부
	REAL32	_fDropedWeaponLifeTime = 0.0f;	// 라이프 타임

	// 네트워크 데이터
	// Client : 리시브용으로 사용
	// Host : 센드용으로 사용
	GAMEINFO_DROPEDWEAPON_NETDATA	_tNetData;

	// 보정용
	GAMEINFO_DROPEDWEAPON_ASSIST	_tAssistData;

	// for CS
	bool	_bSend = false;
	REAL32	_fSendTime = 0.0f;

	void	Reset(void)
	{
#if defined( CLIENT_SIDE)
		_pWeaponRef				= nullptr;
#endif
		_bRecv					= false;
		_fDropedWeaponLifeTime	= 0.f;
		_tNetData.Reset();
		_tAssistData.Reset();
		_bSend					= false;
		_fSendTime				= 0.f;
	}

	void	setDroped( bool bDrop)
	{
		if( bDrop )
			_tNetData.setDrop( 1 );
		else
			_tNetData.setDrop( 0 );
	}
	bool	isDroped( void)
	{
		if( _tNetData.getDrop() )
			return true;
		return false;
	}

	NET_WEAPONDROPED_STATE	getState( void)
	{
		return (NET_WEAPONDROPED_STATE) (_tNetData.getState() & 0x0F);
	}
	void	setState( NET_WEAPONDROPED_STATE state )
	{
		_tNetData.setState( (_tNetData.getState() & 0xF0) | (UINT8)state);
	}

#if defined( CLIENT_SIDE)
	// 게임 정보를 pWeapon과 Bind한다.
	// sendTime이 0.03초만큼 처음 패킷은 조금더 빨리 보내줍니다.
	void	Bind( WeaponBase * pWeapon, CGameCharaBase * pChara, REAL32 sendTime = 0.03f);
#endif

};

//////////////////////////////////////////////////////////////////////////////////////////////
//					NPC game info
class GAMEINFO_NPC : public GameNetworkInfoBase
{
protected:
	UINT8	_nState = 0;
	UINT8	_nHP = 0;

	VEC3D	_vTargetPos;
	INT32	_nHitUserPart = 0;

	bool	_bBroadcast = false;

public:
	virtual void	Reset( void) override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override {}
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;
};

class GAMEINFO_MISSION_INTRUDER : public GameNetworkInfoBase
{
protected:
	UINT8 m_MissionMode = 0;			// 설치 여부
	INT32 m_MissionInstallWho = 0;		// 설치자	// 0 ~ 15
	VEC3D m_MissionInstalledPos;	// 설치 위치

public:
	virtual void	Reset( void) override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	UINT8	getInstallMode( void) const					{ return m_MissionMode; }
	void	setInstallMode( UINT8 mode)					{ m_MissionMode = mode; }

	INT32	getInstallerIndex( void) const				{ return m_MissionInstallWho; }
	void	setInstallerIndex( INT32 idx)				{ m_MissionInstallWho = idx; }

	const VEC3D * getInstallPosition( void) const		{ return &m_MissionInstalledPos; }
	void	setInstallPosition( const VEC3D * pPos)		{ i3Vector::Copy( &m_MissionInstalledPos, const_cast<VEC3D*>(pPos)); }
};

////////////////////////////////////////////////////////////////////////////
//
// 캐릭터 정보 버퍼입니다.
class GAMEINFO_CHARA_NETDATA : public GameNetworkInfoBase
{
public:
	UINT32					_iInfoFlag = 0;

	GAMEINFO_CHARA_HP		_tHP;
	GAMEINFO_CHARA_STATE	_tCharaState;
	GAMEINFO_ACTIONKEY		_tActionKey;
	GAMEINFO_TAKING_OBJECT	_tTakingObject;
	GAMEINFO_SYNC_OBJECT	_tSyncObject;
	GAMEINFO_RADIO_CHAT		_tRadioChat;

	GAMEINFO_CHARA_WEAPON	_tWeaponData;
	GAMEINFO_CHARA_POSROT	_tPosRotData;
	GAMEINFO_FIRE			_tFireData;					// 발사 데이터(발사시의 히트 데이터 포함)
	GAMEINFO_SUFFER			_tSufferData;				// 캐릭터 자신의 피격 데이터입니다.(수류탄)
	GAMEINFO_SUICIDEDAMAGE	_tSuicideDamageData;		//
	GAMEINFO_MISSION		_tMissionData;
	GAMEINFO_DROPWEAPON 	_tDropWeaponData;
	GAMEINFO_GETWEAPON		_tGetWeaponData;
	UINT16					_ui16TouchDownIdx = 0;			// 터치 다운 진행 시 터치한 오브젝트 인덱스
	GAMEINFO_DIRECTPICKUP	_tDirectPickUp;					// 무기를 바로 버리고 장착하는 패킷


	GAMEINFO_DOMI_DINO_GRENADE _tDomiDinoGrenadeData;
	GAMEINFO_DOMIOBJECT_REQUEST _tDomiObjectData;

	GAMEINFO_CHARA_NETDATA() { Reset(); }

	virtual void	Reset( void) override;
	virtual void	CopyTo( GameNetworkInfoBase * pDest) override;
	virtual UINT16	Pack( char * pOutPacket, UINT32 ui32BufSize ) override;
	virtual UINT16	Unpack( const char * pInPacket) override;

	UINT16	Pack_StageInfo( char * pOutPacket);
	UINT16	Unpack_StageInfo( const char * pInPacket);

	bool	SetInfo_ActionState( UINT8 body, UINT8 body2, UINT8 upper, UINT8 lower, UINT8 fireAction, UINT8 jumpstate, WEAPON::FIRE_ORDER fireOrder);
	bool	SetInfo_Movement( INT16 sectorIdx, VEC3D * pPos, REAL32 theta, REAL32 rho);

	/** \brief TakingObject 정보 설정
		\param[in] bOnLoad 탑승 여부
		\param[in] objIndex 탑승한 오브젝트 Index
		\return 패킷 구성 여부		*/
	bool	SetInfo_TakingObject( bool bOnLoad, UINT16 objIndex);
	bool	SetInfo_Weapon( UINT32 weaponID, bool bExtension, WEAPON::ATTACK_TYPE type);
	bool	SetInfo_Fire( UINT32 weaponID, bool bExtension, WEAPON::ATTACK_TYPE type);
};//4534byte


//TCP->UDP..................................................................................
class GAMEINFO_AI_DINO
{
public:
	INT8 i8type = 0;					//enum DINO_KIND
	INT8 i8SubMask = 0;					// 0  0  0  0  LIFE  GRENADE  BULLET  TARGET
	INT32 i32Slot = -1;

	GAMEINFO_CHARA_NETDATA info;

	void	Reset()
	{
		i8type = 0;
		i8SubMask = 0;
		i32Slot = -1;
		info.Reset();
	}
};

//..........................................................................................



// for P2P data recv buffer reference
class RECV_P2P_DATA
{
public:
	INT32		Type = 0;			// 종류인데요.. Type에 따라 캐릭터, 오브젝트,무기 등이 설정됩니다
	INT32		Idx = -1;			// 해당 타입에서의 인덱스이고요 (캐릭터 슬롯 인덱스나 오브젝트의 인덱스입니다, 무기라면 무기매니져의 리스트 인덱스 이고요)
	UINT8		iPing = 0;			// bool : 방장이 핑패킷을 요청하는 경우 설정
	UINT8		iRoundNum = 0;		// 라운드 수
	REAL32		rPingSendTime = 0.0f;	// 핑패킷 요청시 게임타이머를 설정 (방장이 보낸 시간)
	void	*	pArg = nullptr;			// 패킷 데이터는 pArg에 포인터로 해서 게임쪽으로 넘겨 줌

	void	Reset() 
	{
		Type		= 0;
		Idx			= -1;
		iPing		= 0;
		iRoundNum	= 0;
		rPingSendTime = 0.f;
		pArg		= nullptr;
	}
};

struct GAMETOCONTEXT_INFO_CHARA_DEATH
{
	INT32				_iKillidx = -1;
	INT32				_iDeathidx = -1;
	UINT32				_ui32WeaponID = 0;
	CHARA_DEATH_TYPE	_nDeathType = CHARA_DEATH_UN;
	CHARA_HIT_PART		_nHitPart = CHARA_HIT_UNKNOWN;
	VEC3D *				_pHitDir = nullptr;
	INT32				_iByObjectIdx = -1;
	INT32				_iAssistIdx = 0;

	void	Reset( void)
	{
		_iKillidx			= -1;
		_iDeathidx			= -1;
		_ui32WeaponID		= 0;
		_nDeathType			= CHARA_DEATH_UN;
		_nHitPart			= CHARA_HIT_UNKNOWN;
		_pHitDir			= nullptr;
		_iByObjectIdx		= -1;
		_iAssistIdx			= 0;
	}

	bool	IsSameClass(UINT32 classType)
	{
		return (GET_ITEM_SUBTYPE(_ui32WeaponID) == classType);
	}

	bool	IsSameNumber(UINT32 number)
	{
		return (GET_ITEM_NUMBER(_ui32WeaponID) == number);
	}

};

#endif
