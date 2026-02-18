/**
 * \file	GameCharaMaterialContext.h
 *
 * Declares the game chara material context class.
 */


#if !defined( __GAME_CHARA_LIGHT_CONTEXT_H__)
#define __GAME_CHARA_LIGHT_CONTEXT_H__

#include "GameCharaContextBase.h"
#include "GameCharaDef.h"

enum DOMINATION_BUFF_STATUS

{
	SKILL_UNKNOWN = 0,
	SKILL_ATTACK_ON =1,
	SKILL_ATTACK_OFF,
	SKILL_SPEED_ON,
	SKILL_SPEED_OFF,
};

class CGameCharaMaterialContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaMaterialContext, CGameCharaContextBase);
protected:

	// Volume Light를 위한...
	CHARA_MATERIAL	*	m_pMaterialTable = nullptr;
	INT32				m_MaterialTableCount = 0;
	
	i3AttrSet *			m_pAttrSet = nullptr;
	i3AttrSet *			m_pBlendCtrlAttrSet = nullptr;
	i3AttrSet *			m_pFresnelAttrSet = nullptr;
	i3TextureFilterAttr *	m_pTexFilterAttr = nullptr;
	i3ZWriteEnableAttr* m_pZWriteEnable = nullptr;
	i3BlendEnableAttr *	m_pBlendEnableAttr = nullptr;
	i3MaterialAttr *	m_pBlendMtlAttr = nullptr;
	i3FresnelAttr *		m_pFresnelAttr = nullptr;
	i3FaceCullModeAttr *	m_pFaceCullModeAttr = nullptr;

	i3Node *			m_pLeftEyeGlow = nullptr;
	i3Node *			m_pRightEyeGlow = nullptr;
	
	
	COLORREAL			m_TargetDiffuse = { 0.5f, 0.5f, 0.5f, 1.0f };
	COLORREAL			m_OldDiffuse = { 0.5f, 0.5f, 0.5f, 1.0f };
	REAL32				m_timeVolLit = 0.0f;
	REAL32				m_timeVolLitCheck = 0.0f;
	REAL32				m_fAlphaVal = 0.0f;

	REAL32				m_rFadeElapseTime = 0.0f;				// 페이드 효과

	i3Quad *			m_pTeamTag = nullptr;						// 팀 테그
#ifdef DOMI_TAG
	i3Quad *			m_pDominationTag = nullptr;				// 제압 태그
#endif

	bool				m_bPenalty = false;
	i3AmbientAttr *		m_pPenaltyAmbientAttr = nullptr;
	
	COLORREAL			m_Ambient = { 0.0f, 0.0f, 0.0f, 1.0f };

	i3::vector<i3TextureBindAttr*>	m_ClanMarkTextureBindList;			// 클랜 마크

public:
	i3TextureFilterAttr *		getTextureFilterAttr( void)				{ return m_pTexFilterAttr; }

public:
	CGameCharaMaterialContext() {}
	virtual ~CGameCharaMaterialContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	// change view
	virtual void	OnChange1PV( void) override;
	virtual void	OnChange3PV( void) override;

	///////////////////////////////////////////////////////////////////////////////////////////////////
	void			CreateMaterialTable( void);
	void			CreateTopAttrSet( void);
	void			CreateTeamTag( void);

	/** \brief Blend 여부를 설정합니다. */
	void			SetBlendEnable( bool bEnable = true);

	/** \brief ZWrite 여부를 설정합니다. */
	void			SetZWriteEnable(bool bEnable = true);
	void			SetBlendAlpha( REAL32 val);
	void			SetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular);
	void			setFaceCull(bool bFlag)
	{
		if( bFlag)
			m_pFaceCullModeAttr->Set( I3G_FACE_CULL_NONE);
		else
			m_pFaceCullModeAttr->Set( I3G_FACE_CULL_BACK);

	}
	/** \brief 왼손 모드를 설정합니다. */
	void			setLeftMode( bool bFlag)
	{
		if( bFlag)
			m_pFaceCullModeAttr->Set( I3G_FACE_CULL_NONE);
		else
			m_pFaceCullModeAttr->Set( I3G_FACE_CULL_BACK);
	}

	/** \brief Team Mark 활성을 설정합니다. */
	void			SetTeamMarkEnable( bool bEnable = true, INT32 Attack = 0, INT32 Speed = 0);

	void			SetClanMark( i3Texture * pTex);
	
	/** \breif Light Volume을 업데이트 합니다. */
	void			DriveLightVolume( REAL32 rDeltatime);

	/** \brief Material 처리를 합니다.
		\note Alpha 계산 */
	void			ProcessMaterial( REAL32 rDeltatime);



	void EnablePenalty();
	void DisablePenalty();

	bool IsPenalty() { return m_bPenalty == true; }
	i3ZWriteEnableAttr* GetZWriteAttr() { return m_pZWriteEnable; }
public:
	static COLORREAL COL_BLACK;
	static COLORREAL COL_WHITE;
	static COLORREAL COL_RED;
	static COLORREAL COL_GREEN;
	static COLORREAL COL_BLUE;
};

#endif
