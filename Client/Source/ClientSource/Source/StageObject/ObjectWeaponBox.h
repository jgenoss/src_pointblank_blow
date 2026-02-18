#if !defined( __OBJECT_RPG7BOX_H__)
#define __OBJECT_RPG7BOX_H__

#include "GameControledObject.h"

class WeaponBase;
class CGameCharaBase;

class CGameObjectWeaponBox : public CGameControledObject
{
private:
	I3_EXPORT_CLASS_DEFINE( CGameObjectWeaponBox, CGameControledObject);
protected:
	i3Node	*		m_pWeaponPointDummy;
	WeaponBase *	m_pWeaponInBox;

	INT32			m_iShellCount;
	INT32			m_iMaxShellCount;
	REAL32			m_rShellRespawnTimer;
	REAL32			m_rShellRespawnTime;
	INT32			m_iGroupIdx;

	i3::vector<CGameObjectWeaponBox*> m_SameGroupRPG7BoxList;
	INT32			m_iUniqNumber;
	EWEAPON_TYPE	m_eWeaponType;

protected:
	WeaponBase *	_MakeWeapon( CGameCharaBase * pOwner);

public:
	CGameObjectWeaponBox( void);
	virtual ~CGameObjectWeaponBox( void);

	virtual UINT32	OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData) override;
	virtual UINT32	OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz) override;
	virtual UINT32	OnSave(i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad(i3ResourceFile * pResFile) override;

#if defined(I3_DEBUG)//XML
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
#endif

	virtual void	InitObject() override;
	virtual void	ResetObject() override;
	virtual void	EnableObject() override { EnableObjectInBox(); }
	void			EnableObjectInBox();
	void			DisableObjectInBox();
	virtual void	ProcessObject( REAL32 rDeltaSeconds) override;

	virtual void	SetControledObjectState( INT32 state) override;
	virtual INT32	GetControledObjectState(void) override {	return -1;}

	void			setShellCount( INT32 count)				{ m_iShellCount = count; }
	INT32			getShellCount( void)					{ return m_iShellCount; }

	void			setMaxShellCount( INT32 count)			{ m_iMaxShellCount = count; }
	INT32			getMaxShellCount( void)					{ return m_iMaxShellCount; }

	void			setRPG7RespawnTime( REAL32 time)		{ m_rShellRespawnTime = time; }
	REAL32			getRPG7RespawnTime( void)				{ return m_rShellRespawnTime; }

	void			setRespawnTimer( REAL32 tm)				{ m_rShellRespawnTimer = tm; }
	REAL32			getRespawnTimer( void)					{ return m_rShellRespawnTimer; }

	void			setRPG7GroupIdx( INT32 idx)				{ m_iGroupIdx = idx; }
	INT32			getRPG7GroupIdx( void)					{ return m_iGroupIdx; }

	bool			HoldToChara( CGameCharaBase * pChara);
	bool			HoldToCharaUserpation( CGameCharaBase * pChara);
	bool			HoldToCharaNomal( CGameCharaBase * pChara);

	void			CreateWeaponInBox( void);

	void			SetWeaponType(EWEAPON_TYPE eWeaponType) { m_eWeaponType = eWeaponType; }
	EWEAPON_TYPE	GetWeaponType() { return m_eWeaponType; }
	bool		m_bCleaverKnifeUsed;
};

#endif
