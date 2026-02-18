#pragma once

#include "../../Avatar/AvatarDefine.h"
#include "GameCharaBase.h"

class CGameCharaManager;
class AvatarGen;

typedef void (*AvatarLoadedCallback)(void * pAvatar, INT32 idx, AI_BIND_DATA * pAIBind);

class Avatar : public CGameCharaBase
{
	I3_CLASS_DEFINE( Avatar, CGameCharaBase); 

	friend class CGameCharaEquipContext;
	friend class cAvatarManager;
	friend class CGameCharaManager;

private:
	i3::vector<i3VertexArray*>		m_EquipmentVAList;	// 장비용 VA들

	struct tagTeamBandInfo {
		i3Node *			_pNode = nullptr;
		i3TextureBindAttr *	_pAttr = nullptr;
	};
	i3::vector<tagTeamBandInfo>		m_TeamBandSgList;

	typedef i3::vector<i3Node*>		tagHeadResInfo;
	i3::vector<tagHeadResInfo>		m_HeadSgList;	// 머리 리소스를 교체하기 위한 리스트 (0, 1, 2 번 순의 타입으로 들어간다)

private:
	void			__SearchTeamBandSg( void);
	void			__SearchHeadSg( void);

protected:
	AvatarLoadedCallback	m_pfGameNodeEvent = nullptr;
	AvatarLoadedCallback	m_pLoadFinishCallBack = nullptr;

	AvatarGen *		m_pGen;			// Generator [initialize at constructor]
	i3Body *		m_pBody = nullptr;		// 실제 Body
	i3Body *		m_pTempBody = nullptr;	// 로딩용 Body, m_pBody로 교체후 nullptr로 초기화한다.

	bool			m_bLoaded = false;		//현재 로드 되었는지 체크
	bool			m_bReleaseQue = false;	// 삭제될 녀석 (로딩이 완료되면 삭제)
	
	i3Transform*	m_pTrans = nullptr;		//회전된 바디값을 가지고 있다..
	
	GLOBAL_CHARA_INFO	m_CharaInfoForLoad;	// 로드하기 위한 Chara Info // 로딩중에만 사용, 로드 후에는 CharaManager의 Info를 사용하게된다.
	GLOBAL_CHARA_INFO *	m_pTempCharaInfo = nullptr;	// CharaManager의 Info 로드 완료 후 이것으로 대체

	bool			m_bInBattle = false;
	bool			m_bThreadMode = false;
	bool			m_bUseDummyResource = false;

protected:
	/** \brief Frame LOD를 찾는다.*/
	INT32			_FindFrameLOD( i3Body * pBody);

	////////////////////////////////////////////////////////////////////////////////////////////////
	//									inline functions
protected:
	void			SetCallBack( AvatarLoadedCallback pCallBack)			{ m_pfGameNodeEvent = pCallBack; }
	void			SetLoadFinishCallback( AvatarLoadedCallback pCallBack)	{ m_pLoadFinishCallBack = pCallBack; }
	void			SetThreadMode( bool bMode)								{ m_bThreadMode = bMode; }

	void			_Destroy( void);
	
public:
	Avatar();
	virtual ~Avatar(void);

	virtual void	ResetChara( void) override;

	virtual void	AttachScene( void) override;
	virtual void	DetachScene( void) override;

	virtual void	ReturnInstance( void) override;

	void			LostDevice( void);
	void			Revive( i3RenderContext * pCtx);

	/** \brief 더미를 아바타 구성을 한 Res로 대체한다. */
	void			ApplyLoadedRes( void);

	bool			OnUpdateAvatarGen_InThread( void);

	void			Create( GLOBAL_CHARA_INFO* pInfo);

	void			SetPartsEquipInfo( const CHARA_PARTS_INFO * pPartsInfo);

	i3::rc_string	GetPartName( EQUIP::ePART part) const;

	const i3Body *		GetBody()		{ return m_pBody;}

	void			SetCharaInfoForLoad( GLOBAL_CHARA_INFO * pInfo);

	/** \brief 팀밴드 설정
		\param[in] TeamColor 팀색상 설정 true(팀색상 적용), false(적색으로만 적용)*/
	void			SetTeamBand( bool TeamColor = false);

	void			DisableTeamBand( void);

	// 
	void			ProcessCallback(void);
	void			LoadFinish( void);

	AvatarLoadedCallback GetCallBack( void)							{ return m_pfGameNodeEvent; }

	//파츠 셋팅
	void			SetParts( T_ItemID ItemID, bool bInLobby);
	void			SetParts( const CHARA_PARTS_INFO * pInfo, bool bInLobby, bool bForceRebuild = false);

	/** \brief 무기 설정
		\note 로비용 캐릭터에서만 호출합니다. */
	void			SetWeapon( WEAPON_SLOT_TYPE slot, T_ItemID Itemid, bool bIsSameWeapon);

	//파츠가 로드 되었나?
	bool			GetLoadedParts() const			{ return m_bLoaded; }
	void			SetLoadedParts( bool bVal)	{ m_bLoaded = bVal; }

	void			SetEquipResource( T_ItemID itemID, INT32 res_subtype);

	/** \brief Common Res로 등록 in Battle
		\desc Battle 진입시 로비에서 설정한 자신의 Res는 삭제하지 않도록 한다.*/
	void			KeepResource( void);

	/** \brief Headgear 종류에 따라 mask를 설정 */
	void			SetCharaHead( bool bAttachedHeadgear);

	void			Cmd_PrepareEquip( EQUIP::ePART part);

	/** \brief Dummyresource 사용 여부 */
	bool			IsDummyResource( void)		{ return m_bUseDummyResource; }

	void			SetReleaseQue( void)		{ m_bReleaseQue = true; }
	bool			isReleaseQue( void)			{ return m_bReleaseQue; }

	/** \brief Skin 사용 여부 */
	bool			IsSkinResource( void);

	/** \brief Eye Enable */
	virtual void	SetEyeEnable( bool enable) override;
};