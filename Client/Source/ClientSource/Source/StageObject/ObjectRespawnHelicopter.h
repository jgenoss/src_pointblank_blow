#if !defined( __OBJECT_RESPAWN_HELICOPTER_H__)
#define __OBJECT_RESPAWN_HELICOPTER_H__

#include "ObjectWithWeapon.h"

enum RESPAWN_HELICOPTER_PATH
{
	RESPAWN_HELICOPTER_PATH_NONE = 0,
	RESPAWN_HELICOPTER_PATH_RED1,	// 0 group
	RESPAWN_HELICOPTER_PATH_RED2,
	RESPAWN_HELICOPTER_PATH_BLUE1,
	RESPAWN_HELICOPTER_PATH_BLUE2,	
	RESPAWN_HELICOPTER_PATH_RED3,	// 1 group
	RESPAWN_HELICOPTER_PATH_RED4,
	RESPAWN_HELICOPTER_PATH_BLUE3,
	RESPAWN_HELICOPTER_PATH_BLUE4,

	RESPAWN_HELICOPTER_PATH_COUNT,
};

class WeaponM197;
class CGameCharaBase;

class CGameObjectRespawnHelicopter : public CGameObjectWithWeapon
{
	I3_EXPORT_CLASS_DEFINE( CGameObjectRespawnHelicopter, CGameObjectWithWeapon);

protected:
	RESPAWN_HELICOPTER_PATH	m_PathKey;
	i3Animation *			m_pPathAnim[ RESPAWN_HELICOPTER_PATH_COUNT];
	REAL32					m_rPathLocalTime;
	
	bool					m_bMyEnterObject;
	bool					m_bDestroyed;

	REAL32					m_rRespawnTime;
	REAL32					m_rSendCountTime;

	i3Transform *			m_pRespawnDummy;
	i3Transform *			m_pRootDummy;
	i3Transform *			m_pRespawnDummy2;

	INT32					m_nTeam;
	bool					m_bFirstAnimSet;

	i3Transform2 *			m_pHitShapeSetTrans;

	bool					m_bWarningNotice;

protected:
	void					_SetPathAnimKey( RESPAWN_HELICOPTER_PATH pathKey, REAL32 rStartTime = 0.f);
	void					_FirstUpdateAnim( void);

public:
	i3Transform *			getRespawnDummy( void)						{ return m_pRespawnDummy; }
	i3Transform *			getRootDummy( void)							{ return m_pRootDummy; }
	i3Transform *			getRespawnDummy2(void)						{ return m_pRespawnDummy2;}

	RESPAWN_HELICOPTER_PATH	getPathKey( void)							{ return m_PathKey; }
	REAL32					getLocalAnimTime( void)						{ return m_rPathLocalTime; }

	INT32					getTeamIdx( void)							{ return m_nTeam;}
	void					setTeamIdx( INT32 idx)						{ m_nTeam = idx; }

public:
	CGameObjectRespawnHelicopter( void);
	virtual ~CGameObjectRespawnHelicopter( void);

	virtual UINT32	OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData) override;
	virtual UINT32	OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz) override;
	virtual UINT32	OnSave(i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad(i3ResourceFile * pResFile) override;
#if defined(I3_DEBUG)//XML
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
#endif

	virtual void			InitObject() override;
	virtual void			ResetObject() override;
	virtual void			EnableObject() override;
	virtual void			ProcessObject( REAL32 rDeltaSeconds) override;
	virtual void			SetControledObjectState( INT32 state) override;

	void					InitAnimation( void);
	void					Destroy(void);
	void					EnableDustEffect( bool bEnable);
	bool					IsEnableObject();

	MATRIX *				GetRespawnPose( void);

	// 헬기 패킷 처리 
private:
	bool m_bUseObject_Flag;
	INT32 m_iUseObject_Count;

	bool m_bLeaveWeapon_Flag;
	INT32 m_iLeaveWeapon_Count;

	bool m_bEnterWeapon_Flag;
	INT32 m_iEnterWeapon_Count;

public:
	void					InitFlag();
	void					SendLeaveSyncUH60();
};

#endif
