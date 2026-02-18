/**
 * \file	GameCharaInputContext.h
 *
 * Declares the game chara input context class.
 */


#if !defined( __GAME_CHARA_INPUT_CONTEXT_H__)
#define __GAME_CHARA_INPUT_CONTEXT_H__

#include "GameCharaContextBase.h"
#include "GameCharaDef.h"

class CGameCharaInputControl;
class CGameCharaInputControlLocal;
class CGameCharaInputControlNetwork;
class CInputCtrlAI_ForHuman;
class CInputCtrlAI_ForDino;
class CGameCharaInputControlFullbody;

class CGameCharaInputContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaInputContext, CGameCharaContextBase);
protected:
	CGameCharaInputControl *		m_pCurrentInputControl = nullptr;

	CGameCharaInputControlLocal *	m_pLocalInputControl = nullptr;
	CGameCharaInputControlNetwork * m_pNetworkInputControl = nullptr;
	
	CInputCtrlAI_ForHuman *			m_pAIInputControl = nullptr;
	CInputCtrlAI_ForDino *			m_pAI_DinoInputControl = nullptr;

	CGameCharaInputControlFullbody*	m_pFullbodyInputControl = nullptr;

	GAME_CHARA_NETWORK_MOVE_TYPE	m_CheckMoveType = GAME_CHARA_NETWORK_STAND;

	UINT64						m_KeyPressInput = 0;
	UINT64						m_KeyStrokeInput = 0;

	REAL32						m_rInputMoveTheta = 0.0f;
	REAL32						m_rInputMoveRho = 0.0f;

public:
	CGameCharaInputControl *	getControl( void);

	GAME_CHARA_NETWORK_MOVE_TYPE getMoveType( void)					{ return m_CheckMoveType; }
	void						setMoveType( GAME_CHARA_NETWORK_MOVE_TYPE type)	{ m_CheckMoveType = type; }

	UINT64						getKeyPressInput( void)				{ return m_KeyPressInput; }
	void						setKeyPressInput( UINT64 key)		{ m_KeyPressInput = key; }

	UINT64						getKeyStrokeInput( void)			{ return m_KeyStrokeInput; }
	void						setKeyStrokeInput( UINT64 key)		{ m_KeyStrokeInput = key; }

	REAL32						getInputMoveTheta( void)			{ return m_rInputMoveTheta; }
	void						setInputMoveTheta( REAL32 rTheta)	{ m_rInputMoveTheta = rTheta; }
	REAL32						getInputMoveRho( void)				{ return m_rInputMoveRho; }
	void						setInputMoveRho( REAL32 rRho)		{ m_rInputMoveRho = rRho; }

public:
	CGameCharaInputContext() {}
	virtual ~CGameCharaInputContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;
	
	virtual void	OnChangeLocal( void) override;
	virtual void	OnChangeNetwork( void) override;
	virtual void	OnChangeNPC( void) override;
	virtual void	OnChangeAI( void) override;
	virtual void	OnChangeAI_Dino( void);
	virtual void	OnChangeFullbody() override;
	
	virtual void	OnFirstUpdate( REAL32 rDeltatime) override;
	virtual void	OnUpdate( REAL32 rDeltatime) override;

	/** \brief 상체 액션을 실행합니다. 
		\note 네트워크 인풋에 대한 처리 */
	void			ProcessActionUpper( CHARA_ACTION_UPPER upper );

	/** \brief Crouch, Stand 상태를 변경합니다. */
	void			ProcessActionBody( CHARA_ACTION_BODYLOWER body );

	/** \brief AttackIdle, Zoom 상태를 변경합니다. */
	void			ProcessActionBody2( CHARA_ACTION_BODYUPPER body2);

	/** \brief 위치 및 회전값을 설정합니다. */
	void			ProcessPosition( GAMEINFO_CHARA_NETDATA * pRecvInfo);
	void			ProcessRotationTheta( GAMEINFO_CHARA_NETDATA * pRecvInfo);
	void			ProcessRotationRho( GAMEINFO_CHARA_NETDATA * pRecvInfo);

	/** \brief 무기 발사를 처리합니다. */
	void			ProcessNetFire( GAMEINFO_CHARA_NETDATA * pRecvInfo);

	// 훈련전 AI 캐릭터
	void			BindAI(AI_BIND_DATA* pData);
};

#endif
